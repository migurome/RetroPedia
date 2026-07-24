---
titulo: "Bumbershoot Software — variaciones sobre el ABI/convención de llamada del 68000"
plataforma: [MegaDrive]
categoria: [cpu]
tags: [bumbershoot, 68000, abi, calling-convention, stack, movem, link, unlk, gcc]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/03/10/variations-on-the-68000-abi/]
---

# Bumbershoot Software — variaciones sobre el ABI/convención de llamada del 68000

## Resumen

Repaso de la convención de llamada (ABI) que usa GCC para m68k, con foco en cómo se pasan argumentos, qué registros sobreviven a una llamada, cómo se preservan varios registros de golpe con `MOVEM`, y los límites de esta convención (clausuras/funciones anidadas y eliminación de llamadas en cola). Es la referencia de bajo nivel que explica por qué el código de otras entradas de esta serie (p. ej. el "Hello World anotado") organiza el paso de parámetros y el uso de pila como lo hace.

## Detalle técnico

### Reglas básicas de la convención (GCC m68k)

1. **`A7`** es el registro de pila.
2. La pila es "**full descending**": el puntero de pila apunta siempre al elemento empujado más recientemente.
3. Los argumentos de una función se empujan a la pila **antes** de la llamada, empujando el **último argumento primero** (de derecha a izquierda).
4. Una llamada normal **destruye `A0`, `A1`, `D0` y `D1`**; todos los demás registros, incluido el puntero de pila, se preservan.
5. El **valor de retorno** se devuelve en `D0`.

### Ejemplo: llamar a `f(a,b)` con dos argumentos de word

```asm
move.w  #b, -(sp)
move.w  #a, -(sp)
jsr     f
addq    #4, sp
```

Dentro de `f`, como `jsr` empuja una dirección de retorno de un longword (4 bytes), el primer argumento (`a`) queda en `4(sp)` y el segundo (`b`) en `6(sp)`.

### Preservar varios registros de golpe con `MOVEM`

Para preservar más registros de los que la convención garantiza automáticamente, se usa `MOVEM`. Detalle importante: `MOVEM` siempre vuelca los registros a memoria en un **orden fijo**, **excepto en modo predecremento** (`-(sp)`), donde los registros se vuelcan en **orden inverso**. Esto es clave para que un `movem.l ...,-(sp)` de guardado y un `movem.l (sp)+,...` de restauración con la misma lista de registros sean simétricos y no mezclen los valores.

### Inicialización de la pila en Genesis

El límite lógico de pila que se suele usar en Genesis es `$01000000`, pero como el bus de direcciones solo decodifica los 24 bits bajos, `$00000000` es equivalente en la práctica **y más eficiente de generar** (no hace falta cargar una constante de 32 bits completa).

### Pilas de llamada y "backtraces"

Esta disciplina de pila (frames enlazados) es lo que permite reconstruir una traza de llamadas. A diferencia del 6502 (con solo 256 bytes de pila fijos), el 68000 permite llamadas "recursivas" limitadas únicamente por el agotamiento de la RAM disponible.

### Disciplina de frame pointer con `A6`

Ejemplo de prólogo manual para una función con 12 bytes de variables locales y 6 bytes de espacio auxiliar ("scratch"), preservando además `D2`-`D5`/`A2`-`A4`:

```asm
move.l  a6,(-sp)
move.l  sp, a6
sub.l   #12, sp
movem.l d2-d5/a2-a4, -(sp)
sub.l   #6, sp
```

Los argumentos empiezan en el offset 8 respecto a `A6` (los 4 bytes de `A6` guardado más los 4 bytes de la dirección de retorno empujada por `jsr`); las variables locales usan offsets negativos respecto a `A6`; el espacio scratch usa offsets positivos respecto a `sp`.

El epílogo revierte el proceso en orden inverso:

```asm
add.l   #6, sp
movem.l (sp)+, d2-d5/a2-a4
move.l  a6, sp
move.l  (sp)+, a6
```

El propio 68000 incluye instrucciones dedicadas que abrevian la parte de gestión del frame pointer (no el `MOVEM` de registros adicionales, que sigue haciendo falta aparte): **`LINK.W A6,#-12`** para el prólogo y **`UNLK A6`** para el epílogo.

### "Red zone" (mencionada como comparación, no presente en el 68000)

El artículo menciona, a modo de contraste, que la convención de x86_64 incluye una "red zone": una región de memoria por debajo del puntero de pila (128 bytes en x86_64) que el sistema operativo garantiza disponible, permitiendo a una función usarla como espacio de variables locales sin tener que ajustar `sp` primero. La convención de m68k aquí descrita no tiene equivalente: cualquier espacio local debe reservarse explícitamente restando de `sp` (o vía `LINK`).

### Límites de esta convención

- **Objetos función / clausuras (closures):** funciones anidadas que devuelven una clausura necesitarían que su frame de pila sobreviviera más allá del propio retorno de la función — incompatible con esta disciplina de pila simple sin recurrir a asignar el frame en el heap.
- **Eliminación de llamadas en cola (tail call elimination):** solo es viable en condiciones restringidas. Cita textual: *"si la función a la que haces la llamada en cola usa **más** memoria de la que usabas tú, quien llama no podrá saber cuánta pila propia necesita limpiar."* El artículo señala que la instrucción **`RTD`** (return and deallocate — retorna y además libera una cantidad fija de bytes de argumentos) podría facilitar este caso, "pero por desgracia no se introdujo en la familia 68000 hasta el chip siguiente al que usa la Genesis" (es decir, el 68010 en adelante; la Genesis usa un 68000 puro, sin `RTD`).

## Notas / gotchas

- El orden inverso de `MOVEM` en modo predecremento es una fuente común de errores sutiles si se cambia a mano la lista de registros de un guardado sin actualizar la lista correspondiente de restauración (o viceversa) — conviene mantener ambas listas idénticas y solo cambiar el modo de direccionamiento (`-(sp)` vs `(sp)+`).
- Usar `$00000000` en vez de `$01000000` como puntero de pila inicial es un micro-ahorro de inicialización específico de que Genesis solo decodifica 24 bits de dirección; no es portable tal cual a un 68000 con bus de direcciones completo de 32 bits.
- La ausencia de `RTD` en el 68000 (frente al 68010+) es una limitación de hardware real, no solo de convención — cualquier optimización de tail-call que dependa de esa instrucción simplemente no está disponible en Genesis.
- Esta entrada es la base conceptual de por qué el "Hello World anotado" de esta misma serie usa `movem.w`/`movem.l` para cargar de golpe registros de datos y de dirección al arrancar, en vez de una secuencia de `move` individuales.

## Referencias cruzadas

- [Hello World en código máquina puro para Mega Drive (anotado)](bumbershoot-hello-world-anotado.md) — código real de la misma serie que usa `MOVEM` para carga masiva de registros al arranque.
- [Motorola 68000 — instrucciones y modos de direccionamiento (base)](68000-instrucciones-modos-direccionamiento.md) — referencia de registros y modos de direccionamiento (incluido post-incremento/pre-decremento) que complementa el detalle de `MOVEM` aquí explicado.
- [Comparativa de toolchains de ensamblador para Mega Drive](../herramientas/bumbershoot-comparativa-toolchains.md) — SGDK usa GCC para m68k, precisamente el compilador cuya convención de llamada describe este artículo.

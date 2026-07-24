---
titulo: Re-optimización del CCA en Genesis en 2025 — contar ciclos en serio
plataforma: [MegaDrive]
categoria: [cpu]
tags: [cca, optimizacion, 68000, ciclos, lea, autoincremento, macros, snes, benchmark]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2025/06/28/re-optimizing-the-cca-on-the-genesis/]
---

# Re-optimización del CCA en Genesis en 2025 — contar ciclos en serio

## Resumen

Siete años después del proyecto original, Michael Martin retoma el CCA de Genesis con mucho más conocimiento acumulado de la CPU 68000 y del manual de ciclos de Motorola, y reoptimiza a fondo el mismo bucle documentado en la entrada de 2018 (ver referencias cruzadas). El objetivo explícito es igualar o superar la versión equivalente que había hecho para SNES, que partía con ventaja. Técnicas aplicadas: sustituir cálculos de dirección por `LEA`, aprovechar que el post-incremento de direccionamiento no cuesta ciclos extra, inlinear la comprobación de vecino como macro de ensamblador (eliminando el coste de `BSR`/`RTS`), y sustituir el cálculo dinámico de wraparound en bordes/esquinas por offsets constantes precalculados.

## Detalle técnico

### Contabilidad de ciclos como punto de partida

El autor parte de cifras concretas del manual de ciclos del MC68000 para justificar cada cambio:

- Acceso a memoria: mínimo 4 ciclos.
- `ADD` sobre registro de dirección: 8 ciclos.
- `BSR` (llamada a subrutina): 18 ciclos; `RTS` (retorno): 16 ciclos — 34 ciclos solo en ir y volver, antes de contar el cuerpo de la subrutina.
- `CLR.L` es más lento que cargar un registro a cero con `MOVEQ`.
- `DBRA` cuesta lo mismo que un salto incondicional.
- Instrucciones de desplazamiento: 6+2n ciclos (n = número de posiciones desplazadas).
- Direccionamiento indexado con desplazamiento `d(An,Dn)`: 6 ciclos.
- Post-incremento `(An)+` cuesta igual que el direccionamiento base `(An)` sin incremento — es "gratis".
- Pre-decremento `-(An)` cuesta 2 ciclos más que el post-incremento.

Con la CPU a 8MHz y la pantalla a 60Hz, el autor calcula cuántos ciclos representa ganar un frame completo por generación y usa esa cifra como vara de medir para el bucle del núcleo (unas 16000 iteraciones) más el bucle de bordes/esquinas (varios cientos de celdas adicionales).

### Bucle principal: de direccionamiento por registro de datos a LEA + post-incremento

Versión heredada de 2018 (equivalente a la de la entrada de optimización original, con las etiquetas en formato `.lp`/`.eat` en vez de `@lp`/`@eat`):

```asm
move.w  #(126*128-3), d2
movea.l a0, a2
add.w   #129, a2
movea.l a1, a3
add.w   #129, a3
.lp:    move.b  (a2), d4
move.b  d4, d5
addq    #1, d5
and.b   #$0f, d5
cmp.b   -128(a2), d5
beq.s   .eat
cmp.b   -1(a2), d5
beq.s   .eat
cmp.b   1(a2), d5
beq.s   .eat
cmp.b   128(a2), d5
bne.s   .next
.eat:   move.b  d5, d4
.next:  move.b  d4, (a3)
addq    #1, a2
addq    #1, a3
.lpend: dbra    d2, .lp
.done:
```

Versión reoptimizada: `movea.l` + `add.w` se sustituyen por un único `LEA` (mejora claridad y el coste de configuración inicial, sin afectar al bucle en caliente); dentro del bucle, la carga `move.b (a2), d4` + el `addq #1,a2` posterior se funden en un post-incremento `move.b (a2)+,d4`, y el camino donde la celda cambia de estado escribe directamente con post-incremento en vez de escribir y luego incrementar por separado:

```asm
move.w  #(126*128-3), d2
lea     129(a0),a2
lea     129(a1),a3
.lp:    move.b  (a2)+,d4
move.b  d4, d5
addq    #1, d5
and.b   #$0f,d5
cmp.b   -129(a2),d5
beq.s   .eat
cmp.b   -2(a2),d5
beq.s   .eat
cmp.b   (a2),d5
beq.s   .eat
cmp.b   127(a2),d5
bne.s   .skip
.eat:   move.b  d5, (a3)+
dbra    d2, .lp
bra.s   .done
.skip:  move.b  d4, (a3)+
dbra    d2, .lp
.done:
```

Nótese cómo cambian los desplazamientos de los `cmp.b` (-129, -2, sin offset, 127 — en vez de -128, -1, 1, 128 de la versión de 2018): al hacer el post-incremento de `a2` **antes** de las comparaciones, todos los offsets de vecino quedan recalculados relativos a la nueva posición del puntero. También se separan los dos caminos de salida (`.eat`/`.skip`), cada uno con su propio `move.b ...,(a3)+` y su propio `dbra`, para poder usar post-incremento de escritura en ambos sin una comprobación adicional compartida.

### Macro para inlinear la comprobación de vecino

En vez de una subrutina llamada por `BSR`/`RTS` por cada vecino —hasta 11 pares llamada/retorno por punto contando el bucle principal más el tratamiento de bordes/esquinas, con un coste citado de 374 ciclos solo en mecanismo de llamada/retorno, sin contar el cuerpo— se define una macro de ensamblador que se expande inline en cada punto de uso:

```asm
macro check_cell base, north, west, east, south
move.b  d4, d5
addq    #1, d5
and.b   #$0f,d5
cmp.b   north(base),d5
beq.s   .eat ## \?
cmp.b   west(base),d5
beq.s   .eat ## \?
cmp.b   east(base),d5
beq.s   .eat ## \?
cmp.b   south(base),d5
bne.s   .next ## \?
.eat ## \?
move.b  d5, d4
.next ## \?
endm
```

El sufijo `## \?` genera una etiqueta única en cada expansión de la macro, necesario para poder usarla varias veces en el mismo bloque sin colisión de nombres de etiqueta. Esto elimina de un plumazo el coste repetido de `BSR`/`RTS`, a cambio de un binario algo mayor (cada expansión duplica el cuerpo de la comprobación).

El autor también elimina 6 desplazamientos `LSL #7,D0` por punto (20 ciclos cada uno, 120 ciclos en total) que la vieja subrutina de cálculo de índice recalculaba para convertir coordenadas x,y a offset: ya no hacen falta, porque bordes y esquinas ahora usan offsets constantes precalculados en vez de recomputar el índice dinámicamente en cada acceso.

### Esquinas: offsets constantes en vez de wraparound dinámico

Las 4 esquinas de la rejilla toroidal (donde, por ejemplo, el vecino "de arriba" de la fila 0 es la fila 127) se resuelven con offsets constantes fijos aplicados directamente sobre el puntero base, evitando cualquier multiplicación o resto en tiempo de ejecución:

```asm
move.b  (a0),d4
check_cell a0, 1, 127, 128, $3f80
move.b  d4,(a1)
move.b  $7f(a0),d4
check_cell a0, 0, $7e, $ff, $3fff
move.b  d4,$7f(a1)
move.b  $3f80(a0),d4
check_cell a0, 0, $3f00, $3f81, $3fff
move.b  d4,$3f80(a1)
move.b  $3fff(a0),d4
check_cell a0, $7f, $3f7f, $3f80, $3ffe
move.b  d4,$3fff(a1)
```

### Bordes (no esquina): dos pares de punteros de inducción distintos

Los bordes horizontal (fila 0/127) y vertical (columna 0/127) avanzan a pasos distintos (1 byte a lo largo de una fila frente a 128 bytes para bajar una columna), así que se usan dos pares de punteros de inducción independientes: `a2`/`a3` para el barrido horizontal con post-incremento normal, y `a0`/`a1` para el barrido vertical, actualizados con `LEA 128(a0),a0` porque el paso es de 128 en 128:

```asm
lea     1(a0),a2
lea     1(a1),a3
lea     128(a0),a0
lea     128(a1),a1
moveq   #125,d2
.lp2:   move.b  $3f80(a2),d4
check_cell a2,0,$3f00,$3f7f,$3f81
move.b  d4,$3f80(a3)
move.b  (a2)+,d4
check_cell a2,-2,0,127,$3f7f
move.b  d4,(a3)+
move.b  (a0),d4
check_cell a0,-128,1,127,128
move.b  d4,(a1)
move.b  127(a0),d4
check_cell a0,-1,0,126,255
move.b  d4,127(a1)
lea     128(a0),a0
lea     128(a1),a1
dbra    d2,.lp2
```

Este único bucle procesa en cada iteración una celda del borde superior, una del inferior, una del izquierdo y una del derecho (unas 508 celdas de borde en total, descontando ya las 4 esquinas tratadas aparte).

### Idea explorada pero de retorno decreciente: desenrollado del bucle

El autor calcula que desenrollar el bucle principal (procesar 2 celdas por iteración de `DBRA` en vez de 1) ahorraría del orden de 48000 ciclos —aproximadamente un tercio de un frame completo a 8MHz/60Hz— al eliminar la mitad de las comprobaciones/decrementos de `DBRA`, pero anota que los ahorros de desenrollar más niveles van decreciendo frente a la complejidad y el tamaño de código añadidos; el post no deja claro si esta idea llegó a aplicarse en la versión final.

### Resultado: comparación con la versión de SNES

El autor había hecho antes una versión equivalente para SNES, que partía con cierta ventaja frente a la Genesis original de 2018 (~20%), medida en frames necesarios por generación en dos puntos de una misma ejecución de prueba (al principio, con la rejilla más activa, y al final, más asentada):

- SNES original: 22 frames/generación al principio, bajando a 16 al final.
- Genesis original (2018, resultado final de la entrada de optimización previa): 27 frames/generación al principio, bajando a 20 al final.
- Genesis tras la reoptimización de 2025: 20 frames/generación al principio, bajando a 15-16 al final — igualando o superando ligeramente a la SNES en ambos puntos de medida.

## Notas / gotchas

- La lección central frente a 2018: con más conocimiento de los costes reales de ciclos del 68000 (tabla del manual de Motorola), fue posible exprimir bastante más rendimiento del mismo algoritmo y las mismas estructuras de datos, cambiando solo la codificación de bajo nivel.
- Inlinear con macros de ensamblador en vez de subrutinas es el clásico intercambio "tamaño de binario por velocidad": ahorra 34+ ciclos de `BSR`/`RTS` por cada llamada eliminada, a cambio de un código más grande (cada expansión de la macro duplica el cuerpo).
- El post-incremento `(An)+` no cuesta nada extra sobre el direccionamiento base `(An)` en el 68000 — una de las optimizaciones "gratis" que conviene tener asumidas antes de tocar bucles calientes en este procesador.
- Cuando los offsets de vecino cambian de valor entre versiones del mismo bucle (p. ej. -128,-1,1,128 en 2018 frente a -129,-2,0,127 en 2025), suele ser porque cambia el punto de referencia (antes o después del post-incremento) sobre el que se calculan — fuente fácil de bugs sutiles al portar este tipo de optimización de un proyecto a otro.
- Separar los bordes horizontal/vertical en punteros de inducción distintos tiene sentido cuando sus incrementos naturales difieren mucho (1 byte vs. 128 bytes); forzar un único esquema de índice habría exigido lógica condicional dentro del bucle, contraproducente para el rendimiento buscado.

## Referencias cruzadas

- [Primer diseño e implementación del CCA en Genesis](bumbershoot-cca-primera-implementacion.md) — origen del algoritmo y de la estructura de datos que aquí se reoptimiza.
- [Optimización del bucle principal del CCA en 68000 (2018)](bumbershoot-cca-optimizacion-inicial.md) — el bucle de partida de esta entrada (27 frames/generación) es el resultado final de esa optimización previa.
- [Motorola 68000 — instrucciones y modos de direccionamiento (base)](68000-instrucciones-modos-direccionamiento.md) — referencia general de `LEA`, post-incremento/pre-decremento y direccionamiento indexado cuyos costes en ciclos se citan aquí.
- [Fundido de paleta y efecto "pixel bleed" por VSRAM (Bumbershoot / CCA)](../graficos/bumbershoot-cca-fade-pixel-bleed.md) — pulido visual del mismo port de 2025, hecho en paralelo a esta reoptimización de rendimiento.

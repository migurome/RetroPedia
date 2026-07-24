---
titulo: Primer diseño e implementación del Autómata Celular Cíclico (CCA) en Genesis
plataforma: [MegaDrive]
categoria: [cpu]
tags: [cca, automata-celular, buffers, doble-buffer, rng, boton-start, algoritmo, toroidal]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/04/15/genesis-first-draft-of-the-cca/]
---

# Primer diseño e implementación del Autómata Celular Cíclico (CCA) en Genesis

## Resumen

Michael Martin (Bumbershoot Software) implementa por primera vez, en ASM 68000 puro, el algoritmo del Autómata Celular Cíclico (CCA) para Genesis: una rejilla toroidal de 128×128 celdas con 16 estados posibles por celda, doble buffer en RAM intercambiado por bit en vez de copiado, un generador de números aleatorios para poblar la rejilla al pulsar START, y gestión de ese botón por flanco. Documenta también un bug de límites de bucle (constante hexadecimal usada donde tocaba decimal) que, pese a acceder muy fuera del rango previsto, no llegó a corromper memoria gracias al enmascarado de bits usado en el propio cálculo de índices.

## Detalle técnico

### La regla del autómata

Cada celda tiene un estado de 0 a 15. En cada generación, una celda calcula su "estado objetivo" como `(estado_actual + 1) mod 16`; si **alguno** de sus 4 vecinos ortogonales (arriba/abajo/izquierda/derecha) ya tiene ese estado objetivo, la celda pasa a él; si no, se queda igual. La rejilla es toroidal: los bordes conectan con el lado opuesto (el vecino "de arriba" de la fila 0 es la fila 127, etc.).

### Estructura de datos

- Rejilla de 128×128 celdas (16384 celdas), 1 byte por celda (solo se usan 4 bits, 16 estados).
- Doble buffer de 16KB cada uno (32KB en total, la mitad de la RAM disponible del 68000), ubicados en `$FF0000` en adelante. En vez de copiar los datos de un buffer a otro cada generación, se alterna cuál es "actual" invirtiendo el bit `$4000` del puntero mediante `EOR`:

```asm
        movea.l CurBuf, a0
        move.l  a0, d0
        eor.w   #$4000, d0
        move.l  d0, a1
        move.l  a1, CurBuf
```

- Cálculo de índice de celda `(x, y)` → offset plano: se enmascaran x e y a 7 bits cada uno (0-127; el enmascarado da el wraparound toroidal gratis, sin comparar límites) y se desplaza y 7 bits para formar un offset de 14 bits:

```asm
@index: move.w  d2, d0
        and.w   #$7f, d0
        lsl.w   #7, d0
        move.w  d3, d1
        and.w   #$7f, d1
        or.w    d1, d0
        rts
```

### Generador aleatorio y reinicio (botón START)

Al pulsar START se rellena el buffer actual con estados aleatorios (0-15), llamando a una rutina `rnd` (no detallada en el post) y extrayendo 4 nibbles de cada word de 16 bits que genera (4095 iteraciones × 4 celdas por word = 16384 celdas, la rejilla completa):

```asm
CCAReset:
        movem.l d2-d3/a2, -(sp)
        movea.l CurBuf, a2
        move.w  #4095, d2
@lp:    bsr     rnd
        moveq   #3, d3
@cell:  move.w  d0, d1
        and.b   #$0f, d1
        move.b  d1, (a2)+
        lsr.w   #4, d0
        dbra    d3, @cell
        dbra    d2, @lp
        move.b  #2, reset_requested
        movem.l (sp)+, d2-d3/a2
        rts
```

El botón START se gestiona por **flanco**, no por nivel: dentro de la interrupción vertical se compara el estado pulsado/no-pulsado actual contra el anterior usando dos bits de una variable `reset_requested` (bit 0 = "hay que reiniciar", bit 1 = "START seguía pulsado el frame anterior"), de forma que mantener pulsado START no dispare reinicios repetidos:

```asm
        move.b  reset_requested, d1
        btst    #7, d0
        bne.s   @start_pressed
        bclr    #1, d1
        bra.s   @start_processed
@start_pressed:
        btst    #1, d1
        bne.s   @start_processed
        bset    #0, d1
@start_processed:
        move.b  d1, reset_requested
```

`CCAStep` comprueba el bit 0 de `reset_requested` al empezar cada paso y, si está activo, salta a `CCAReset` en vez de calcular la siguiente generación:

```asm
CCAStep:
        move.b  reset_requested, d0
        btst    #0, d0
        bne     CCAReset
```

### Bucle principal (primera versión, sin optimizar)

Recorre las 128×128 celdas con dos bucles anidados (d2/d3 como contadores fila/columna), calculando el índice con `@index` tanto para la celda actual como para cada uno de sus 4 vecinos — el wraparound toroidal sale gratis del enmascarado a 7 bits, así que no hace falta lógica especial para los bordes en esta primera versión:

```asm
        move.w  #127, d2        ; 128 rows
@lp:    move.w  #127, d3        ; 128 columns
@lp2:   bsr.s   @index
```

```asm
        move.b  (a0, d0), d4
        move.b  d4, d5
        addq    #1, d5
        and.b   #$0f, d5
```

```asm
        subq    #1, d2
        bsr.s   @check
        addq    #2, d2
        bsr.s   @check
        subq    #1, d2
        subq    #1, d3
        bsr.s   @check
        addq    #2, d3
        bsr.s   @check
        subq    #1, d3
```

```asm
@check: bsr     @index
        cmp.b   (a0, d0), d5
        bne.s   @done
        move.b  d5, d4
@done:  rts
```

```asm
        bsr.s   @index
        move.b  d4, (a1, d0)
        dbra    d3, @lp2
        dbra    d2, @lp
```

### El bug de límites hex vs. decimal

En una versión previa (nunca publicada así) el bucle usaba `#$127` (hexadecimal, 295 en decimal) en vez de `#127` (decimal) como límite de fila/columna, haciendo que el contador recorriera 0-295 en vez de 0-127 — muy por encima del buffer de 16KB reservado. En teoría esto debería haber corrompido datos vecinos en RAM, pero como toda dirección se recalcula pasando por el enmascarado a 7 bits de `@index` antes de usarse, los accesos quedaron siempre dentro del buffer de 16KB pese a que el propio contador de bucle se saliera de rango. La consecuencia real no fue corrupción, sino un rendimiento pésimo: **558 frames por generación** (~0.11 generaciones/segundo) hasta corregir el bug, pasando a **124 frames por generación** (~0.48 gen/s) una vez arreglado.

## Notas / gotchas

- El enmascarado de índices (`and.w #$7f`) es justo lo que evitó que este bug se convirtiera en corrupción de memoria real: cualquier esquema que calcule offsets de array mediante máscaras de bits en vez de comparaciones de límites es tolerante a contadores de bucle fuera de rango, siempre que el cálculo de índice se aplique en cada acceso (no solo al entrar al bucle).
- El doble buffer con intercambio por bit (en vez de copia) es necesario porque el CCA necesita leer el estado completo de la generación anterior mientras escribe la nueva generación; reutilizar un único buffer para ambas cosas rompería la semántica del algoritmo.
- Gestionar START por flanco (detectando la transición pulsado→no antes de permitir un nuevo reinicio) evita que mantener pulsado el botón dispare reinicios en cadena.
- 124 frames por generación (~0.48 "fps" de la simulación) es el punto de partida que ataca directamente la siguiente entrada de optimización.

## Referencias cruzadas

- [Optimización del bucle principal del CCA en 68000 (2018)](bumbershoot-cca-optimizacion-inicial.md) — tres optimizaciones sucesivas sobre este mismo bucle, partiendo de estos 124 frames/generación.
- [Re-optimización del CCA en Genesis en 2025](bumbershoot-cca-reoptimizacion-2025.md) — retoma el algoritmo siete años después con más conocimiento del hardware.
- [Bugs de compatibilidad entre emuladores detectados al lanzar CCA](../herramientas/bumbershoot-cca-bugs-emuladores-lanzamiento.md) — problemas encontrados al pulir y publicar este mismo proyecto.
- [Fundido de paleta y efecto "pixel bleed" por VSRAM (Bumbershoot / CCA)](../graficos/bumbershoot-cca-fade-pixel-bleed.md) — pulido visual del mismo proyecto, hecho en 2025.
- [Motorola 68000 — instrucciones y modos de direccionamiento (base)](68000-instrucciones-modos-direccionamiento.md) — referencia de los modos de direccionamiento usados aquí (indexado por registro de datos, `dbra`, etc.).

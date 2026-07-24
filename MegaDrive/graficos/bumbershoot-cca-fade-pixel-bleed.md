---
titulo: Fundido de paleta y efecto "pixel bleed" por VSRAM (Bumbershoot / CCA)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [fade, fundido, paleta, cram, vsram, scroll, vdp, h-v-counter, punto-fijo]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2025/07/19/genesis-cca-special-effects-and-final-polish/]
---

# Fundido de paleta y efecto "pixel bleed" por VSRAM (Bumbershoot / CCA)

## Resumen

Michael Martin (Bumbershoot Software) documenta dos efectos visuales implementados al portar el juego *Cyclic Cellular Automaton* (CCA) a Genesis: un fundido de logo manipulando directamente la paleta (la Genesis, a diferencia de la SNES, no tiene un registro global de brillo de pantalla) y un efecto de "sangrado de píxeles" (pixel bleed) en la pantalla de título logrado desplazando el scroll vertical línea a línea vía VSRAM con espera activa sobre el contador V del VDP. También recoge varias correcciones de precisión de hardware surgidas al pulir el port.

## Detalle técnico

### Fundido de paleta (fade) sin registro de brillo

La SNES tiene registros de brillo de pantalla dedicados; el VDP de Genesis no, así que fundir a negro (o desde negro) implica interpolar manualmente los 16 valores de una paleta de 9 bits (3-3-3, ver `vdp-color-ramp.md` para la curva real de esos valores) a lo largo de 64 frames, usando aritmética de punto fijo con 6 bits fraccionarios. El proceso usa tres buffers de trabajo en RAM en `$FF0000`, `$FF0040` y `$FF0100`.

**Paso 1-2: separar canales y calcular el paso de fundido.** Para cada entrada de paleta se extraen los tres canales (R, G, B empaquetados en un word) mediante máscaras y desplazamientos de 1, 5 y 9 bits, y se guarda el valor "por frame" a restar (dividido entre 64, con 6 bits de precisión fraccionaria):

```asm
        lea     $ff0000,a0
        lea     64(a0),a1
        moveq   #31,d0
.fade1: move.w  (a0)+,d1
        move.w  d1,d2
        and.w   #$00e,d2
        lsr.w   #1,d2
        move.w  d2,(a1)+
        move.w  d1,d2
        and.w   #$0e0,d2
        lsr.w   #5,d2
        move.w  d2,(a1)+
        move.w  d1,d2
        and.w   #$e00,d2
        lsr.w   #8,d2
        lsr.w   #1,d2
        move.w  d2,(a1)+
        dbra    d0,.fade1
```

```asm
        moveq   #95,d0
.fade2: move.w  (a0)+,d1
        lsl.w   #6,d1
        move.w  d1,(a1)+
        dbra    d0,.fade2
```

**Paso 3-6: bucle de 64 iteraciones que resta el paso de fundido, espera a VBLANK (comprobando además si se pulsó START para poder saltarse el fundido) y reconstruye/vuelca la paleta a CRAM:**

```asm
        moveq   #63,d3                  ; Step 6: a 64-iteration loop
.fade6: lea     $ff0040,a0              ; Step 3: Subtract 1st buffer from 2nd
        lea     192(a0),a1
        moveq   #95,d0
.fade3: move.w  (a0)+,d1
        sub.w   d1,(a1)+
        dbra    d0,.fade3
```

```asm
.f4a:   move.w  (a3),d0                 ; Step 4: Wait for VBLANK, abort if START
        btst    #3,d0
        bne.s   .f4a
.f4b:   move.w  (a3),d0
        btst    #3,d0
        beq.s   .f4b
        addq    #1,d4                   ; Increment frame count
        bsr     ReadJoy1
        btst    #7,d0
        bne.s   .skip
```

```asm
        lea     $ff0100,a0              ; Step 5: Reassemble palette and blit to CRAM
        move.l  #$c0000000,(a3)         ; Write to CRAM starting at 0
        moveq   #31,d0
.fade5: move.w  (a0)+,d2
        lsr.w   #5,d2
        and.w   #$00e,d2
        move.w  (a0)+,d1
        lsr.w   #1,d1
        and.w   #$0e0,d1
        or.w    d1,d2
        move.w  (a0)+,d1
        lsl.w   #3,d1
        and.w   #$e00,d1
        or.w    d1,d2
        move.w  d2,(a2)
        dbra    d0,.fade5
        dbra    d3,.fade6
```

Al terminar (o al abortar por START), se toma el bus del Z80 y se fuerza su reset para detener la música, se limpia la pantalla y se restaura la paleta original desde el buffer sin modificar:

```asm
        ;; Stop playback by taking over the Z80 bus and forcing a reset
.skip:  move.w  #0,$a11200
        move.w  #256,$a11100

        ;; Clear screen
        move.l  #$40940003,(a3)
        move.w  #3071,d1
        moveq   #$00,d0
.cls:   move.l  d0,(a2)
        dbra    d1,.cls

        ;; Restore palette in CRAM
        move.l  #$c0000000,(a3)
        lea     $ff0000,a0
        moveq   #15,d0
.pfix:  move.l  (a0)+,(a2)
        dbra    d0,.pfix

        move.l  d4,d0                   ; Adjusted frame counter back to d0
        movem.l (sp)+,d2-d4/a2-a3
        rts
```

Al ser Genesis un color de 9 bits (3-3-3) frente a los 15 bits (5-5-5) de SNES, este fundido produce un "chunkeo"/distorsión de matiz apreciable en comparación con un fundido equivalente en SNES — es una limitación de precisión de color del hardware, no un fallo de la técnica.

### Pantalla de título: texto por control VRAM directo

El texto del título se codifica como una lista de comandos de control VRAM (dirección de escritura de 32 bits) seguidos de la cadena terminada en null, con relleno `even` para mantener alineación a 16 bits entre entradas:

```asm
.title: dc.l    $41940003
        dc.b    "BUMBERSHOOT SOFTWARE",0
        even
        dc.l    $42240003
        dc.b    "2025",0
        even
        dc.l    $43200003
        dc.b    "PRESENTS",0
        even
        dc.l    $469e0003
        dc.b    "THE CYCLIC",0
        even
        dc.l    $47160003
        dc.b    "CELLULAR AUTOMATON",0
        even
        dc.l    $4c140003
        dc.b    "PRESS START TO BEGIN",0
        even
        dc.l    0
```

Rutina que recorre la lista y hace blit de cada cadena a VRAM, cuidando la alineación par del puntero de origen:

```asm
        lea     .title,a0               ; Set up messages
.blit:  move.l  a0,d0                   ; Is address even?
        btst    #0,d0
        beq.s   .ok
        addq    #1,a0                   ; If not, advance one more
.ok:

        move.l  (a0)+,d0                ; Load next VRAM control
        beq.s   .ready                  ; Stop if done
        move.l  d0,(a3)
        moveq   #0,d0
.line:  move.b  (a0)+,d0                ; Load next byte
        beq.s   .blit                   ; Finish string if null
        move.w  d0,(a2)                 ; Write to VRAM
        bra.s   .line
```

### Efecto "pixel bleed" (sangrado) por scroll vertical línea a línea

Genesis no tiene un equivalente de HDMA (a diferencia de SNES), así que para conseguir un efecto de scroll distinto por línea de barrido hay que hacer que el 68000 espere activamente ("spin") sobre el contador H/V del VDP en `$C00008` y reescriba el scroll vertical (VSRAM, vía `$40000010`) en el instante justo antes de que el rayo dibuje cada línea objetivo.

Arranque del efecto: espera a VBLANK, fija el nametable correcto del plano A, comprueba START para poder abortar, y escribe el primer valor de scroll:

```asm
        moveq   #1,d3                   ; Scanline to start the bleed
.wait:  move.w  (a3),d0
        btst    #3,d0                   ; Wait for no VBLANK
        bne.s   .wait
.v:     move.w  (a3),d0
        btst    #3,d0                   ; Wait for VBLANK
        beq.s   .v
        move.w  #$8230,(a3)             ; Set correct Plane A nametable
        addq    #1,d2                   ; Increment frame count
        bsr     ReadJoy1
        btst    #7,d0                   ; Start pressed?
        bne.s   .exit                   ; If so, we're done here
        moveq   #0,d1                   ; Initial scroll value
        move.l  #$40000010,(a3)         ; Write scroll to VSRAM
        move.w  d1,(a2)
        cmp.b   #222,d3                 ; Bleed effect finished?
        beq.s   .wait                   ; If so, we're done
```

Espera activa hasta alcanzar la línea de barrido objetivo (`d3`), leyendo el contador V como **byte** (para quedarse solo con los 8 bits altos) y comprobándolo dos veces seguidas para descartar una lectura corrupta justo en el límite entre valores:

```asm
.bzz:   move.b  8(a2),d0                ; Otherwise spin till we reach line d3
        cmp.b   d3,d0
        bne.s   .bzz
        move.b  8(a2),d0                ; Double-check in case of corruption
        cmp.b   d3,d0
        bne.s   .bzz
```

A partir de ahí, en cada línea siguiente se decrementa el scroll y se vuelve a escribir en VSRAM, comprobando línea a línea hasta la 223 (el corte se hace en la 222 y no en la 223 para evitar saltarse un frame por culpa del propio VBLANK):

```asm
.bld:   subq    #1,d1
        move.l  #$40000010,(a3)         ; Write scroll to VSRAM
        move.w  d1,(a2)
```

```asm
.bz2:   cmp.b   8(a2),d0
        beq.s   .bz2
        addq    #1,d0
        cmp.b   #223,d0
        bne.s   .bld
```

```asm
.bot:   subq    #1,d1
        move.l  #$40000010,(a3)         ; Write final scroll to VSRAM
        move.w  d1,(a2)
        addq    #1,d3                   ; Start bleed one line down next frame
        bra.s   .wait
```

La línea de arranque del efecto (`d3`) avanza una posición cada frame, por lo que el "borde" del sangrado desciende progresivamente por la pantalla frame a frame.

### Correcciones de precisión de hardware (pulido final)

Al validar el port en varios emuladores de distinta fidelidad (GensKMod, DGen, Exodus, BizHawk) surgieron ajustes de precisión que no son específicos de los efectos anteriores pero se corrigieron en la misma pasada:

- **Reset del Z80:** debe mantenerse la línea de reset afirmada durante 192 ciclos como mínimo (ver `using-the-z80.md`); el autor consolidó el código de gestión del Z80, antes duplicado, en una única función.
- **Lectura segura del mando durante los efectos:** además de las 4 NOPs de asentamiento tras el strobe (los ~2µs documentados), la rutina detiene el Z80 (`$a11100`) mientras dura la lectura del mando y lo reanuda justo después, para evitar conflictos de bus con el acceso al chip de sonido que puedan producirse en medio de un efecto por software como el pixel bleed:

```asm
ReadJoy1:
        moveq   #$40,d0
        moveq   #0,d1
        lea     $a10003,a0              ; I/O Port 1 Data
        lea     $a11100,a1
        move.w  #$100,(a1)              ; Halt Z80
        move.b  d0,6(a0)                ; Configure data direction
        move.b  d0,(a0)                 ; Strobe controller
        nop                             ; Wait two microseconds
        nop
        nop
        nop
        move.b  (a0),d0                 ; Read buttons
        move.b  #$00,(a0)               ; Second strobe
        move.b  d0,d1
        and.b   #$0f,d1
        add.b   d0,d0
        and.b   #$60,d0
        or.b    d0,d1
        move.b  (a0),d0
        move.w  #0,(a1)                 ; Resume Z80
        add.b   d0,d0
        add.b   d0,d0
        asr.b   #2,d0
        and.b   #$90,d0
        or.b    d1,d0
        eor.b   #$ff,d0
        rts
```

- **Reset "suave" (botón RESET de la consola):** muchas implementaciones de arranque saltan la inicialización estándar en un soft reset. El autor ajustó su código de arranque para preservar la configuración del VDP y limpiar solo la RAM de la CPU en ese caso, permitiendo conservar datos como puntuaciones altas.
- El autor concluye que perseguir compatibilidad con emuladores poco precisos es contraproducente: ajustarse a las restricciones de hardware documentadas da mejor compatibilidad general en las plataformas (y emuladores) que sí son fieles al hardware real.

## Notas / gotchas

- La Genesis no tiene registro de brillo global: cualquier fundido de pantalla implica reescribir la paleta completa en CRAM frame a frame.
- El color de 9 bits (3-3-3) de Genesis hace que los fundidos de paleta muestren más distorsión de matiz que en SNES (15 bits); es inherente al hardware, no un error de la técnica de fundido en sí.
- El contador V (`$C00008`) debe leerse como **byte** para quedarse solo con la parte alta relevante, y conviene leerlo dos veces seguidas para protegerse de una lectura corrupta justo en el cambio de valor.
- Cortar el efecto de scroll por línea un poco antes del final de pantalla visible (222 en vez de 223) evita que el propio VBLANK haga que se salte un frame del efecto.
- No hay HDMA en Genesis: cualquier efecto "por línea de barrido" (como este pixel bleed) exige que el 68000 esté activamente ocupado esperando al contador V, a diferencia de SNES donde el hardware lo haría de forma pasiva.
- El reset del Z80 debe mantenerse un mínimo de 192 ciclos; leer el mando exige tanto las NOPs de asentamiento como detener el Z80 durante la lectura si se está en medio de una rutina sensible al timing del bus.

## Referencias cruzadas

- [Rampa de color del VDP](vdp-color-ramp.md) — la curva de salida real de los valores de CRAM que este fundido manipula; relevante para saber que la interpolación lineal de este fundido no corresponde 1:1 con brillo percibido.
- [Usando el Z80 en la Mega Drive](../cpu/using-the-z80.md) — procedimiento general de petición de bus y reset del Z80 (los 192 ciclos mínimos ya están documentados ahí).
- [Mandos de Mega Drive (3 y 6 botones)](../perifericos/controllers.md) — lectura estándar del mando; esta entrada añade el detalle concreto de detener el Z80 durante la lectura en medio de una rutina de efectos.

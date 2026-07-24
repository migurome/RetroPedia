---
titulo: "Hello World en código máquina puro para Mega Drive (anotado)"
plataforma: [MegaDrive]
categoria: [cpu]
tags: [68000, asm, hello-world, vasm, rom-header, vdp, dma, tmss, fundamentos]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/platform-guide-sega-genesis-mega-drive/]
---

# Hello World en código máquina puro para Mega Drive (anotado)

## Resumen

Programa mínimo en ensamblador 68000 puro (sin SGDK) que arranca una Mega Drive/Genesis, inicializa VDP/Z80/PSG a mano y escribe el texto "HELLO WORLD" en pantalla usando un tileset dibujado a mano bit a bit. Es el ejemplo de referencia de la guía de plataforma de Bumbershoot Software, pensado para ensamblarse con Vasm (`vasmm68k_mot -Fbin -o hello.bin hello.asm`) y ejecutarse directamente en cualquier emulador.

## Detalle técnico

### Código fuente completo (verbatim)

```asm
        org     $000000
        dc.l    0,RESET                 ; Vectors and ROM header
        dcb.l   62,IRQ                  ; Ignore every IRQ
        dc.b    "SEGA GENESIS    (C)BUMB 2025.APR"
        dc.b    "HELLO WORLD                                     "
        dc.b    "HELLO WORLD                                     "
        dc.b    "GM 00000000-00"
        dc.w    $043b                   ; Checksum
        dc.b    "J               "
        dc.l    $00000000,$00003fff     ; ROM limits
        dc.l    $00ff0000,$00ffffff     ; RAM limits
        dcb.b   64,32                   ; A bunch of stuff we don't support
        dc.b    "JUE             "

        ;; Bulk-init tables for d5-d7, a0-a4, and all 24 VSP registers
Init:   dc.w    $8000,$3fff,$0100
        dc.l    $00a00000,$00a11100,$00a11200,$00c00000,$00c00004
        dc.b    $04,$14,$30,$2c,$07,$54,$00,$00,$00,$00,$00,$00
        dc.b    $81,$2b,$00,$01,$01,$00,$00,$ff,$ff,$00,$00,$80

RESET:  lea     Init(pc),a5             ; Set up init-data pointer
        movem.w (a5)+,d5-d7             ; Load initial data values
        movem.l (a5)+,a0-a4             ; Load base addresses
        move.b  -$10ff(a1),d0           ; Appease Lockout Circuitry if needed
        andi.b  #$0f,d0
        beq.s   .verOK
        move.l  #$53454741,$2f00(a1)
.verOK: move.w  (a4),d0                 ; Reset VDP Status word
        moveq   #$00,d0                 ; Convenience Zero Register
        move.l  d0,a7                   ; Reset stack pointer

        moveq   #$17,d1                 ; Load VDP registers from data table.
.RegLp: move.b  (a5)+,d5                ; D5's value from the table manages
        move.w  d5,(a4)                 ; the high byte of the control word
        add.w   d7,d5                   ; and adding D7 updates it to the
        dbra    d1,.RegLp               ; value needed for the next register

        move.l  #$40000080,(a4)         ; Clear VRAM with the DMA that our
        move.w  d0,(a3)                 ; bulk VSP assignment configured
.DMALp: move.w  (a4),d1                 ; Then wait for it to finish
        btst    #1,d1
        bne.s   .DMALp

        move.w  d7,(a1)                 ; Halt the Z80

        move.l  #$81048f02,(a4)         ; Disable DMA, Word-advance in VRAM

        move.l  #$40000010,(a4)         ; Clear VSRAM
        moveq   #$13,d4
.VSClr: move.l  d0,(a3)
        dbra    d4,.VSClr

        move.b  #$9f,17(a3)             ; Silence the PSG
        move.b  #$bf,17(a3)
        move.b  #$df,17(a3)
        move.b  #$ff,17(a3)

        move    #$2700,sr               ; Disable Interrupts

main:   move.l  #$40200000,(a4)         ; Decode characters into VDP format
        lea     chars,a1                ; (This is more compact than just
        moveq   #$47,d0                 ;  storing it in native format,
.lp:    move.b  (a1)+,d1                ;  sadly enough...)
        moveq   #7,d2
.lp2:   asl.l   #4,d3
        asl.b   #1,d1
        bcc.s   .nobit
        ori.b   #1,d3
.nobit: dbra    d2,.lp2
        move.l  d3,(a3)
        dbra    d0,.lp

        move.l  #$c0000000,(a4)         ; Set Colors 0-1 to Black/White
        move.l  #$FFF,(a3)

        lea     msg,a1                  ; Display message
        move.l  #$46980003,(a4)
        moveq   #12,d0
.msglp: move.w  (a1)+,(a3)
        dbra    d0,.msglp

        move.w  #$8144,(a4)             ; Enable display
freeze: bra.s    freeze                 ; Loop forever

IRQ:    rte                             ; Ignore all interrupts

        ;; Message and character data
msg:    dc.w    1,2,3,3,4,5,0,6,4,7,3,8,9
chars:  dc.b    $00,$62,$62,$7e,$62,$62,$62,$00,$00,$7e,$60,$7c,$60,$60,$7e,$00
        dc.b    $00,$60,$60,$60,$60,$60,$7e,$00,$00,$3c,$62,$62,$62,$62,$3c,$00
        dc.b    $00,$00,$00,$00,$00,$00,$18,$30,$00,$62,$62,$62,$6a,$76,$62,$00
        dc.b    $00,$7c,$66,$7c,$68,$64,$62,$00,$00,$7c,$62,$62,$62,$62,$7c,$00
        dc.b    $00,$18,$18,$18,$18,$00,$18,$00

        org     $003fff                 ; Expand binary to 16KB
        dc.b    0
```

Compilación: `vasmm68k_mot -Fbin -o hello.bin hello.asm`.

### Explicación por bloques (en mis propias palabras)

**Cabecera y vectores (`org $000000` hasta antes de `Init`).** Los dos primeros longwords son la tabla de vectores mínima del 68000: valor inicial de stack (0, se corrige más adelante) y dirección de arranque (`RESET`). `dcb.l 62,IRQ` rellena el resto de los 64 vectores de interrupción apuntando todos a una rutina que simplemente hace `rte` (no se usa ninguna interrupción en este ejemplo). A continuación viene la cabecera oficial de Sega en `$100` en adelante: nombre de sistema, copyright/fecha, títulos doméstico e internacional, número de serie, checksum (`$043b`, precalculado para este binario concreto), soporte de región (`J`), rangos de ROM/RAM y el campo de soporte de dispositivos. El formato exacto de cada campo ya está documentado con más detalle en `../memoria/rom-header.md`.

**Tabla `Init`.** Es un bloque de datos compacto pensado para cargarse de un tirón con `movem`: primero tres words (`$8000`, `$3fff`, `$0100`) que van a d5-d7, luego cinco longwords de direcciones base de hardware que van a a0-a4 (`$00a00000` Z80, `$00a11100`/`$00a11200` control de bus del Z80, `$00c00000` puerto de datos del VDP, `$00c00004` puerto de control del VDP), y por último los 24 bytes de valores para los registros del VDP más un longword de comando DMA (`$40000080`, usado después para el clear de VRAM).

**Arranque (`RESET`).** `lea Init(pc),a5` deja el puntero de lectura al principio de la tabla; los dos `movem` consecutivos cargan de golpe d5-d7 y a0-a4 sin tener que escribir seis/ocho instrucciones `move` sueltas. El bloque siguiente (`move.b -$10ff(a1),d0` / `andi.b #$0f,d0` / `beq.s .verOK` / `move.l #$53454741,$2f00(a1)`) es la comprobación de TMSS: si el nibble de versión de hardware leído es distinto de cero, escribe el longword `$53454741` (que son los caracteres ASCII "SEGA") en el puerto correspondiente para que la BIOS deje continuar la ejecución. Después se lee `(a4)` para resetear el registro de estado del VDP, se pone d0 a cero como "registro de conveniencia" reutilizado durante todo el programa, y se usa para inicializar el stack pointer (`a7`).

**Carga de los 24 registros del VDP.** El bucle `.RegLp` recorre `d1` de 23 a 0 (`moveq #$17,d1` + `dbra`). En cada vuelta toma un byte de la tabla (el valor del registro), lo mete en la mitad baja de d5 y lo escribe como word en el puerto de control del VDP — el truco es que d5 ya trae precargado en su mitad alta el patrón de comando (`$8000` visto en `Init`) y en cada iteración se le suma d7 (`$0100`), que incrementa en uno el número de registro VDP objetivo para la siguiente escritura. Es la misma técnica de "un valor base + incremento" que aparece en otras implementaciones de este mismo paso (ver referencia cruzada a `bec-vdp-init.md`, que usa un bucle equivalente con d1 como incremento en vez de d7).

**Limpieza de VRAM vía DMA.** `move.l #$40000080,(a4)` dispara una operación DMA de llenado/copia hacia VRAM (el comando ya estaba precalculado en la tabla `Init`); escribir `d0` (cero) en el puerto de datos (`a3`) arranca el llenado con ceros. El bucle `.DMALp` sondea el bit 1 del registro de estado del VDP hasta que la DMA termina.

**Apagado del Z80 y ajustes finales de VDP.** `move.w d7,(a1)` escribe `$0100` en el puerto de bus-request del Z80 para dejarlo detenido (este ejemplo no lo usa para nada, a diferencia del startup code "real" de Sega — ver la entrada de análisis del startup code en `../avanzado/`). Luego se desactiva la DMA y se configura auto-incremento de una palabra en VRAM (`$81048f02`), se limpia la VSRAM completa con otro DMA fill (`$40000010` + bucle `.VSClr` de 20 longwords) y se silencia el PSG escribiendo secuencialmente `$9f`, `$bf`, `$df`, `$ff` en su puerto (offset 17 sobre `a3`, que coincide con la dirección del PSG en el mapa de memoria). Finalmente `move #$2700,sr` deshabilita todas las interrupciones enmascarables.

**Dibujo de caracteres (`main`).** En lugar de guardar el tileset ya en el formato nativo de 4 bits por píxel del VDP, el autor lo almacena como bitmap 1bpp compacto (`chars`) y lo "expande" en tiempo de ejecución: por cada byte de entrada, el bucle interno `.lp2` desplaza cada bit hacia una palabra de 32 bits (`d3`), poniendo a 1 el nibble correspondiente cuando el bit desplazado es 1 (`asl.b #1,d1` + `bcc.s .nobit` + `ori.b #1,d3`). El resultado por byte es un longword que se escribe directamente en el puerto de datos del VDP (ya configurado en modo escritura de VRAM vía `$40200000`). El comentario original del autor admite que es "más compacto que simplemente guardarlo en formato nativo, por desgracia" — es decir, un ahorro de espacio en el binario a cambio de más instrucciones de CPU.

**Paleta y mensaje.** `$c0000000` apunta la siguiente escritura a CRAM desde la entrada de color 0, y `$FFF` en el puerto de datos fija los colores 0 y 1 a negro y blanco. Luego se posiciona la escritura en la tabla de nombres del plano A (`$46980003`) y se vuelca `msg` (una lista de índices de carácter, no de texto ASCII directo) palabra a palabra en el puerto de datos — cada word en `msg` es el índice de tile (dentro de los que se generaron en el bloque anterior) que ocupará esa celda de la tilemap.

**Encendido de pantalla y bucle infinito.** `$8144` es el valor final que se escribe en el registro de control 1 del VDP para habilitar la salida de vídeo (bit de display-enable activado). `freeze: bra.s freeze` es el bucle infinito típico de un ejemplo mínimo sin lógica de juego.

## Notas / gotchas

- Este `RESET` es una versión **simplificada** pensada solo para un ejemplo didáctico: no reinicializa el Z80 con un programa propio (solo lo detiene), no limpia la RAM de trabajo del 68000, y no comprueba si el reset ocurrió con el juego ya en marcha (comportamiento que sí implementa el startup code real de Sega usado en ROMs comerciales — ver referencia cruzada). No usar este código tal cual como base de un proyecto real sin añadir esos pasos.
- El checksum `$043b` en la cabecera es específico de este binario exacto; cambiar una sola instrucción del programa invalida ese valor y hay que recalcularlo.
- La técnica de expandir un tileset 1bpp a 4bpp en tiempo real (bloque `main`) es un ejemplo de trade-off tamaño-de-ROM vs. ciclos de CPU — válido para un "Hello World" que solo dibuja unas pocas letras una vez, pero no escalaría bien como método general para cargar gráficos en un juego real (para eso se usa DMA de datos ya en formato VDP, ver `../graficos/dma-transfer.md`).
- La secuencia de inicialización del VDP mediante tabla + bucle con incremento (`.RegLp`) es una variante más comentada por el propio autor del mismo patrón que aparece en `../graficos/bec-vdp-init.md`; comparar ambas si hace falta entender por qué distintos ejemplos "de referencia" difieren ligeramente en el orden de pasos.

## Referencias cruzadas

- [Comparativa de toolchains de ensamblador para Mega Drive](../herramientas/bumbershoot-comparativa-toolchains.md) — describe el ensamblador (Vasm) y el comando exacto usado para compilar este código.
- [Cabecera de la ROM](../memoria/rom-header.md) — formato completo de los campos que ocupan `$100`-`$1FF` en este listado.
- [Big Evil Corporation — inicialización de hardware al arranque](../graficos/bec-vdp-init.md) — otra implementación comentada de la misma secuencia de boot (TMSS + VDP + Z80 + PSG), con datos y orden ligeramente distintos.
- [Análisis del startup code real de Sega](../avanzado/bumbershoot-analisis-startup-code-sega.md) — versión "de producción" mucho más completa del mismo tipo de código de arranque, con inicialización real del Z80 y limpieza de RAM.

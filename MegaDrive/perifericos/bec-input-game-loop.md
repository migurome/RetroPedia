---
titulo: Lectura de mando y bucle principal (game loop) — enfoque Big Evil Corporation
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [bigevilcorporation, tanglewood, mando, game-loop, vblank, interrupciones]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/04/30/sega-megadrive-7-gamepad-input-and-the-game-loop/]
---

# Lectura de mando y bucle principal (game loop) — enfoque Big Evil Corporation

## Resumen

Séptima entrada de la serie de desarrollo de Matt (Big Evil Corporation, autor de TANGLEWOOD), publicada el 30/04/2012. Muestra cómo leer un mando de 3 botones desde ensamblador 68000 y cómo estructurar un bucle principal de juego sincronizado con el vblank del VDP, usando interrupciones para llevar la cuenta de fotogramas.

## Detalle técnico

### Lectura del mando (3 botones)

El protocolo descrito son cinco pasos: leer el byte alto del puerto de datos del mando (devuelve el estado de Start/A), rotar ese valor a la parte alta de un registro de 16 bits, escribir `$40` en el puerto de datos para forzar la línea alta, leer el byte bajo del puerto (devuelve C/B/derecha/izquierda/abajo/arriba), y finalmente restaurar el puerto de datos a `$00`. El autor solo implementa mandos de 3 botones en este artículo; advierte que un mando de 6 botones necesitaría ciclos de lectura adicionales.

```
ReadPad1:
   move.b pad_data_a, d0    ; Leer byte superior del puerto de datos
   rol.w  #0x8, d0          ; Mover al byte superior de d0
   move.b #0x40, pad_data_a ; Escribir bit 7 al puerto de datos
   move.b pad_data_a, d0    ; Leer byte inferior del puerto de datos
   move.b #0x00, pad_data_a ; Restaurar puerto de datos a normal
   rts
```

Formato del valor devuelto en d0: `00SA0000 00CBRLDU`. Definiciones de bits usadas en el resto del artículo:

```
pad_button_up    = bit 0
pad_button_down  = bit 1
pad_button_left  = bit 2
pad_button_right = bit 3
pad_button_b     = bit 4
pad_button_c     = bit 5
pad_button_a     = bit 12 (0xC)
pad_button_start = bit 13 (0xD)
```

Un lector comentó en su día que hacen falta dos instrucciones `NOP` entre la escritura al puerto y la lectura siguiente, para dar tiempo al hardware a responder (compatibilidad con chips de control más lentos que los de una Mega Drive real).

### Espera de vblank

Para sincronizar el juego con el refresco de pantalla, el autor sondea el registro de estado del VDP hasta que cambia el bit de vblank (bit 3, máscara `0x0008`):

```
WaitVBlankStart:
   move.w vdp_control, d0
   andi.w #0x0008, d0
   bne    WaitVBlankStart
   rts

WaitVBlankEnd:
   move.w vdp_control, d0
   andi.w #0x0008, d0
   beq    WaitVBlankEnd
   rts
```

### Bucle principal de ejemplo

El ejemplo mueve un sprite por pantalla leyendo el mando cada iteración, actualiza la posición fuera de vblank, y solo escribe las coordenadas del sprite (a través de `SetSpritePosX`/`SetSpritePosY`) dentro de la ventana de vblank para evitar tearing:

```
move.l #0x80, d4 ; Posición X en d4
move.l #0x80, d5 ; Posición Y en d5

GameLoop:
 jsr ReadPad1 ; Leer estado del pad 1, resultado en d0

 move.l #0x1, d6              ; Velocidad de movimiento por defecto
 btst   #pad_button_a, d0     ; Verificar botón A
 bne    @NoA
 move.l #0x2, d6              ; Duplicar velocidad si A presionado
 @NoA:

 btst   #pad_button_right, d0
 bne    @NoRight
 add.w  d6, d4                ; Incrementar posición X
 @NoRight:

 btst   #pad_button_left, d0
 bne    @NoLeft
 sub.w  d6, d4                ; Decrementar posición X
 @NoLeft:

 btst   #pad_button_down, d0
 bne    @NoDown
 add.w  d6, d5                ; Incrementar posición Y
 @NoDown:

 btst   #pad_button_up, d0
 bne    @NoUp
 sub.w  d6, d5                ; Decrementar posición Y
 @NoUp:

 jsr    WaitVBlankStart ; Esperar inicio de vblank
 move.w #0x0, d0        ; ID del sprite
 move.w d4, d1          ; Coordenada X
 jsr    SetSpritePosX   ; Establecer X
 move.w d5, d1          ; Coordenada Y
 jsr    SetSpritePosY   ; Establecer Y
 jsr    WaitVBlankEnd   ; Esperar fin de vblank
 jmp    GameLoop        ; Volver al inicio
```

### Contadores de fotogramas vía interrupción

El autor introduce, por primera vez en la serie, el uso de RAM del sistema para contadores incrementados desde los manejadores de interrupción de hblank y vblank:

```
HBlankInterrupt:
   addi.l #0x1, hblank_counter
   rte

VBlankInterrupt:
   addi.l #0x1, vblank_counter
   rte
```

```
WaitFrames:
   move.l  vblank_counter, d1
   @Wait:
   move.l  vblank_counter, d2
   subx.l  d1, d2
   cmp.l   d0, d2
   bge     @End
   jmp     @Wait
   @End:
   rts
```

Para habilitar todos los niveles de interrupción en el registro de estado del 68000, usa:

```
move #0x2000, sr
```

### Direcciones y valores clave

| Elemento | Valor |
|---|---|
| Puerto de datos, mando 1 | `$A10003` |
| Puerto de datos, mando 2 | `$A10005` |
| Bit de activación de línea TH | `$40` |
| Máscara de bit vblank en registro de estado VDP | `$0008` |
| Posición inicial de sprite (X e Y) | `$80` |
| Inicio de RAM libre usado para contadores | `$FF0000` (hblank), `$FF0004` (vblank) |

## Notas / gotchas

- El autor razona explícitamente contra el uso de delta-time: prefiere velocidades fijas ("hardcodeadas") atadas al vsync (50 Hz PAL / 60 Hz NTSC), asumiendo que todo el trabajo del frame cabe antes del siguiente vblank; si no cabe, el juego cae a la mitad de framerate en vez de sufrir slowdown gradual.
- Las actualizaciones de posición de sprites deben ocurrir dentro de la ventana de vblank; hacerlo fuera provoca tearing visible.
- Esta entrada solo cubre mandos de 3 botones; la lectura de 6 botones requiere pasos adicionales (ver el artículo posterior de PSG/sonido de la misma serie no cubre esto — comparar con la entrada de mandos ya existente en esta base, más completa en ese aspecto).
- Es la primera vez en la serie del autor que se usa RAM del sistema para variables de estado (contadores de interrupción), lo cual introduce la necesidad de un mapa de memoria explícito en artículos posteriores.

## Referencias cruzadas

- [`controllers.md`](controllers.md) (fuente plutiedev.com) — cubre el mismo protocolo de lectura de mando de forma más completa (incluye el mando de 6 botones y detección de tipo de mando); esta entrada es el enfoque equivalente pero más temprano/simplificado de un desarrollador con juego comercial publicado.
- [`../graficos/segaretro-interrupts.md`](../graficos/segaretro-interrupts.md) — detalle de los niveles de interrupción VInt/HInt del 68000 que este artículo usa de forma práctica sin profundizar en la teoría.

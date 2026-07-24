---
titulo: Animación de sprites — enfoque Big Evil Corporation
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bigevilcorporation, tanglewood, sprites, animacion, vram, dma]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/05/05/sega-megadrive-8-animated-sprites/]
---

# Animación de sprites — enfoque Big Evil Corporation

## Resumen

Octava entrada de la serie de Matt (Big Evil Corporation, autor de TANGLEWOOD), publicada el 05/05/2012. Explica tres estrategias posibles para animar sprites en Mega Drive según el compromiso VRAM/velocidad, y desarrolla en detalle la que elige el autor: mantener un único frame en VRAM y recargarlo desde ROM en cada cambio de fotograma, usando una tabla de datos de animación.

## Detalle técnico

### Tres estrategias posibles

1. **Todos los frames en VRAM a la vez**: la más rápida en tiempo de ejecución (solo hay que cambiar el ID de tile del sprite), pero cara en espacio — el autor calcula que una animación corta ya consume unos 4 KB de VRAM.
2. **Recargar un único frame desde ROM en cada vblank**: económica en VRAM, más lenta por la transferencia constante. Es la que desarrolla el artículo, elegida "sin datos que lo respalden" por simplicidad de código.
3. **Cachear en RAM primero**: solución intermedia entre las dos anteriores, mencionada pero no implementada en este artículo.

### Datos de sprite y constantes derivadas

El sprite de prueba son 4 fotogramas de 32×32 px (16 tiles cada uno) con las letras S-E-G-A. El autor deja que el ensamblador derive automáticamente tamaños en bytes/tiles/frames a partir de las etiquetas de inicio y fin de los datos, evitando constantes mágicas:

```
SegaLogo:
dc.l    $00000000
dc.l    $00000000
dc.l    $00000000
dc.l    $00000000
dc.l    $00000000
dc.l    $00000001
dc.l    $00000001
dc.l    $00000011
; [resto del frame 1...]

SegaLogoTile2:
dc.l    $00000000
dc.l    $00000000
dc.l    $00000000
dc.l    $00000000
dc.l    $00000000
dc.l    $00000001
dc.l    $00000001
dc.l    $00000011
; [etc...]

SegaLogoEnd
SegaLogoSizeB:      equ (SegaLogoEnd-SegaLogo)
SegaLogoSizeT:      equ (SegaLogoSizeB/32)
SegaLogoSizeF:      equ (SegaLogoSizeT/16)
SegaLogoOneFrameT:  equ (SegaLogoSizeT/SegaLogoSizeF)
SegaLogoOneFrameB:  equ (SegaLogoSizeB/SegaLogoSizeF)
SegaLogoTileID:     equ (SegaLogoVRAM/32)
SegaLogoDimentions: equ (%1111)
SegaLogoVRAM:       equ Sprite2VRAM+Sprite2SizeB
```

### Tabla de datos de animación

Estructura mínima: un array de bytes donde cada byte es el índice de fotograma a mostrar en ese "paso" de la animación. Permite hasta 256 fotogramas distintos, no exige que los fotogramas se muestren en orden, y comprime bien por tener valores repetidos:

```
SegaLogoAnimData:
dc.b    0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3

SegaLogoAnimDataEnd
SegaLogoAnimNumFrames: equ (SegaLogoAnimDataEnd-SegaLogoAnimData)
```

### Mapa de memoria RAM usado

```
SizeByte:    equ 1
SizeWord:    equ 2
SizeLong:    equ 4
SizeTile:    equ 64
SizePalette: equ 64

hblank_counter:      equ 0x00FF0000
vblank_counter:      equ (hblank_counter+SizeLong)
segalogo_anim_frame: equ (vblank_counter+SizeByte)
```

### Rutina de avance de animación (AnimateSpriteFwd)

Parámetros: `d0.w` = dirección VRAM del sprite, `d1.w` = tamaño de un frame en bytes, `d2.w` = número de frames de animación, `a0` = datos de sprite en ROM, `a1` = datos de animación en ROM, `a2` = dirección del contador de frame actual en RAM (variable, escribible).

Lógica: lee el índice de frame actual desde RAM, incrementa el contador con wraparound al llegar al final de la tabla de animación, compara el frame actual con el anterior y, solo si cambió, calcula el offset dentro de ROM (índice × tamaño de frame) y dispara la carga a VRAM.

```
AnimateSpriteFwd:
  ; Parámetros:
  ; d0 (w) - Dirección VRAM del sprite
  ; d1 (w) - Tamaño de un frame en bytes
  ; d2 (w) - Número de frames de animación
  ; a0 --- Dirección de datos de sprite (ROM)
  ; a1 --- Dirección de datos de animación (ROM)
  ; a2 --- Dirección del contador de frame actual (RAM, escribible)

  clr.l  d3
  move.b (a2), d3
  addi.b #0x1, (a2)
  cmp.b  d3, d2
  bne    @NotAtEnd
  move.b #0x0, (a2)
  @NotAtEnd:

  move.b (a1,d3.w), d4
  move.b (a2), d2
  move.b (a1,d3.w), d5

  cmp.b  d3, d4
  beq    @NoChange

  move.l a0, d2
  move.w d1, d4
  mulu.w d5, d4
  add.w  d4, d2
  move.l d2, a0

  jsr LoadTiles

  @NoChange:
  rts
```

### Carga del primer frame y entrada en la tabla de sprites

```
lea      SegaLogo, a0
move.l   #SegaLogoVRAM, d0
move.l   #SegaLogoOneFrameT, d1
jsr      LoadTiles
```

Entrada de 8 bytes en la Sprite Attribute Table (Y, dimensiones, enlace al siguiente sprite, flip/paleta/prioridad, tile ID inicial, X):

```
dc.w 0x0000
dc.b SegaLogoDimentions
dc.b 0x00
dc.b 0x40
dc.b SegaLogoTileID
dc.w 0x0000
```

### Llamada por vblank

```
move.l  #SegaLogoVRAM, d0
move.l  #SegaLogoOneFrameB, d1
move.l  #SegaLogoAnimNumFrames, d2
lea     SegaLogo, a0
lea     SegaLogoAnimData, a1
lea     segalogo_anim_frame, a2
jsr     AnimateSpriteFwd
```

## Notas / gotchas

- Solo hay un frame en VRAM en cualquier momento dado con esta técnica; esto limita la técnica a animaciones donde el coste de recarga por vblank (tamaño de un frame en tiles) es asumible dentro del presupuesto de VRAM/DMA del frame.
- El código está deliberadamente escrito para pasar todos los parámetros por registros; el propio autor anota que si se necesitaran más parámetros de los que caben en registros, habría que pasar a usar la pila.
- El autor deja mejoras pendientes explícitas: una versión `AnimateSpriteRev` (reproducción hacia atrás) y un parámetro de velocidad de animación (control de cuántos vblanks dura cada frame), ninguna implementada en este artículo.
- Nota de mantenimiento: el propio autor señala en el artículo siguiente de la serie (mapas y scroll) que este código tenía un error en la cantidad de bytes copiados al actualizar frames de animación.

## Referencias cruzadas

- [`sprites.md`](sprites.md) (fuente plutiedev.com) — formato de la Sprite Attribute Table y límites de hardware de sprites; esta entrada complementa con una técnica concreta de animación por recarga desde ROM usada en un juego comercial real.
- [`dma-transfer.md`](dma-transfer.md) — relevante para entender el coste de la recarga de tiles por frame que esta técnica requiere.

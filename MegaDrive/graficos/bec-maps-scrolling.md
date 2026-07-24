---
titulo: Mapas y planos de scroll — enfoque Big Evil Corporation
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bigevilcorporation, tanglewood, vdp, scroll, mapas, planos, vram]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/05/30/sega-megadrive-9-maps-and-scrolling-planes/]
---

# Mapas y planos de scroll — enfoque Big Evil Corporation

## Resumen

Novena entrada de la serie de Matt (Big Evil Corporation, autor de TANGLEWOOD), publicada el 30/05/2012. Cubre cómo construir un nivel completo como mapa de tiles cargado en el plano A del VDP, el pipeline de herramientas (GIMP, BMP2Tile, MMM) usado para generarlo, y cómo implementar scroll horizontal de pantalla completa sincronizado con el vblank.

## Detalle técnico

### Formato de una celda de mapa

Cada palabra (16 bits) de los datos de mapa contiene, según describe el autor: bits 0-10 el ID del tile, bits 11-13 el ID de paleta, bits 14-15 flags de inversión (flip horizontal/vertical). Al cargar un mapa hay que reescribir el ID de paleta original por el que corresponda en el juego actual, y sumar un offset al ID de tile si el tileset no arranca en la dirección VRAM `$0000`.

### Rutina de carga de mapa al plano A

```
LoadMapPlaneA:
 ; a0 (l) - Map address (ROM)
 ; d0 (b) - Size in words
 ; d1 (b) - Y offset
 ; d2 (w) - First tile ID
 ; d3 (b) - Palette ID

 mulu.w  #0x0040, d1            ; Multiply Y offset by line width (in words)
 swap    d1                     ; Shift to upper word
 add.l   #vdp_write_plane_a, d1 ; Add PlaneA write cmd + address
 move.l  d1, vdp_control        ; Move dest address to VDP control port

 rol.l   #0x08, d3              ; Shift palette ID to bits 14-15
 rol.l   #0x05, d3              ; Can only rol 8 bits at a time

 subq.b  #0x01, d0              ; Num words in d0, minus 1 for counter

 @Copy:
 move.w  (a0)+, d4              ; Move tile ID from map data to lower d4
 and.l   #%0011111111111111, d4 ; Mask out original palette ID
 or.l    d3, d4                 ; Replace with our own
 add.w   d2, d4                 ; Add first tile offset to d4
 move.w  d4, vdp_data           ; Move to VRAM
 dbra    d0, @Copy              ; Loop

 rts
```

La dirección VRAM destino se calcula multiplicando el offset Y por el ancho de línea del plano (`$0040` palabras = 64 palabras); esto asume un plano configurado a 64 celdas de ancho.

### Definición de datos de mapa y secuencia de carga de nivel

```
Level1Map:
; Include data from .bin file
incbin    "assets\maps\level1_map.bin"

Level1MapEnd
Level1MapSizeB: equ (Level1MapEnd-Level1Map) ; Map size in bytes
Level1MapSizeW: equ (Level1MapSizeB/2)       ; Map size in words
Level1MapSizeL: equ (Level1MapSizeB/4)       ; Map size in longs

Level1MapDimentions: equ 0x4010              ; Dimentions (W/H)
```

```
; Load palette
lea Level1Palette, a0 ; Palette source address (ROM) to a0
move.l #0x0, d0       ; Palette slot ID to d0
jsr LoadPalette       ; Jump to subroutine

; Load map tiles
lea      Level1Tiles, a0       ; Tileset source address (ROM) to a0
move.l   #Level1TilesVRAM, d0  ; VRAM dest address to d0
move.l   #Level1TilesSizeT, d1 ; Number of tiles in tileset to d1
jsr      LoadTiles             ; Jump to subroutine

; Load map
lea      Level1Map, a0           ; Map data to a0
move.w   #Level1MapSizeW, d0     ; Size (words) to d0
move.l   #0x18, d1               ; Y offset to d1
move.w   #Level1TilesTileID, d2  ; First tile ID to d2
move.l   #0x0, d3                ; Palette ID to d3
jsr      LoadMapPlaneA           ; Jump to subroutine
```

### Registros VDP relevantes para scroll y tamaño de plano

- **Registro 11 (Scroll Mode)**: bits 0-1 seleccionan el modo de scroll horizontal (`0` = pantalla completa, `1` = por línea de píxel, `2` = por celda); bit 2 selecciona el modo de scroll vertical (`0` = pantalla completa, `1` = por bloques de dos celdas).
- **Registro 13**: define la dirección base en VRAM de los datos de scroll horizontal.
- **Registro 16**: dimensiones del plano, patrón de bits `00VV00HH` donde `00` = 32 celdas, `01` = 64 celdas, `11` = 128 celdas (por eje).

### Scroll horizontal de pantalla completa

En modo "screen" (el más simple), un único valor de 16 bits en la dirección VRAM `$D000` define el desplazamiento horizontal de toda la pantalla en píxeles (positivo = derecha, negativo = izquierda):

```
move.l  #0x50000003, vdp_control
move.w  #0x0010, vdp_data
```

Bucle principal con scroll controlado por el pad, actualizado dentro de la ventana de vblank:

```
 move.l #0x0, d6              ; HScroll value

GameLoop:

; Read gamepad input
 jsr ReadPad1 ; Read pad 1 state, result in d0

 btst   #pad_button_right, d0 ; Check right button
 bne    @NoRight              ; Branch if button off
 subi   #0x1, d6              ; Update H scroll value
 @NoRight:

 btst   #pad_button_left, d0  ; Check left button
 bne    @NoLeft               ; Branch if button off
 addi   #0x1, d6              ; Update H scroll value
 @NoLeft:

; Update scrolling during vblank
 jsr WaitVBlankStart   ; Wait for start of vblank

 move.l  #vdp_write_hscroll, vdp_control ; Update H scrolling
 move.w  d6, vdp_data

 jsr     WaitVBlankEnd ; Wait for end of vblank
 jmp     GameLoop      ; Back to the top
```

### Scroll vertical

El VDP soporta dos modos (bit 2 del registro 11): pantalla completa (el valor vive en la **VSRAM**, un área de memoria separada de la VRAM) y por bloques de dos celdas. El autor especula que la VSRAM es un área aparte porque el soporte de scroll vertical se añadió al chip en una fase posterior de su diseño.

### Pipeline de herramientas para crear el mapa

1. **GIMP** (o cualquier editor de BMP): crear el tileset como imagen indexada con paleta limitada.
2. **BMP2Tile**: convertir el BMP a datos de tiles y paleta en formato ASM (seleccionar todo con `*`, luego `File > Save Palette > In ASM` y `File > Save Tiles > In ASM`), y exportar un `.TLE` para importarlo en el editor de mapas (`File > Export TLE > New`).
3. **MMM (Genesis Map Editor)**: importar el `.TLE`, configurar las dimensiones del mapa (ejemplo del autor: 64×16 tiles para un nivel con scroll horizontal), dibujar el mapa tile a tile, y exportar los datos binarios (`File > Export Binary`).

### Optimización de tilesets

El autor recomienda diseñar pensando en la reutilización desde el principio: bordes reutilizables, tiles centrales repetibles para variar la longitud de una estructura, aprovechar el flipping de tiles que soporta MMM (el autor admite no haberse dado cuenta de que existía esta opción hasta terminar su mapa de prueba), y reservar el tile índice 0 como "vacío" para zonas transparentes.

## Notas / gotchas

- El plano máximo soporta 128 celdas de ancho; niveles más grandes que eso requieren streaming de datos nuevos a VRAM a medida que se hace scroll, no cargarlo todo de una vez.
- Para efectos de scroll más complejos (el autor menciona agua ondulante como ejemplo) hace falta actualizar el scroll por línea individual dentro de la ventana de vblank, no un único valor por frame.
- El propio autor califica su ejemplo de "primitivo" comparado con las técnicas de scroll que usan juegos comerciales reales de Mega Drive.
- Corrección retrospectiva: el autor señala en este mismo artículo que el código de animación de sprites del artículo anterior de la serie tenía un error en la cantidad de bytes copiados por frame.
- Herramienta de ensamblado usada: `asm68k.exe /p scrolltest.asm,scrolltest.bin`.

## Referencias cruzadas

- [`vdp-registers.md`](vdp-registers.md) (fuente plutiedev.com) — referencia completa de los registros del VDP, incluidos los de scroll y tamaño de plano que aquí se usan de forma práctica.
- [`writing-video.md`](writing-video.md) (fuente plutiedev.com) — mecánica genérica de direccionamiento y escritura en VRAM/CRAM/VSRAM que subyace a `LoadMapPlaneA` y a la escritura del valor de hscroll.
- [`tiles-and-palettes.md`](tiles-and-palettes.md) — formato de tiles y paletas relevante para el pipeline BMP2Tile/MMM descrito aquí.

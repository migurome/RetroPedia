---
titulo: Big Evil Corporation — fuentes y renderizado de texto (serie Tanglewood)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bigevilcorporation, tanglewood, fuentes, texto, ascii, plano-a, vdp]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/04/09/sega-megadrive-5-fonts-and-text/]
---

# Big Evil Corporation — fuentes y renderizado de texto (serie Tanglewood)

## Resumen

Quinta entrada de la serie de Matt (Big Evil Corporation, autor de Tanglewood): sustituye el "escribir cada letra a mano" de `bec-hello-world.md` por un sistema reutilizable de fuente tipográfica — una tabla de tiles de carácter, una tabla de mapeo ASCII→tile, y dos subrutinas genéricas (`LoadFont` para subir la fuente a VRAM y `DrawTextPlaneA` para pintar cualquier cadena terminada en cero en el Plano A).

## Detalle técnico

### Organización de la fuente en tiles

La fuente se genera como datos de patrones de 8×8 (aunque el diseño real ocupa solo 7×7 píxeles, dejando una fila en blanco) y se convierte de un BMP a código ensamblador mediante la herramienta **BMP2Tile**. Cada carácter es un tile de 32 bytes, igual que en `bec-hello-world.md`.

```
PixelFont: ; Font start address
dc.l    $01111100
dc.l    $11000110
dc.l    $10111010
dc.l    $10000010
dc.l    $10111010
dc.l    $10101010
dc.l    $11101110
dc.l    $00000000
; Resto de los datos de la fuente...

PixelFontEnd
PixelFontSizeB: equ (PixelFontEnd-PixelFont)
PixelFontSizeW: equ (PixelFontSizeB/2)
PixelFontSizeL: equ (PixelFontSizeB/4)
PixelFontSizeT: equ (PixelFontSizeB/32)
PixelFontVRAM:  equ 0x0100
PixelFontTileID: equ (PixelFontVRAM/32)
```

Los símbolos `Size*` calculan automáticamente el tamaño de la fuente en bytes/words/longs/tiles a partir de las etiquetas de inicio y fin, en vez de escribirlos a mano. `PixelFontVRAM` (`0x0100` en este ejemplo, dirección arbitraria) es donde se cargará la fuente en VRAM, y `PixelFontTileID` se obtiene dividiendo esa dirección entre 32 (el tamaño de un tile) para saber qué ID de patrón le corresponde al primer carácter.

### Rutina LoadFont

Copia la fuente desde ROM a una dirección VRAM arbitraria:

```
LoadFont:
; a0 - Font address (l)
; d0 - VRAM address (w)
; d1 - Num chars (w)

swap     d0
add.l    #vdp_write_tiles, d0
move.l   d0, vdp_control

subq.b   #0x1, d1
@CharCopy:
move.w   #0x07, d2
@LongCopy:
move.l   (a0)+, vdp_data
dbra     d2, @LongCopy
dbra     d1, @CharCopy

rts
```

Invocación:

```
lea        PixelFont, a0
move.l    #PixelFontVRAM, d0
move.l    #PixelFontSizeT, d1
jsr        LoadFont
```

### Tabla de mapeo ASCII → tile de fuente

Convierte códigos ASCII en el rango `0x20`-`0x5A` al índice de tile correspondiente dentro de la fuente. Empieza en `0x20` (espacio):

```
ASCIIStart: equ 0x20

ASCIIMap:
dc.b 0x00   ; SPACE (ASCII 0x20)
dc.b 0x28   ; !
dc.b 0x2B   ; "
dc.b 0x2E   ; #
dc.b 0x00   ; UNUSED
dc.b 0x00   ; UNUSED
dc.b 0x00   ; UNUSED
dc.b 0x2C   ; '
dc.b 0x29   ; (
dc.b 0x2A   ; )
dc.b 0x00   ; UNUSED
dc.b 0x2F   ; +
dc.b 0x26   ; ,
dc.b 0x30   ; -
dc.b 0x25   ; .
dc.b 0x31   ; /
dc.b 0x1B   ; 0
dc.b 0x1C   ; 1
dc.b 0x1D   ; 2
dc.b 0x1E   ; 3
dc.b 0x1F   ; 4
dc.b 0x20   ; 5
dc.b 0x21   ; 6
dc.b 0x22   ; 7
dc.b 0x23   ; 8
dc.b 0x24   ; 9
dc.b 0x2D   ; :
dc.b 0x00   ; UNUSED
dc.b 0x00   ; UNUSED
dc.b 0x00   ; UNUSED
dc.b 0x00   ; UNUSED
dc.b 0x27   ; ?
dc.b 0x00   ; UNUSED
dc.b 0x01   ; A
dc.b 0x02   ; B
dc.b 0x03   ; C
dc.b 0x04   ; D
dc.b 0x05   ; E
dc.b 0x06   ; F
dc.b 0x07   ; G
dc.b 0x08   ; H
dc.b 0x09   ; I
dc.b 0x0A   ; J
dc.b 0x0B   ; K
dc.b 0x0C   ; L
dc.b 0x0D   ; M
dc.b 0x0E   ; N
dc.b 0x0F   ; O
dc.b 0x10   ; P
dc.b 0x11   ; Q
dc.b 0x12   ; R
dc.b 0x13   ; S
dc.b 0x14   ; T
dc.b 0x15   ; U
dc.b 0x16   ; V
dc.b 0x17   ; W
dc.b 0x18   ; X
dc.b 0x19   ; Y
dc.b 0x1A   ; Z (ASCII 0x5A)
```

### Rutina DrawTextPlaneA

Pinta una cadena terminada en cero en el Plano A, en la posición (en tiles) indicada:

```
DrawTextPlaneA:
; a0 (l) - String address
; d0 (w) - First tile ID of font
; d1 (bb)- XY coord (in tiles)
; d2 (b) - Palette

clr.l    d3
move.b   d1, d3
mulu.w   #0x0040, d3
ror.l    #0x8, d1
add.b    d1, d3
mulu.w   #0x2, d3
swap     d3
add.l    #vdp_write_plane_a, d3
move.l   d3, vdp_control

clr.l    d3
move.b   d2, d3
rol.l    #0x8, d3
rol.l    #0x5, d3

lea      ASCIIMap, a1

@CharCopy:
move.b   (a0)+, d2
cmp.b    #0x0, d2
beq.b    @End

sub.b    #ASCIIStart, d2
move.b   (a1,d2.w), d3
add.w    d0, d3
move.w   d3, vdp_data
jmp      @CharCopy

@End:
rts
```

**Cálculo de la dirección VRAM destino:** la coordenada Y se multiplica por `0x0040` (64 tiles, el ancho de una fila en modo H40), se le suma la coordenada X (extraída con `ror.l #0x8, d1` para pasarla del byte alto al bajo), el resultado se convierte a offset en words (×2) y se desplaza a la mitad alta del longword antes de sumarle el comando base de escritura al Plano A.

**Construcción del descriptor de tile por carácter:** el índice de paleta se coloca en los bits 14-15 mediante dos rotaciones (`rol.l #0x8` seguido de `rol.l #0x5`, 13 posiciones en total); al resultado se le suma el ID de tile obtenido de la tabla ASCII más el offset del primer tile de la fuente (`d0`).

**Recorrido de la cadena:** se lee byte a byte con `(a0)+`, se compara con `0x0` para detectar el terminador (`beq.b @End`), y para cada carácter válido se resta `ASCIIStart` para indexar la tabla `ASCIIMap` mediante direccionamiento indexado `(a1,d2.w)`.

### Ejemplo de uso completo

```
; Cargar la fuente
lea       PixelFont, a0
move.l    #PixelFontVRAM, d0
move.l    #PixelFontSizeT, d1
jsr       LoadFont

; Dibujar texto
lea       String1, a0
move.l    #PixelFontTileID, d0
move.w    #0x0501, d1      ; X=5, Y=1
move.l    #0x0, d2         ; Paleta 0
jsr       DrawTextPlaneA

lea       String2, a0
move.l    #PixelFontTileID, d0
move.w    #0x0502, d1      ; X=5, Y=2
move.l    #0x1, d2         ; Paleta 1
jsr       DrawTextPlaneA

String1:
dc.b "ABCDEFGHIJKLM",0
String2:
dc.b "NOPQRSTUVWXYZ",0
String5:
dc.b "THE QUICK BROWN FOX JUMPS",0
String6:
dc.b "OVER THE LAZY DOG",0

include 'fonts\pixelfont.asm'
```

## Notas / gotchas

- Esta entrada reemplaza el enfoque "una instrucción `move.w` por letra" de `bec-hello-world.md` por subrutinas parametrizadas y reutilizables — es el paso natural siguiente una vez se entiende el formato de comando del VDP.
- Toda cadena debe terminar en byte `0x00`; `DrawTextPlaneA` depende de ese terminador para saber cuándo parar (`cmp.b #0x0, d2` / `beq.b @End`).
- El cálculo de dirección VRAM asume modo de pantalla H40 (64 tiles de ancho por fila, de ahí el multiplicador `0x0040`); en H32 habría que ajustar esa constante.
- El flujo de trabajo artístico pasa por herramientas externas (BMP2Tile) para convertir bitmaps a datos de tile — no se generan a mano salvo en el ejemplo mínimo del artículo anterior.

## Referencias cruzadas

- [Big Evil Corporation — primer "Hello World" en pantalla](bec-hello-world.md) (artículo anterior de la serie; aquí se sustituye el volcado manual de tiles por estas subrutinas)
- [Formato del Tile ID (nametable / sprites)](tile-id.md)
- [Tiles y paletas](tiles-and-palettes.md)
- [Big Evil Corporation — sprites (Scary Monsters and Nice Sprites)](bec-sprites.md) (artículo siguiente de la serie, reutiliza el patrón `LoadFont`→`LoadTiles`)

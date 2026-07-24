---
titulo: Big Evil Corporation — sprites, "Scary Monsters and Nice Sprites" (serie Tanglewood)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bigevilcorporation, tanglewood, sprites, sprite-table, vdp, vram]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/04/24/sega-megadrive-6-scary-monsters-and-nice-sprites/]
---

# Big Evil Corporation — sprites (serie Tanglewood)

## Resumen

Sexta entrada de la serie de Matt (Big Evil Corporation, autor de Tanglewood): construye desde cero un sistema de sprites — formato de la Sprite Attribute Table, organización de VRAM para varios assets (fuente + sprites) mediante un archivo de mapeo dedicado, la rutina de carga de la tabla al VDP, y subrutinas para mover un sprite en pantalla sin recargar toda la tabla.

## Detalle técnico

### Formato de la Sprite Attribute Table

La tabla vive en la dirección VRAM `0xE000` en el ejemplo del autor (configurable mediante el registro 5 del VDP durante la inicialización — ver `bec-vdp-init.md`). Cada entrada mide 8 bytes, con este layout de bits:

```
000000YY YYYYYYYY 0000HHVV 0NNNNNNN DPPFFTTT TTTTTTTT 000000XX XXXXXXXX
```

- **Y** (bits 0-9): coordenada vertical, rango -128 a altura_pantalla+128.
- **H/V** (bits 2-3 del tercer byte): dimensiones en tiles de la rejilla del sprite (de 1×1 a 4×4).
- **N** (bits 0-6 del cuarto byte): índice del siguiente sprite en la lista enlazada; 0 marca el final de la lista.
- **D** (bit 7 del cuarto byte): prioridad de dibujado.
- **P** (bits 5-6 del quinto byte): índice de paleta.
- **F** (bits 3-4 del quinto byte): flags de flip vertical/horizontal.
- **T** (bits 0-7 repartidos en el quinto-sexto byte): índice del primer tile del patrón.
- **X** (bits 0-9): coordenada horizontal, mismo rango con offset que Y.

Igual que en la tabla de sprites documentada por plutiedev.com, el sistema usa un borde/offset de 128 píxeles: para colocar un sprite en la esquina superior izquierda visible de la pantalla hay que escribir la coordenada `128,128` (no `0,0`).

### Organización de VRAM entre varios assets (assetmap.asm)

Para evitar que la fuente y los distintos sprites se solapen en VRAM, el autor centraliza el mapeo de direcciones en un único archivo que encadena cada asset a partir del tamaño del anterior:

```asm
; Art asset VRAM mapping
PixelFontVRAM:  equ 0x0000
Sprite1VRAM:    equ PixelFontVRAM+PixelFontSizeB
Sprite2VRAM:    equ Sprite1VRAM+Sprite1SizeB
Sprite3VRAM:    equ Sprite2VRAM+Sprite2SizeB

; Include all art assets
    include 'assets\fonts\pixelfont.asm'
    include 'assets\sprites\sprite1.asm'
    include 'assets\sprites\sprite2.asm'
    include 'assets\sprites\sprite3.asm'

; Include all palettes
    include 'assets\palettes\paletteset1.asm'
```

Cada archivo de asset define sus propios símbolos `SizeB`/`SizeW`/`SizeL`/`SizeT` y `TileID` siguiendo el mismo patrón visto en `bec-fonts-text.md` (`PixelFontSizeB`, etc.). Ejemplo de datos de un sprite de 2 tiles:

```asm
Sprite1:
    dc.l    $11111111    ; Tile: 1
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $11111111

    dc.l    $11111111    ; Tile: 2
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $10000001
    dc.l    $11111111

Sprite1End
Sprite1SizeB: equ (Sprite1End-Sprite1)
Sprite1SizeW: equ (Sprite1SizeB/2)
Sprite1SizeL: equ (Sprite1SizeB/4)
Sprite1SizeT: equ (Sprite1SizeB/32)
Sprite1TileID: equ (Sprite1VRAM/32)
```

### Rutina de carga de la tabla de sprites

```asm
LoadSpriteTables:
   ; a0 - Dirección datos sprite
   ; d0 - Número sprites
   move.l    #vdp_write_sprite_table, vdp_control
   subq.b    #0x1, d0
   @AttrCopy:
   move.l    (a0)+, vdp_data
   move.l    (a0)+, vdp_data
   dbra    d0, @AttrCopy
   rts
```

Invocación:

```asm
lea     SpriteDesc1, a0     ; Dirección tabla sprites
move.w  #0x1, d0            ; 1 sprite
jsr     LoadSpriteTables
```

Descriptor de un único sprite (8 bytes, mismo orden que el layout de bits de arriba):

```asm
SpriteDesc1:
dc.w 0x0080        ; Y coord (+ 128)
dc.b %00001111     ; Ancho (bits 0-1), alto (bits 2-3)
dc.b 0x00          ; Índice siguiente sprite
dc.b 0x00          ; Flip (bits 3-4), paleta (bits 5-6), prioridad (bit 7)
dc.b Sprite1TileID ; Índice del primer tile
dc.w 0x0080        ; X coord (+ 128)
```

### Mover un sprite sin recargar la tabla completa

```asm
SetSpritePosX:
   ; d0 (b) - ID sprite
   ; d1 (w) - Coordenada X
   clr.l    d3
   move.b    d0, d3
   mulu.w    #0x8, d3        ; Offset en el array (8 bytes/entrada)
   add.b    #0x6, d3         ; Offset del campo X (bytes 6-7)
   swap    d3
   add.l    #vdp_write_sprite_table, d3
   move.l    d3, vdp_control
   move.w    d1, vdp_data
   rts
```

```asm
SetSpritePosY:
   ; d0 (b) - ID sprite
   ; d1 (w) - Coordenada Y
   clr.l    d3
   move.b    d0, d3
   mulu.w    #0x8, d3        ; Offset en el array
   swap    d3
   add.l    #vdp_write_sprite_table, d3
   move.l    d3, vdp_control
   move.w    d1, vdp_data
   rts
```

Uso conjunto:

```asm
move.w  #0x0,  d0      ; ID sprite
move.w  #0xB0, d1      ; Coordenada X
jsr     SetSpritePosX
move.w  #0xB0, d1      ; Coordenada Y
jsr     SetSpritePosY
```

### Varios sprites encadenados (lista enlazada)

```asm
SpriteDescs:
dc.w 0x0000        ; Y
dc.b %00001111     ; H/V (4x4 tiles)
dc.b 0x01          ; Next = sprite 1
dc.b 0x00          ; Flip/paleta/prioridad
dc.b Sprite1TileID
dc.w 0x0000        ; X

dc.w 0x0000        ; Y
dc.b %00001111
dc.b 0x00          ; Next = 0 (fin de lista)
dc.b 0x20          ; Paleta bit 5
dc.b Sprite2TileID
dc.w 0x0000        ; X
```

### Comando de ensamblado usado en el artículo

```
asm68k.exe /p spritetest.asm,spritetest.bin
```

### Flujo de trabajo completo descrito por el autor

1. Preparar el artwork como PNG indexado a 16 colores con GIMP y exportarlo a datos de tile con Bmp2Tile.
2. Definir el mapa de direcciones VRAM en `assetmap.asm`, concatenando cada asset a partir del tamaño del anterior para no solaparlos.
3. Cargar los patrones a VRAM con una rutina de carga de tiles genérica (el autor reutiliza y renombra `LoadFont` de `bec-fonts-text.md` como `LoadTiles`).
4. Definir los descriptores de sprite: estructuras de 8 bytes con Y, dimensiones, puntero al siguiente, flags/paleta/prioridad, tile inicial y X.
5. Transferir la tabla completa a VRAM con `LoadSpriteTables` (4 longwords, 8 bytes, por sprite).
6. El VDP dibuja los sprites siguiendo la lista enlazada de adelante hacia atrás: el primer sprite de la lista aparece por encima del resto.
7. Para animación/movimiento en tiempo real, modificar solo X/Y de un sprite concreto con `SetSpritePosX`/`SetSpritePosY` en vez de reescribir la tabla entera.

## Notas / gotchas

- El sistema de offset de coordenadas (+128 en X e Y) coincide exactamente con el documentado por plutiedev.com — es una particularidad de hardware del VDP, no una convención propia del autor.
- Separar la actualización de posición (`SetSpritePosX`/`SetSpritePosY`) de la carga completa de la tabla (`LoadSpriteTables`) es la clave de rendimiento: mover un sprite cada frame no debería implicar reescribir los 8 bytes de todos los sprites, solo los 2 bytes del campo que cambia.
- El archivo `assetmap.asm` como fuente única de verdad para direcciones VRAM evita colisiones manuales cuando el número de assets crece — es un patrón organizativo, no una limitación de hardware.
- El pattern ID de cada sprite depende de dónde haya quedado cargado en VRAM (`Sprite1TileID = Sprite1VRAM/32`), así que el orden de los `include` en `assetmap.asm` determina directamente los IDs — reordenar los includes sin recompilar todo puede desincronizar los descriptores con los datos reales.

## Referencias cruzadas

- [Sprites en el VDP](sprites.md) — fuente plutiedev.com; mismo formato de entrada de 8 bytes y mismo offset de coordenadas (+128), con más detalle sobre límites de hardware (sprites por línea, caché de sprites) que este artículo no cubre.
- [Referencia de la tabla de sprites](sprite-table.md) — fuente plutiedev.com; referencia bit a bit equivalente al layout documentado aquí.
- [Big Evil Corporation — fuentes y renderizado de texto](bec-fonts-text.md) (artículo anterior de la serie; `LoadFont` se reutiliza aquí como `LoadTiles`)

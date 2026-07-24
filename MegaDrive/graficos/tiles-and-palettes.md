---
titulo: Tiles y paletas
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, tiles, paletas, cram, vram, color]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/tiles-and-palettes]
---

# Tiles y paletas

## Resumen

Describe el formato de los tiles (imágenes de 8×8 píxeles con hasta 15 colores más transparencia) y el formato de las paletas de color (CRAM) de la Mega Drive, junto con el código para cargar ambos a VRAM/CRAM y para configurar el color de fondo.

## Detalle técnico

### Definición de tiles

Un tile es "una imagen de 8×8 que puede tener hasta 15 colores (y píxeles transparentes)". En memoria ocupan 32 bytes, organizados como 8 longwords (4 bytes cada uno) donde cada fila representa un longword. Cada píxel utiliza 4 bits (un dígito hexadecimal).

**Estructura en memoria del tile "LetterA":**

```
dc.l  $00111000  ; fila 0
dc.l  $01101100  ; fila 1
dc.l  $22000220  ; fila 2
dc.l  $22000220  ; fila 3
dc.l  $11111110  ; fila 4
dc.l  $11000110  ; fila 5
dc.l  $11000110  ; fila 6
dc.l  $00000000  ; fila 7
```

### Definición de paletas

"La Mega Drive puede mostrar hasta cuatro paletas en pantalla, y cada paleta consiste en 16 colores". El primer color es siempre transparente (excepto cuando se usa como color de fondo).

**Formato en CRAM:** cada paleta ocupa 32 bytes. Cada color son 2 bytes en formato BGR (azul-verde-rojo), usando solo dígitos pares (02468ACE). Ejemplo: `$E00` es azul, `$048` es marrón.

**Estructura de paleta de ejemplo:**

```
dc.w  $000                  ; Transparente
dc.w  $6AE,$26A,$026        ; Tonos piel
dc.w  $E00,$A00,$600        ; Tonos azul
dc.w  $00E,$00A,$006        ; Tonos rojo
dc.w  $0EE,$0AA             ; Tonos amarillo
dc.w  $EEE,$AAA,$666,$000   ; Grises
```

### Direcciones VDP

```
VdpCtrl:  equ $C00004  ; Puerto control
VdpData:  equ $C00000  ; Puerto datos
```

### Cálculo de direcciones

- **Para tiles:** número de tile << 5
- **Para paletas:** número de paleta << 5

### Código: cargar tiles a VRAM

```
LoadTiles:
    lsl.w   #5, d0
    SetVramAddrReg d0
    subq.w  #1, d1
    lea     (VdpData), a1
@Loop:
    move.l  (a0)+, (a1)  ; 8 longwords = 1 tile
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    dbf     d1, @Loop
    rts
```

### Código: cargar paletas a CRAM

```
LoadPalette:
    lsl.w #5, d0
    SetCramAddrReg d0
    lea     (VdpData), a1
    move.l  (a0)+, (a1)  ; 8 longwords = 1 paleta
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    move.l  (a0)+, (a1)
    rts
```

### Color de fondo

**Registro:** `VDPREG_BGCOL: equ $8700`

**Fórmula:** (número paleta × 16) + número color

**Código de asignación:**

```
and.w   #$FF, d0
or.w    #VDPREG_BGCOL, d0
move.w  d0, (VdpCtrl)
```

**Nota especial del autor:** "Este es el único caso donde puede usarse un color transparente".

## Notas / gotchas

- Límites de hardware: VRAM de 64 KB (aproximadamente 1536 tiles útiles tras descontar el espacio reservado para tablas).
- CRAM aloja 4 paletas de 16 colores cada una.
- Cada tile efectivo tiene 15 colores opacos + 1 transparente (el color índice 0 de cada paleta).
- El color de fondo es la única situación donde el color transparente (índice 0) tiene un uso visible, ya que se muestra literalmente como ese color en vez de dejar ver algo detrás.

## Referencias cruzadas

- (pendiente de enlazar)

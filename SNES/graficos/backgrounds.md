---
titulo: Fondos y modos de BG
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, backgrounds, bgmode, prioridad, mosaico, modo7]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Backgrounds]
---

# Fondos y modos de BG

## Resumen

El PPU de la SNES compone la imagen a partir de hasta cuatro capas de fondo (BG1-BG4) más la capa de sprites, cuya combinación de profundidad de color, número de capas activas y orden de prioridad depende del "modo de BG" (0-7) seleccionado en el registro BGMODE ($2105). Esta entrada resume las ocho combinaciones posibles y los conceptos generales (prioridad, mosaico, alta resolución) que se aplican a los fondos independientemente del modo.

## Detalle técnico

### Modos de BG (0-7)

Cada modo fija la profundidad de color (bits por píxel, bpp) de cada capa y, con ello, cuántos colores por paleta y cuántas paletas caben en CGRAM. Tabla resumen:

| Modo | BG1 | BG2 | BG3 | BG4 | Alta res. | Prioridad (frente→fondo) |
|---|---|---|---|---|---|---|
| 0 | 2bpp | 2bpp | 2bpp | 2bpp | — | S3 1H 2H S2 1L 2L S1 3H 4H S0 3L 4L |
| 1 | 4bpp | 4bpp | 2bpp | — | — | S3 1H 2H S2 1L 2L S1 3H S0 3L |
| 2 | 4bpp | 4bpp | OPT | — | — | S3 1H S2 2H S1 1L S0 2L |
| 3 | 8bpp | 4bpp | — | — | — | S3 1H S2 2H S1 1L S0 2L |
| 4 | 8bpp | 2bpp | OPT | — | — | S3 1H S2 2H S1 1L S0 2L |
| 5 | 4bpp | 2bpp | — | — | Sí | S3 1H S2 2H S1 1L S0 2L |
| 6 | 4bpp | OPT | — | — | Sí | S3 1H S2 S1 1L S0 |
| 7 | 8bpp | (EXTBG) | — | — | — | S3 S2 2H S1 1L S0 2L |

`OPT` = offset-per-tile (BG3 se reutiliza como tabla de desplazamiento, ver `offset-per-tile.md`). `S0-S3` son las cuatro subcapas de prioridad de sprites; `nH`/`nL` la subcapa alta/baja de cada BGn (ver más abajo).

Notas por modo:

- **Modo 0:** único modo con cuatro capas 2bpp simultáneas; cada BG usa un bloque distinto de 32 colores de CGRAM (BG1: 0-31, BG2: 32-63, BG3: 64-95, BG4: 96-127).
- **Modo 1:** el más usado en juegos comerciales — dos capas principales de 16 colores (BG1, BG2) más una auxiliar de 4 colores (BG3).
- **Modo 7:** ocupa siempre la primera mitad de VRAM con un único tilemap de 128×128 tiles de 8×8 (256 tiles disponibles), y aplica una transformación afín (rotación/escala) en vez del sistema estándar de scroll de BG1-4; el bit EXTBG permite además una segunda capa (BG2) partida en dos niveles de prioridad.

### Registros implicados

- **BGMODE ($2105):** selecciona el modo y el tamaño de carácter (8×8 o 16×16) de cada BG.
- **BGnSC ($2107-$210A):** dirección VRAM y repetición horizontal/vertical del tilemap de cada BG.
- **BG12NBA / BG34NBA ($210B-$210C):** dirección base de los tiles (CHR) de cada BG.
- **BGnHOFS / BGnVOFS ($210D-$2114):** scroll horizontal y vertical de cada BG.
- **SETINI ($2133):** activa alta resolución e interlace.
- **M7SEL ($211A):** ajustes específicos de Modo 7 (flip, comportamiento fuera de límites).

El detalle bit a bit de todos estos registros está en `ppu-registers.md`.

### Prioridad de capas

Cada BG se subdivide en una subcapa de prioridad alta y otra baja según el bit de prioridad de cada entrada de tilemap (ver `tilemaps.md`). Los sprites, por su parte, tienen cuatro subcapas de prioridad independientes (S0 más baja, S3 más alta) fijadas por el atributo de prioridad de cada sprite en OAM. El orden final de composición (de frente hacia el fondo) es el que aparece en la columna "Prioridad" de la tabla de modos.

### Alta resolución e interlace

Los modos 5 y 6 fuerzan una resolución horizontal alta (512 píxeles) mediante el bit `H` de SETINI; en ese caso el desplazamiento horizontal se produce en incrementos "finos" de 2 píxeles de alta resolución. El entrelazado (bit `I` de SETINI) produce una imagen de 480i alternando dos campos por frame, y en los modos 5/6 los BG se auto-entrelazan verticalmente para duplicar la resolución percibida.

### Mosaico

El registro MOSAIC ($2106) permite aplicar un efecto de pixelado por bloques a cualquier combinación de BG1-BG4, con un tamaño de bloque configurable de 1×1 (sin efecto) hasta 16×16 píxeles.

## Notas / gotchas

- El modo 7 no tiene registros de scroll propios: reutiliza $210D/$210E (BG1HOFS/BG1VOFS) como M7HOFS/M7VOFS.
- Los modos con offset-per-tile (2, 4, 6) sacrifican el tilemap de BG3 (o de BG2 en el modo 6) para usarlo como tabla de desplazamiento; no pueden dibujar una capa normal ahí.
- En modos 5 y 6, el ancho de tile queda forzado según el modo, condicionando cómo se organiza el tileset en VRAM.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Tilemaps](tilemaps.md)
- [Tiles y formatos de bitplane](tiles.md)
- [Offset-per-tile](offset-per-tile.md)
- [Paletas y CGRAM](palettes.md)

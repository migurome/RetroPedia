---
titulo: Formato del Tile ID (nametable / sprites)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, tile-id, nametable, sprites, flip, paleta, prioridad]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/tile-id]
---

# Formato del Tile ID (nametable / sprites)

## Resumen

Describe el formato de bits de la palabra de 16 bits usada para especificar un patrón gráfico (tile ID) tanto en los tilemaps (planos A, B o ventana) como en los sprites, incluyendo el índice de tile, flip horizontal/vertical, selección de paleta y prioridad de capa.

## Detalle técnico

### Estructura del Tile ID

El formato de especificación de patrones en tilemaps (planos A, B o ventana) y sprites utiliza la siguiente distribución de bits:

**Bits 10-0:** "tile number (from 0 to 2047)" — identificador del patrón gráfico.

### Flags de efectos especiales

Los bits restantes aplican transformaciones:

- **Bit 11:** "flip the picture horizontally"
- **Bit 12:** "flip the picture vertically"
- **Bits 14-13:** "pick one of the four palettes (0 to 3)"
- **Bit 15:** "pick layer priority (0 = low, 1 = high)"

### Diferencia en aplicación

"For the tilemaps the effects are applied per tile, while for sprites the effects are applied to the whole sprite" — esto significa que los flips en sprites afectan la imagen completa, no individualmente cada tile dentro del sprite.

### Labels en ensamblador

Constantes predefinidas para operaciones OR con el tile ID:

**Flips:** NOFLIP ($0000), HFLIP ($0800), VFLIP ($1000), HVFLIP ($1800)

**Paletas:** PAL0 ($0000), PAL1 ($2000), PAL2 ($4000), PAL3 ($6000)

**Prioridad:** LOPRI ($0000), HIPRI ($8000)

## Notas / gotchas

- En tilemaps, cada flag (flip, paleta, prioridad) se aplica de forma independiente por cada tile individual del mapa.
- En sprites, los flags se aplican al sprite completo (todos los tiles que lo componen comparten el mismo flip, paleta y prioridad), no tile a tile.
- El índice de tile (bits 10-0) tiene un rango de 0 a 2047.

## Referencias cruzadas

- (pendiente de enlazar)

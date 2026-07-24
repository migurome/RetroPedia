---
titulo: SGDK — Tutorial de Backgrounds (tiles, mapas y planos)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, tutorial, background, tilemap, vdp, recursos, map, tileset]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/Tuto-Background]
---

# SGDK — Tutorial de Backgrounds (tiles, mapas y planos)

## Resumen

Cómo manejar fondos/tilemaps/planos en SGDK, tanto mediante la API de alto nivel (recursos `MAP`/`TILESET`) como con la API de bajo nivel del VDP. Cubre la sintaxis de declaración de recursos y las funciones principales para cargar, escribir y desplazar tiles.

## Detalle técnico

### API de alto nivel: recurso MAP (recomendado)

El enfoque recomendado usa el tipo de recurso `MAP`, que codifica fondos grandes de forma eficiente minimizando el uso de ROM.

**Declaración en archivo `.res`:**
```
TILESET bga_tileset "gfx/S1_GHZ1_FG.png" BEST ALL
MAP bga_map "gfx/S1_GHZ1_FG.png" bga_tileset BEST
```

El recurso `MAP` requiere un recurso `TILESET` previo, lo que permite reutilizarlo entre varios mapas.

**Funciones principales:**

Crear el mapa:
```c
Map* bga = MAP_create(&bga_map, BG_A,
  TILE_ATTR_FULL(0, FALSE, FALSE, FALSE, TILE_USER_INDEX));
```

Scroll:
```c
MAP_scrollTo(bga, x, y);
```

Liberar memoria:
```c
MEM_free(map);
```

### API de bajo nivel de TileMap

La documentación señala que este enfoque está desactualizado pero explica los conceptos fundamentales. El uso moderno recomienda recursos `IMAGE` con estas funciones:

- `VDP_loadTileSet(..)`
- `VDP_setTileMapxx(..)`

### Conceptos fundamentales de los tiles

- Un tile ocupa 32 bytes: 4 bytes por línea.
- Cada píxel usa un valor de 4 bits (índices de color `0x0`-`0xF`).
- El tile 0 sirve como relleno de fondo.

### Atributos de plano

Los tiles soportan atributos codificados en un único valor:
- índice de paleta (0-3)
- flag de prioridad
- flip vertical
- flip horizontal
- referencia al índice del tile

### Funciones esenciales de la API

Cargar datos de tile:
```c
VDP_loadTileData((const u32 *)tile, 1, 1, 0);
```

Establecer atributos de tile:
```c
VDP_setTileMapXY(BG_A, TILE_ATTR_FULL(PAL2, 0, 1, 0, 1), 6, 5);
```

Rellenar regiones:
```c
VDP_fillTileMapRect(BG_B, TILE_ATTR_FULL(PAL3, 0, 0, 0, TILE1),
  12, 12, 8, 8);
```

Dibujar imágenes:
```c
VDP_drawImageEx(BG_A, &moon, TILE_ATTR_FULL(PAL1, 0, 0, 0, 1),
  12, 12, 0, CPU);
```

Gestión de paletas:
```c
VDP_setPalette(PAL1, moon.palette->data);
```

## Notas / gotchas

- SGDK reserva espacio en VRAM para 1310 tiles (más 96 para la fuente de texto).
- Los bitmaps deben tener dimensiones alineadas a 8 píxeles.
- Restricción: 16 colores por tile, máximo 64 colores en total.
- Hay opciones de compresión (RLE, Huffman) disponibles a nivel de declaración de recursos.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Hello World](tuto-hello-world.md)
- [Tutorial: Sprites](tuto-sprites.md)
- Comparar con el sistema de tiles/paletas "crudo" del VDP en [`../../graficos/tiles-and-palettes.md`](../../graficos/tiles-and-palettes.md) (fuente plutiedev.com)

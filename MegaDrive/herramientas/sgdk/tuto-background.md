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
- **Comprimir los fondos sale muy a cuenta:** `IMAGE nombre "f.png" FAST ALL` (LZ4W) en lugar de `NONE ALL` solo cuesta un poco más de tiempo al cargar la pantalla. En Super Smash Land MD, 25 fondos de 160x144 y 320x224 con colores planos pasaron de 108 KB a 39 KB, y con eso cupieron tres canciones PCM más.
- **`VDP_drawImageEx(..., dma = TRUE)` usa un búfer temporal de la cola de DMA** para el mapa con el tile base sumado, y también para los tiles si la imagen va comprimida. La copia ocurre en la VBlank. Si en ese mismo fotograma se crean muchos sprites nuevos con frames comprimidos, también piden búfer temporal, y el mapa o los tiles del fondo pueden salir pisados: casillas que muestran otros tiles del mismo fondo.
  - Al cambiar de pantalla conviene `dma = FALSE` (copia por CPU, inmediata). El tiempo extra en un cambio de pantalla no se nota.
  - Para diagnosticarlo, volcar la VRAM: el mapa del plano B empieza por defecto en 0xC000 y el del A en 0xE000 (64x32, 2 bytes por casilla, big endian). La tabla de sprites va en 0xF400.
- **Tablas de recursos escritas a mano:** si el código tiene un array con los `Image*` de cada pantalla, añadir `_Static_assert(sizeof(T)/sizeof(T[0]) == N)`. Una pantalla nueva que falta en la tabla deja un puntero nulo, y SGDK pinta basura sin dar ningún error.
- **Marcador sobre un escenario que llena la pantalla:** usar el plano ventana en las últimas filas: `VDP_setWindowVPos(TRUE, fila)` y `VDP_drawTextBG(WINDOW, …)`.
  - En esas filas la ventana sustituye al plano A, así que el primer plano del escenario no ensucia el texto.
  - Si además se vacía el plano B en esas filas, las letras salen sobre el color de fondo, como una franja.
  - En los menús se quita con `VDP_setWindowVPos(FALSE, 0)`.
  - Usado en el prototipo a 1,5x y 2x de Super Smash Land MD (B053).
- **Capturas de pruebas en un cambio de pantalla:** el fotograma en que se pulsa el botón que cambia de pantalla ya está cargando la siguiente, y la captura sale mezclada (tiles nuevos con el mapa viejo). No es un fallo del juego, salvo que se vea también fuera de ese fotograma.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Hello World](tuto-hello-world.md)
- [Tutorial: Sprites](tuto-sprites.md)
- Comparar con el sistema de tiles/paletas "crudo" del VDP en [`../../graficos/tiles-and-palettes.md`](../../graficos/tiles-and-palettes.md) (fuente plutiedev.com)

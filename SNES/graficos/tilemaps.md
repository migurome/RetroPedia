---
titulo: Tilemaps
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, tilemap, nametable, vram, backgrounds]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Tilemaps]
---

# Tilemaps

## Resumen

Un tilemap (también llamado nametable) es el bloque de VRAM que le indica al PPU qué tile dibujar en cada casilla de una capa de fondo, junto con su paleta, prioridad y orientación. Cada tilemap ocupa 2 KiB (1 Kiword) de VRAM y describe una rejilla de 32×32 tiles. Esta entrada documenta el formato de cada entrada de 16 bits y cómo se organiza la memoria de un tilemap.

## Detalle técnico

### Formato de cada entrada (16 bits)

Cada casilla del tilemap ocupa una palabra de VRAM, con el byte bajo en `VMDATAL` ($2118) y el byte alto en `VMDATAH` ($2119):

```
15  bit  8   7  bit  0
---- ----   ---- ----
VHPC CCTT   TTTT TTTT
```

- **`T9-0`:** número de tile (0-1023) dentro del conjunto de caracteres seleccionado para ese BG.
- **`C2-0`:** selección de paleta (0-7) dentro del bloque de CGRAM correspondiente a ese BG.
- **`P`:** bit de prioridad — determina si esa casilla pertenece a la subcapa alta o baja del BG (ver `backgrounds.md` para el orden de composición resultante).
- **`H`:** volteo horizontal del tile.
- **`V`:** volteo vertical del tile.

### Organización en memoria

Un tilemap de 32×32 casillas se almacena por filas: los 32 tiles de la primera fila (izquierda a derecha), luego los 32 de la segunda, y así sucesivamente. El registro BGnSC correspondiente ($2107-$210A) fija tanto la dirección base como si ese BG usa uno o dos tilemaps en horizontal y/o en vertical (ver `ppu-registers.md`), permitiendo tableros efectivos de 32×32, 64×32, 32×64 o 64×64 tiles repartidos en varios bloques de 2 KiB contiguos.

Cuando el BG usa tiles de 16×16 (tamaño de carácter grande, fijado en BGMODE), cada entrada del tilemap sigue señalando un único índice de tile de 8×8 como esquina superior izquierda del carácter grande; el PPU añade automáticamente los tiles adyacentes en los desplazamientos +1 (horizontal) y +16/+17 (para completar el cuadrado de 2×2 tiles de 8×8).

### Caso especial: Modo 7

El Modo 7 no usa el formato de entrada de 16 bits anterior: su tilemap ocupa 128×128 entradas, pero cada entrada es un único byte que contiene directamente el índice de tile (0-255) en el byte bajo de VRAM, sin bits de paleta, prioridad ni flip (el Modo 7 no admite estos atributos por entrada).

### Relación con offset-per-tile

En los modos de BG con offset-per-tile (2, 4 y 6), el tilemap de BG3 (BG2 en el modo 6) deja de usarse como tilemap normal: sus entradas se reinterpretan como tabla de desplazamientos de scroll por columna, no como referencias a tiles. Ver `offset-per-tile.md` para el formato de esas entradas.

## Notas / gotchas

- El bit de prioridad de cada entrada es lo que separa la subcapa "alta" de la "baja" de un mismo BG en la tabla de prioridades de `backgrounds.md`.
- Los tiles grandes (16×16) consumen 4 índices de tile contiguos en el conjunto de caracteres (offsets 0, +1, +16, +17) aunque el tilemap solo almacene un índice por casilla.
- El formato de Modo 7 (un byte por casilla, sin atributos) es incompatible con el resto de modos; no se puede reutilizar directamente un tilemap estándar como mapa de Modo 7.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Fondos y modos de BG](backgrounds.md)
- [Tiles y formatos de bitplane](tiles.md)
- [Offset-per-tile](offset-per-tile.md)

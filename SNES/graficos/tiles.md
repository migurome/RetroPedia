---
titulo: Tiles y formatos de bitplane
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, tiles, bitplane, vram, 2bpp, 4bpp, 8bpp]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Tiles]
---

# Tiles y formatos de bitplane

## Resumen

Los tiles (o "caracteres") son las unidades gráficas básicas de 8×8 píxeles con las que el PPU construye tanto los fondos como los sprites. Se almacenan en VRAM como planos de bits ("bitplanes") intercalados, y la SNES soporta tres profundidades de color — 2bpp, 4bpp y 8bpp — que determinan cuántos bitplanes se combinan por tile y cuántos colores puede tener cada píxel.

## Detalle técnico

### Formato base: 2bpp

Cada tile 2bpp ocupa 8 words (16 bytes) de VRAM: una word por cada una de las 8 filas de píxeles del tile. Dentro de cada word, el byte bajo contiene el plano de bit 0 de esa fila y el byte alto contiene el plano de bit 1; el valor final de cada píxel (0-3) se forma combinando el bit correspondiente de ambos planos. Con 2bpp cada píxel puede tomar uno de 4 valores de color dentro de la paleta asignada a ese tile.

### Formato 4bpp

Es el formato más usado: todos los sprites y los BG de los modos 1, 2, 3, 5 y 6 lo emplean. Equivale a dos tiles 2bpp superpuestos: los primeros 16 bytes contienen los planos de bit 0 y 1 (igual que en 2bpp), y los 16 bytes siguientes contienen los planos de bit 2 y 3 de las mismas 8 filas. El tile ocupa por tanto 32 bytes en total, y cada píxel puede tomar uno de 16 valores de color.

### Formato 8bpp

Usado en los modos de BG 3 y 4 (y en Modo 7 con color directo). Son cuatro bloques de plano 2bpp concatenados (planos 0-1, 2-3, 4-5 y 6-7), ocupando 64 bytes por tile en total. Cada píxel puede tomar uno de 256 valores de color.

### Resumen de tamaños

| Formato | Bitplanes | Colores por píxel | Bytes por tile |
|---|---|---|---|
| 2bpp | 2 | 4 | 16 |
| 4bpp | 4 | 16 | 32 |
| 8bpp | 8 | 256 | 64 |

### Organización en VRAM

Dentro de un conjunto de caracteres (tileset), los tiles se numeran en filas de 16 tiles de ancho: el tile N y el tile N+1 son vecinos horizontales, mientras que N y N+16 son vecinos verticales dentro de esa rejilla de 16 columnas. Esta disposición es la misma que usan los sprites grandes para encontrar sus tiles adyacentes (ver `sprites.md`).

### Color directo (bypass de paleta)

En el modo de color directo de 8bpp, el valor del píxel no se usa como índice de CGRAM sino que se traduce directamente a un color RGB expandido: los 8 bits del píxel se interpretan como `BBGG GRRR` y cada componente se expande a 5 bits, aunque solo 8 de los 15 bits de color resultantes quedan bajo control real (el resto se rellena con ceros o bits fijos). El Modo 7 con color directo usa el mismo mapeo de bits, pero al carecer sus entradas de tilemap de atributos adicionales, los bits "extra" que en 8bpp normal aportan algo de variación quedan simplemente a 0.

### Píxel transparente

En cualquier formato, un valor de píxel igual a 0 es siempre transparente (se ve el fondo o la capa inferior). Esto implica que en los modos de color directo no es posible representar un negro puro mediante el valor de píxel 0, ya que ese valor nunca se dibuja.

## Notas / gotchas

- El valor de píxel 0 es transparente en todos los formatos, incluido el color directo — no hay forma de pintar negro puro con ese índice.
- El color directo solo controla 8 de los 15 bits de color disponibles en CGRAM; no sustituye por completo la precisión de una paleta indexada normal.
- Los formatos 4bpp y 8bpp comparten la misma organización interna que 2bpp (bloques de 16 bytes apilados), lo que facilita convertir arte de menor profundidad a mayor profundidad rellenando planos con cero.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Fondos y modos de BG](backgrounds.md)
- [Tilemaps](tilemaps.md)
- [Sprites (OAM)](sprites.md)
- [Paletas y CGRAM](palettes.md)

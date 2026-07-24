---
titulo: Paletas y CGRAM
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, cgram, paletas, color, bgr555]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Palettes]
---

# Paletas y CGRAM

## Resumen

CGRAM es la memoria de paleta del PPU: almacena 256 colores de 15 bits que los tiles de fondo y los sprites referencian por índice. Esta entrada documenta el formato de color, los registros de acceso a CGRAM y cómo se reparten las 256 entradas entre fondos y sprites según la profundidad de color de cada tile.

## Detalle técnico

### Estructura de CGRAM

CGRAM contiene 256 words de 15 bits cada una (una entrada de color por índice 0-255).

### Formato de color (BGR555)

```
15  bit  8   7  bit  0
---- ----   ---- ----
.BBB BBGG   GGGR RRRR
```

- **`R4-0`:** componente rojo (0-31)
- **`G4-0`:** componente verde (0-31)
- **`B4-0`:** componente azul (0-31)

El bit 15 no se usa.

### Registros de acceso

- **CGADD ($2121):** escribe un byte para seleccionar cuál de las 256 entradas de CGRAM se va a leer/escribir.
- **CGDATA ($2122):** dos escrituras de byte (primero el byte bajo, luego el alto) fijan el color de 15 bits de la entrada seleccionada. Tras completar las dos escrituras, CGADD se incrementa automáticamente a la siguiente entrada.
- **CGDATAREAD ($213B):** lectura equivalente a CGDATA, en dos accesos de 8 bits. El bit alto no implementado devuelve un valor no fiable que debe ignorarse.

El detalle bit a bit completo de estos tres registros está en `ppu-registers.md`.

### Reparto de paletas según profundidad de tile

- **Tiles 2bpp:** usan grupos de 4 colores de CGRAM; la primera entrada de cada grupo (índice de píxel 0) es siempre transparente.
- **Tiles 4bpp:** usan grupos de 16 colores; igualmente la entrada 0 de cada grupo es transparente. Los fondos usan los primeros 8 grupos de 16 colores (entradas 0-127) y los sprites usan los 8 grupos restantes (entradas 128-255).
- **Tiles 8bpp:** pueden usar toda CGRAM salvo la entrada 0 (siempre transparente). Esto incluye los tiles de Modo 7.
- **Color directo:** no consulta CGRAM en absoluto — el valor de píxel se traduce directamente a un color RGB expandido (ver `tiles.md`).

### Color de fondo (backdrop)

La entrada 0 de CGRAM cumple una función especial: es el color que se ve allí donde no hay ningún BG ni sprite opaco (el "backdrop"), por debajo de todas las capas.

## Notas / gotchas

- El índice de píxel 0 es transparente en cualquier profundidad de color, así que la entrada 0 de cada grupo de paleta rara vez se usa como color visible dentro de un tile — pero la entrada 0 global de CGRAM sí se usa, como color de backdrop.
- En tiles 4bpp, la división entre paletas de fondo (0-127) y de sprite (128-255) es fija por hardware, no configurable.
- El acceso a CGRAM (lectura o escritura) solo es fiable durante VBlank, HBlank o forced blanking; escribir durante el renderizado activo apunta a una dirección incorrecta.
- El bit 15 (más significativo) de cada color no está implementado; al leer CGDATAREAD ese bit debe descartarse.

## Referencias cruzadas

- [Registros del PPU ($2100-$213F)](ppu-registers.md)
- [Tiles y formatos de bitplane](tiles.md)
- [Sprites (OAM)](sprites.md)
- [Color math](color-math.md)

---
titulo: Glosario de términos de desarrollo SNES
plataforma: [SNES]
categoria: [herramientas]
tags: [glosario, terminologia, ppu, cpu, audio, definiciones]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Glossary]
---

# Glosario de términos de desarrollo SNES

## Resumen

Glosario de referencia de la SNESdev Wiki con las definiciones de los términos técnicos más usados al hablar de la SNES, organizados en tres bloques: CPU, gráficos (PPU) y sonido. Útil como referencia rápida para descifrar la jerga que aparece en el resto de la documentación técnica de la consola.

## Detalle técnico

### CPU

- **5A22**: nombre que da Ricoh al chip S-CPU.
- **A Bus**: bus de direcciones de memoria al que se conectan la ROM, la WRAM y los coprocesadores.
- **B Bus**: bus de direcciones de E/S usado principalmente para la comunicación con la S-PPU y la S-SMP; la S-WRAM tiene un puerto en este bus pensado sobre todo para copiar bloques de la ROM a la RAM de trabajo.
- **DMA**: Direct Memory Access. Unidad del controlador de memoria de la S-CPU capaz de pausar brevemente el 65C816 para realizar copias, ya sea como copia de bloque o como HDMA.
- **HDMA**: Horizontal blank Direct Memory Access. Envía una palabra de datos a la PPU entre una scanline y la siguiente, para producir scroll por línea, formas de ventana y otros efectos de raster.
- **S-CPU**: procesador central, formado por un núcleo 65C816 licenciado de Western Design Center más un controlador de memoria propio.
- **S-WRAM**: Work RAM, una DRAM de 128 KiB.

### Gráficos

- **Background**: capa basada en rejilla, compuesta por un tileset, uno o varios tilemaps, un offset de scroll y atributos relacionados.
- **CGRAM**: 256 palabras de 15 bits de memoria situada en la PPU que almacena los datos de la paleta.
- **CHR**: character (carácter) — término alternativo para tile.
- **Nametable**: otro nombre para el tilemap.
- **Object**: terminología de Nintendo para referirse a un sprite.
- **OAM**: Object Attribute Memory. 544 bytes de memoria en la PPU que almacenan la tabla de sprites.
- **Palette**: tabla de búsqueda de colores.
- **Raster**: línea horizontal de la imagen de salida (también llamada scanline).
- **Scanline**: línea horizontal de la imagen de salida (también llamada raster).
- **Sliver**: segmento de 8x1 píxeles de un tile.
- **Sprite**: cuadrado de tiles de 16 colores (4bpp), habitualmente de 8x8, 16x16 o 32x32 píxeles, que puede dibujarse en cualquier parte de la pantalla.
- **Tile**: gráfico cuadrado pequeño. Los tiles de SNES son de 8x8 píxeles y se almacenan en distintos formatos en la VRAM.
- **Tilemap**: rejilla (en VRAM) que describe qué tiles y paletas dibujar para una capa de fondo.
- **Mode 7 Tilemap**: rejilla de 128x128 de índices de tile de 8 bits, para el fondo de Modo 7 (en VRAM).
- **Tilemap Entry**: palabra de 16 bits que contiene el índice de tile, el flip, el índice de paleta y la prioridad.
- **PPU**: Picture Processing Unit — genera la salida de vídeo a partir de la VRAM, la CGRAM y la OAM.
- **VRAM**: Video-RAM. La SNES tiene dos chips de Video-RAM de 32 KiB cada uno (64 KiB en total), donde se almacenan los datos de tile, los datos de tilemap y los datos de offset-per-tile.

### Sonido

- **ARAM**: Audio RAM. Memoria de 64 KiB que contiene el driver de sonido, la RAM de trabajo del driver, las secuencias, las muestras y el buffer de eco.
- **S-DSP**: el procesador digital de señal dentro de SHVC-Sound, encargado de reproducir muestras BRR y actualizar el buffer de eco.
- **S-SMP**: el núcleo de CPU dentro de SHVC-Sound. Se comunica con la S-CPU y traduce las secuencias musicales en comandos para el S-DSP.
- **SHVC-Sound**: el subsistema de audio de la SNES, diseñado por Sony. Está formado por el S-SMP, el S-DSP y la ARAM.
- **SPC700**: la arquitectura de 8 bits, similar al 65C02, sobre la que está construido el S-SMP.

## Notas / gotchas

- "Raster" y "scanline" son sinónimos exactos en esta terminología, igual que "CHR" y "tile", y "nametable" y "tilemap" — puede aparecer cualquiera de los dos términos indistintamente en otra documentación.
- Distinguir OAM (memoria física de 544 bytes en la PPU) de "sprite table" en sentido genérico: OAM es el nombre específico de esa memoria en SNES.

## Referencias cruzadas

- [Timing del sistema SNES](timing.md)
- [Erratas y quirks conocidos del hardware SNES](errata.md)
- (pendiente de enlazar)

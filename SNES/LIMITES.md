---
titulo: SNES — límites y presupuestos
plataforma: [SNES]
categoria: [referencia]
tags: [limites, vram, sprites, dma, interrupciones]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [entradas enlazadas en cada fila, todas extraídas de la SNESdev Wiki]
---

# SNES — límites y presupuestos

Todo viene de la SNESdev Wiki. **Ningún proyecto nuestro lo ha comprobado todavía.**

## Máquina

| Recurso | Límite | Entrada |
|---|---|---|
| CPU | 5A22 (65C816): 3,58 MHz en Fast-ROM y registros, 2,68 MHz en Slow-ROM y WRAM, 1,79 MHz en lo más lento | [timing](herramientas/timing.md) |
| WRAM | 128 KiB | [glosario](herramientas/glosario.md) |
| VRAM | 64 KiB; solo es seguro escribir en vblank o forced blank | [registros PPU](graficos/ppu-registers.md) |
| OAM | 544 bytes | [sprites](graficos/sprites.md) |
| CGRAM | 256 colores de 15 bits | [paletas](graficos/palettes.md) |
| ROM | LoROM, hasta 4 MiB | [mapa de memoria](memoria/memory-map.md) |
| DMA | 8 canales | [registros DMA](memoria/dma-registers.md) |

## Vídeo

| Recurso | Límite | Entrada |
|---|---|---|
| Sprites en total | 128 | [sprites](graficos/sprites.md) |
| Sprites por línea | 32 | [sprites](graficos/sprites.md) |
| Franjas de 8 px de sprite por línea | 34 | [sprites](graficos/sprites.md) |
| Modo 1 | 2 capas de 16 colores y 1 de 4 | [fondos](graficos/backgrounds.md) |
| Modo 0 | 4 capas de 4 colores | [fondos](graficos/backgrounds.md) |
| Modo 7 | 1 tilemap de 128×128, 256 tiles; usa la primera mitad de la VRAM | [fondos](graficos/backgrounds.md) |
| Color math en sprites | Solo en las últimas 4 paletas | [erratas](herramientas/errata.md) |

## Presupuesto de vblank

La tabla da lo máximo por DMA en todo el vblank, sin contar el resto de la rutina.

| Región | Altura | Líneas | Clocks | Bytes de DMA | Tiles 4 bpp | Tiles 8 bpp |
|---|---|---|---|---|---|---|
| NTSC | 224 | 37 | 48.988 | 6.123 | 191 | 95 |
| NTSC | 239 | 22 | 29.128 | 3.641 | 113 | 56 |
| PAL | 224 | 87 | 115.188 | 14.398 | 449 | 224 |
| PAL | 239 | 72 | 95.328 | 11.916 | 372 | 186 |

Fuente: [timing](herramientas/timing.md). Cada línea dura 1364 clocks, con 40 de pausa de la CPU por el refresco de DRAM.

## Sonido

| Recurso | Límite | Entrada |
|---|---|---|
| Voces | 8, con muestras BRR | [S-DSP](sonido/s-dsp-registers.md) |
| Audio RAM | 64 KiB, compartida entre driver, secuencias, muestras y eco | [glosario](herramientas/glosario.md) |
| Frecuencia real | 32.000–32.160 Hz según la consola | [timing](herramientas/timing.md) |

## Cálculo

| Operación | Coste | Entrada |
|---|---|---|
| División por hardware | 16 ciclos de CPU | [división](cpu/division.md) |

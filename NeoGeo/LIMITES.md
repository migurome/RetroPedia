---
titulo: Neo Geo — límites y presupuestos
plataforma: [NeoGeo]
categoria: [referencia]
tags: [limites, sprites, paletas, rom]
estado: borrador
comprobado: emulador
comprobado_en: "parcial: GnGeo y ngdevkit 0.5, RickAndMorty/neogeo; solo las filas marcadas como Medido"
fecha: 2026-11-06
fuentes: [NeoGeo/README.md, NeoGeo/graficos/sprites-scb.md, NeoGeo/graficos/rom-c-s-y-paletas.md]
---

# Neo Geo — límites y presupuestos

**Origen:** *Medido* se usó en `RickAndMorty/neogeo` con GnGeo, nunca en la consola. *Documentado* viene de ngdevkit o de
otra documentación, sin comprobar aquí.

## Máquina

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| CPU principal | 68000 a 12 MHz, sin FPU: punto fijo | Medido | [overview](README.md) |
| CPU de sonido | Z80 con su propia ROM M | Medido | [overview](README.md) |
| Sonido | YM2610: FM, SSG, ADPCM-A y ADPCM-B | Documentado | [overview](README.md) |
| Resolución | 320×224 | Medido | [overview](README.md) |
| Capa fix | 40×32 tiles de 8×8, siempre encima; en GnGeo se ven las columnas 1–38 | Medido | [sprites](graficos/sprites-scb.md) |

## Sprites y color

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| Tamaño de sprite | 1 tile de ancho (16 px), hasta 32 de alto; más ancho se encadena | Medido | [sprites](graficos/sprites-scb.md) |
| Total de sprites | 381 | Documentado | [overview](README.md) |
| Sprites por línea | 96 | Documentado; **sin verificar** | [sprites](graficos/sprites-scb.md) |
| Escala | Solo reducción | Documentado | [overview](README.md) |
| Paletas | 256 de 16 colores (el 0 es transparente); 4096 en pantalla | Documentado | [overview](README.md) |
| Colores por tile | Una sola paleta: 15 colores más transparente | Medido | [ROM C y paletas](graficos/rom-c-s-y-paletas.md) |

No hay planos de fondo de tiles: todo son sprites, salvo la capa fix.

## ROM (ejemplos de ngdevkit)

No son máximos del hardware, sino los tamaños usados en los ejemplos.

| ROM | Contenido | Tamaño |
|---|---|---|
| P1 | Programa 68000 | 1 MB |
| C1/C2 | Tiles de sprite 16×16 a 4 bpp | 2 MB cada una |
| S1 | Tiles de la capa fix | 128 KB |
| M1 | Driver de sonido Z80 | 128 KB |
| V1 | Samples ADPCM | 512 KB |

## Pendiente de medir

El límite de sprites por línea con muchas columnas a la misma altura, sobre todo con fondos hechos de sprites.

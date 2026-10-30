---
titulo: Neo Geo (AES / MVS) — overview
plataforma: [NeoGeo]
categoria: [general]
tags: [hardware, overview, ngdevkit]
estado: borrador
fecha: 2026-10-23
fuentes: [ngdevkit 0.5 (cabeceras, crt0, linkscript), ngdevkit-examples, proyecto RickAndMorty/neogeo]
---

# Neo Geo (AES / MVS)

## Resumen de hardware

Lo marcado con ✔ se comprobó en un proyecto real (`RickAndMorty/neogeo`, ngdevkit 0.5 + GnGeo). El resto es documentación pendiente de contrastar.

- CPU principal: Motorola 68000 a 12 MHz, sin FPU: la lógica va en punto fijo ([punto fijo](../GLOSARIO/fixed-point.md)). ✔ (24.8 en el proyecto)
- CPU de sonido: Z80, con su propia ROM M (driver). ngdevkit trae el driver vacío `nullsound`. ✔
- Sonido: YM2610 (FM, SSG, ADPCM-A y ADPCM-B), con los samples en la ROM V.
- Vídeo: **no hay planos de fondo de tiles**. Todo es sprites, salvo la capa fix de texto:
  - Sprites de 1 tile de ancho (16 px) y hasta 32 tiles de alto. Para dibujar algo más ancho se encadenan columnas. ✔
  - Capa fix: 40×32 tiles de 8×8, siempre por encima de los sprites. ✔
- Resolución: 320×224. GnGeo, como muchos televisores, recorta 8 px por lado: se ven las columnas 1–38 de la capa fix. ✔
- Color: paletas de 16 colores de 16 bits (el 0 es transparente). La paleta de cada tile se elige en su atributo. ✔
- Límites que da la documentación: 381 sprites, 96 por línea, solo reducción (sin ampliación) y 256 paletas (4096 colores en pantalla).

## Cartucho

Un juego son varias ROM:

| ROM | Contenido | Tamaño usado (ejemplos de ngdevkit) |
|---|---|---|
| P1 | programa 68000 (bytes de cada palabra intercambiados) | 1 MB |
| C1/C2 | tiles de sprite 16×16, 4 bpp, planos repartidos entre las dos | 2 MB cada una |
| S1 | tiles de la capa fix, 8×8 | 128 KB |
| M1 | driver de sonido Z80 | 128 KB |
| V1 | samples ADPCM | 512 KB |

## Categorías

- `herramientas/` — toolchain (ngdevkit en Windows), emulador y casos prácticos.
- `graficos/` — sprites (SCB1-4), formatos de las ROM C y S y paletas.

## Entradas

- [ngdevkit en Windows (MSYS2): instalación, build de la ROM y GnGeo](herramientas/ngdevkit-windows.md)
- [Sprites: tablas SCB1-SCB4, cadenas sticky, espejo y orden de dibujo](graficos/sprites-scb.md)
- [Formatos de la ROM C, la ROM S y las paletas](graficos/rom-c-s-y-paletas.md)
- [Caso práctico: personajes de SimuLockdown (Godot) en Neo Geo](herramientas/caso-practico-simulockdown.md)

---
titulo: Mega Drive — límites y presupuestos
plataforma: [MegaDrive]
categoria: [referencia]
tags: [limites, vram, dma, sprites, sgdk, rom]
estado: borrador
comprobado: emulador
comprobado_en: "parcial: BlastEm y emulador WASM de MD Engine, port de Super Smash Land; solo las filas marcadas como Medido"
fecha: 2026-11-06
fuentes: [entradas enlazadas en cada fila]
---

# Mega Drive — límites y presupuestos

**Origen:** *Medido* es de nuestros proyectos, en emulador. *Documentado* viene de documentación de terceros
(plutiedev, Bumbershoot, Doctor Ludos), sin comprobar aquí.

## Máquina

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| CPU | 68000 a ~7,6 MHz, sin caché; sin accesos de palabra a direcciones impares | Documentado | [problemas de hardware](memoria/hardware-issues.md) |
| RAM de trabajo | 64 KB | Documentado | [primeros pasos](herramientas/bec-getting-started.md) |
| RAM del Z80 | 8 KB | Documentado | [usar el Z80](cpu/using-the-z80.md) |
| ROM | 4 MB sin mapper; 8 MB con el de SSF2 en MD Engine | Documentado | [más de 4 MB](memoria/beyond-4mb.md) |
| Resolución | 320 o 256 de ancho; 224 de alto (240 con V30) | Documentado | [resolución](graficos/screen-resolution.md) |
| Margen seguro | Se pueden recortar 16 px a los lados y 8 arriba y abajo | Documentado | [problemas de hardware](memoria/hardware-issues.md) |

## Vídeo

| Recurso | 320 de ancho | 256 de ancho | Origen | Entrada |
|---|---|---|---|---|
| Sprites en pantalla | 80 | 64 | Documentado | [sprites](graficos/sprites.md) |
| Sprites por línea | 20 | 16 | Documentado | [sprites](graficos/sprites.md) |
| Píxeles de sprite por línea | 320 | 256 | Documentado | [sprites](graficos/sprites.md) |
| Bytes a VRAM por línea en vblank | 205 | 167 | Documentado | [ancho de banda](graficos/bumbershoot-badlines-revenge.md) |
| Bytes a VRAM por línea activa | 18 | 16 | Documentado | [ancho de banda](graficos/bumbershoot-badlines-revenge.md) |

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| VRAM | 64 KB; ~1536 tiles útiles descontando tablas | Documentado | [tiles y paletas](graficos/tiles-and-palettes.md) |
| Tile | 8×8, 32 bytes, 15 colores más transparente | Documentado | [tiles y paletas](graficos/tiles-and-palettes.md) |
| Color | 4 paletas de 16 de 512 posibles; 64 en pantalla | Documentado | [imitar paletas](graficos/imitar-paletas-otras-consolas.md) |
| Presupuesto a VRAM a 60 Hz, en H40 | **Planificar con 7.200 bytes**, lo que reserva SGDK 2.11. Máximo teórico: 7.524 según Sega Retro y Doctor Ludos; 7.380 con 36 líneas × 205 de Bumbershoot | Documentado, sin medir | [ancho de banda](graficos/bumbershoot-badlines-revenge.md), [caso SGDK](herramientas/caso-practico-sgdk-2019.md) |
| Presupuesto a VRAM a 60 Hz, en H32 | 6.118 bytes en vblank y 3.584 en pantalla activa, según Sega Retro | Documentado, sin medir | [ancho de banda](graficos/bumbershoot-badlines-revenge.md) |
| Presupuesto de SGDK a 50 Hz | `DMA_TRANSFER_CAPACITY_PAL_LOW` 8.000 bytes y `_PAL_MAX` 15.000 | Documentado (`inc/dma.h`) | [DMA](graficos/dma-transfer.md) |
| DMA | No cruza un límite de 128 KB de la fuente; longitud 0 es 65536 | Documentado | [DMA](graficos/dma-transfer.md) |

## Sonido

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| Chips | YM2612 (6 FM) y PSG (3 cuadradas y ruido) | Documentado | [imitar paletas](graficos/imitar-paletas-otras-consolas.md) |
| Reset del Z80 y el YM2612 | Mínimo de 192 ciclos del 68000 | Documentado | [problemas de hardware](memoria/hardware-issues.md) |

## Límites de SGDK y MD Engine

| Recurso | Límite | Origen | Entrada |
|---|---|---|---|
| Sprites de hardware por frame en rescomp | 16 | Medido (error del compilador) | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| Objetos del motor de sprites de SGDK | 80, contando cada actor y proyectil | Documentado (código de MD Engine) | [eventos](herramientas/md-engine/eventos-y-scripts.md) |
| Búfer temporal de DMA en MD Engine | 10.000 bytes por defecto | Medido | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |
| VRAM de sprites en MD Engine | ~700 tiles en el proyecto | Medido | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| Guardado en flash | Reserva la parte alta del cartucho: 242 ranuras de 1 KB bajo `TBCE_CART_SIZE - 64 KB` | Medido | [depuración](herramientas/md-engine/depuracion-y-generacion.md) |

## Costes de referencia (Super Smash Land MD)

| Operación | Coste | Origen | Entrada |
|---|---|---|---|
| Llamada a función con 9-11 registros y 8 argumentos | 200-300 ciclos | Medido | [optimizar C](cpu/optimizar-c-gcc-68000.md) |
| 5 sprites ocultos reservados, con 4 CPU | 1,5 % de fotogramas perdidos | Medido | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| Sprites a 1,5x, con 4 luchadores | Pérdidas del 7-10 % al 19-21 % | Medido | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |
| Fondos a 1,5x | El doble de tiles: de 293 a 586 en el peor | Medido | [sprites de SGDK](herramientas/sgdk/tuto-sprites.md) |

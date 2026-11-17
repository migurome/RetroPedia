---
titulo: SNES — overview
plataforma: [SNES]
categoria: [general]
tags: [referencia]
estado: borrador
comprobado: no
fecha: 2026-07-21
fuentes: [resumen de las entradas de esta carpeta]
---

# Super Nintendo (SNES)

## Empieza aquí

[Límites y presupuestos](LIMITES.md) · [Síntomas y causas](SINTOMAS.md)

Todo viene de la documentación: ningún proyecto nuestro lo ha comprobado todavía.

| Si vas a... | Lee |
|---|---|
| Elegir herramientas | [herramientas](herramientas/tools.md) y [tutoriales](herramientas/tutoriales.md) |
| Aprender la CPU | [65C816 desde el 6502](cpu/65c816-para-desarrolladores-6502.md) |
| Crear la ROM | [mapa de memoria](memoria/memory-map.md) y [cabecera](memoria/rom-header.md) |
| Organizar el vblank | [interrupciones](graficos/vblank-interrupts.md) y [rutina de VBlank](graficos/vblank-routine.md) |
| Calcular cuánto cabe por frame | [timing](herramientas/timing.md) |
| Dibujar fondos | [fondos](graficos/backgrounds.md) y [tilemaps](graficos/tilemaps.md) |
| Mostrar sprites | [sprites](graficos/sprites.md) |
| Mover datos | [ejemplos de DMA](memoria/dma-ejemplos.md) |
| Hacer efectos | [HDMA](graficos/hdma-ejemplos.md), [color math](graficos/color-math.md) y [modo 7](graficos/modo-7-efectos-perspectiva.md) |
| Añadir sonido | [arranque del SPC700](sonido/arranque-spc700.md) y [muestras BRR](sonido/brr-samples.md) |
| Leer el mando | [lectura de mando](perifericos/lectura-de-mando.md) |
| Evitar fallos solo en consola | [erratas](herramientas/errata.md) |

## Resumen de hardware (a validar/ampliar durante el proyecto)

- CPU principal: Ricoh 5A22 (basada en 65c816)
- Vídeo: PPU1 + PPU2 (dos chips)
- Sonido: SPC700 + DSP (procesador de audio dedicado, con su propia RAM)
- Resoluciones habituales: 256x224 / 512x224 (hi-res)

Esta ficha es un punto de partida — se ampliará y corregirá con detalle real a medida que trabajemos en proyectos concretos de SNES.

## Categorías

- `graficos/` — PPU1/PPU2, registros, fondos, tiles, sprites, paletas, ventanas, color math, modo 7, HDMA aplicado a vídeo
- `sonido/` — SPC700, S-DSP, muestras BRR, envolventes
- `cpu/` — 65C816 ASM, modos de direccionamiento, vectores, multiplicación/división
- `memoria/` — mapa de memoria, cabecera de ROM, DMA, formatos de ROM/save
- `perifericos/` — mando estándar, ratón, multitap, Super Scope, conectores
- `avanzado/` — chips de expansión (Super FX, DSP-x, CX4)
- `herramientas/` — toolchain, timing, errata, glosario, tests de emuladores

## Entradas

Contenido extraído de [SNESdev Wiki](https://snes.nesdev.org/wiki/SNESdev_Wiki) (2026-07-21), `estado: borrador` — ver [INDEX.md](../INDEX.md) para el listado completo enlazado por categoría. No se documentaron los pinouts eléctricos de chips internos (S-CPUN, APU, CPU, PPU, DSP, SA-1, S-RGB, S-ENC, S-MIX, CIC, MAD-1, WRAM) por ser referencia de ingeniería electrónica para reparación/clonado de hardware, no técnica de desarrollo de juegos — sí se conservaron los conectores de cartucho y mando, más relevantes para periféricos/homebrew físico.

| Categoría | Nº entradas |
|---|---|
| cpu/ | 9 |
| graficos/ | 17 |
| sonido/ | 7 |
| memoria/ | 7 |
| perifericos/ | 10 |
| herramientas/ | 9 |

**Sin entradas en `avanzado/`**: las tres páginas de chips de expansión (DSP_Expansion, Super FX, CX4) de la SNESdev Wiki resultaron ser stubs sin contenido técnico real (solo listas de qué juego usó cada chip) — no se inventó contenido. Pendiente de una futura ronda con otra fuente (p. ej. superfamicom.org o el propio código de emuladores como bsnes) si se necesita documentar estos coprocesadores en profundidad.

Otras páginas de la wiki que resultaron ser stubs sin contenido desarrollado y por tanto no se documentaron: `65C816` (índice general — el detalle real ya está en `cpu/65c816-para-desarrolladores-6502.md`), `Variable_width_fonts` (solo una lista de juegos), `Copier` y `Standard_controller`/`Controller_reading` (más breves de lo esperado, documentados fielmente con ese límite señalado).

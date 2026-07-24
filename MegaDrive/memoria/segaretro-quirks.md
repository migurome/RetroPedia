---
titulo: Quirks de hardware documentados por Sega Retro
plataforma: [MegaDrive]
categoria: [memoria]
tags: [quirks, hardware, tas, clr, vdp, segaretro, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://segaretro.org/Mega_Drive_Quirks]
---

# Quirks de hardware documentados por Sega Retro

## Resumen

La página ["Sega Mega Drive/Quirks"](https://segaretro.org/Mega_Drive_Quirks) de Sega Retro recopila peculiaridades de hardware de la Mega Drive que no encajan en ninguna otra categoría — complementa [`hardware-issues.md`](hardware-issues.md) (fuente plutiedev.com) y las notas de [`kabuto-hardware-notes.md`](kabuto-hardware-notes.md).

## Detalle técnico

### El crash de `clr` sobre el puerto de datos del VDP

Usar la instrucción `clr` sobre el puerto de datos del VDP mientras está en modo de escritura VRAM/CRAM/VSRAM provoca un **cuelgue del sistema**: `clr` se trata como una instrucción de tipo read-modify-write, lo que hace que la CPU intente *leer* el puerto de datos del VDP mientras está en modo escritura, y eso cuelga el sistema. *(Esto corrobora, con más detalle del mecanismo exacto, la advertencia que ya aparecía en [`vdp-setup.md`](../graficos/vdp-setup.md) sobre no usar `clr`.)*

### La instrucción TAS y el bus

La instrucción `TAS` (test-and-set) usa un ciclo de bus distinto al resto de instrucciones. En la práctica, tanto en hardware MD1 como MD2, `TAS` se comporta como un **no-op para escrituras en memoria** — es decir, el efecto de escritura de `TAS` no llega a producirse realmente en el hardware de Mega Drive, a diferencia de lo que dictaría la especificación estricta del 68000.

## Notas / gotchas

- El quirk de `TAS` es relevante si se usa esa instrucción para primitivas de sincronización o flags — en Mega Drive no hace lo que un desarrollador familiarizado con el 68000 "de libro" esperaría.
- Contenido reconstruido vía WebSearch (la wiki de Sega Retro da 403 — ver [`../herramientas/segaretro-overview.md`](../herramientas/segaretro-overview.md)); la página original probablemente tiene más quirks no capturados aquí — revisarla directamente si aparece un bug raro que no encaje con el comportamiento esperado del 68000/VDP.

## Referencias cruzadas

- [`hardware-issues.md`](hardware-issues.md), [`kabuto-hardware-notes.md`](kabuto-hardware-notes.md) (fuente plutiedev.com)
- [`../graficos/vdp-setup.md`](../graficos/vdp-setup.md)
- [Hilos de SpritesMind sobre diferencias entre revisiones de hardware](spritesmind-hardware-revisiones-hilos.md)
- [Sega Retro — overview y limitaciones](../herramientas/segaretro-overview.md)

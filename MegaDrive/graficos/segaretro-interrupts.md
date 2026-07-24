---
titulo: Interrupciones del VDP y externas (VInt, HInt, external)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [interrupciones, vdp, vblank, hblank, vint, hint, segaretro, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://segaretro.org/Category:Mega_Drive_technical_information, https://segaretro.org/images/1/18/GenesisTechnicalOverview.pdf]
---

# Interrupciones del VDP y externas (VInt, HInt, external)

## Resumen

La Mega Drive tiene tres fuentes de interrupción relevantes para el desarrollo, todas controladas por bits `IEx` en los registros del VDP ya documentados en [`vdp-registers.md`](vdp-registers.md) (fuente plutiedev.com): la interrupción vertical (VInt/vblank), la horizontal (HInt/hblank) y la externa. Esta entrada añade el detalle de niveles de interrupción del 68000 y semántica exacta que no estaba recogido en la entrada original de registros.

## Detalle técnico

### Las tres interrupciones

- **`IE0`** — activa la interrupción de **vblank** (VInt). Ocurre justo después del retrace vertical.
- **`IE1`** — activa la interrupción **horizontal** (HInt), que se comporta como una interrupción de **nivel 4** para el 68000. Ocurre justo antes del retrace horizontal.
- **`IE2`** — activa la interrupción **externa**, disparada por el pin `TH` de los tres puertos de I/O (es el mecanismo que usan las light guns, p. ej. Menacer/Justifier). Al poner `TH` a nivel alto se ejecuta la rutina de interrupción externa como interrupción de **nivel 2** en el 68000.

### Contador de línea para HInt (registro VDP #10)

El registro #10 del VDP controla cada cuántas líneas de scan ocurre la interrupción horizontal:
- `$00` → ocurre en cada línea
- `$01` → ocurre cada dos líneas
- `$02` → ocurre cada tres líneas

(Y así sucesivamente — el valor indica cuántas líneas adicionales se saltan entre disparos consecutivos de HInt.)

## Notas / gotchas

- Esta entrada complementa pero no sustituye [`vdp-setup.md`](vdp-setup.md) y [`vdp-registers.md`](vdp-registers.md) (fuente plutiedev.com), que ya mencionan los bits `IE0`/`IE1` en los registros de modo pero sin detallar los niveles de interrupción del 68000 ni la semántica del contador de HInt.
- Contenido reconstruido vía WebSearch (la wiki de Sega Retro da 403 — ver [`../herramientas/segaretro-overview.md`](../herramientas/segaretro-overview.md)); contrastar con la página original o con `genvdp.txt` de Charles MacDonald antes de depender de los niveles de interrupción exactos para escribir un manejador de interrupciones real.

## Referencias cruzadas

- [`vdp-setup.md`](vdp-setup.md), [`vdp-registers.md`](vdp-registers.md) (fuente plutiedev.com)
- [Sega Retro — overview y limitaciones](../herramientas/segaretro-overview.md)
- [Hilos de SpritesMind sobre internals del VDP](spritesmind-vdp-hilos.md)

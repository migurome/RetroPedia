---
titulo: Hilos de SpritesMind sobre diferencias entre revisiones de hardware
plataforma: [MegaDrive]
categoria: [memoria]
tags: [spritesmind, foro, revisiones, hardware, vdp, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://gendev.spritesmind.net/forum/viewtopic.php?t=977, https://gendev.spritesmind.net/forum/viewtopic.php?t=3175, https://gendev.spritesmind.net/forum/viewtopic.php?t=2604]
---

# Hilos de SpritesMind sobre diferencias entre revisiones de hardware

## Resumen

Índice curado de hilos que documentan diferencias de comportamiento entre las distintas revisiones de placa de la Mega Drive (VA a VA3/VA4, modelos posteriores, versiones "Model 1"/"Model 2", etc.) — relevante para cualquier juego/demo que dependa de timing fino o de comportamiento indocumentado del VDP, ya que puede variar entre consolas reales. Ver la advertencia de fidelidad en [`../herramientas/spritesmind-overview.md`](../herramientas/spritesmind-overview.md): esto es un índice construido desde fragmentos de búsqueda, no una transcripción completa.

## Detalle técnico

### [Differences between Mega Drive versions? (VDP in particular)](https://gendev.spritesmind.net/forum/viewtopic.php?t=977)
Discusión centrada específicamente en diferencias del VDP entre revisiones (contenido no detallado en los fragmentos disponibles — abrir el hilo).

### [List of every detectable difference in Mega Drive revisions?](https://gendev.spritesmind.net/forum/viewtopic.php?t=3175)
Intento de la comunidad de compilar una lista exhaustiva de diferencias detectables por software entre revisiones — probablemente el hilo más útil como checklist si un proyecto necesita comportarse igual en todas las consolas reales.

### [Titan's Mega Drive tech docs including Debug Register](https://gendev.spritesmind.net/forum/viewtopic.php?t=2604)
Notas técnicas de Titan/Kabuto sobre hardware de Mega Drive, incluyendo el registro de depuración del VDP. **Posible mismo autor/origen que** [`kabuto-hardware-notes.md`](kabuto-hardware-notes.md) (mirror ya presente en esta base, extraído vía plutiedev.com) — contrastar ambas fuentes antes de asumir que son idénticas o de citar una en vez de la otra.

## Notas / gotchas

- Antes de confiar en un comportamiento concreto de hardware en un proyecto real, verificar si depende de la revisión de consola — estos hilos son el punto de partida para saberlo.

## Referencias cruzadas

- [`kabuto-hardware-notes.md`](kabuto-hardware-notes.md), [`hardware-issues.md`](hardware-issues.md) (fuente plutiedev.com)
- [`../graficos/spritesmind-vdp-hilos.md`](../graficos/spritesmind-vdp-hilos.md)
- [SpritesMind — overview](../herramientas/spritesmind-overview.md)

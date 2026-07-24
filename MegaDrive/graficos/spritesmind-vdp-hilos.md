---
titulo: Hilos de SpritesMind sobre internals del VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [spritesmind, foro, vdp, fifo, timing, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://gendev.spritesmind.net/forum/viewtopic.php?t=1291, https://gendev.spritesmind.net/forum/viewtopic.php?t=740, https://gendev.spritesmind.net/forum/viewtopic.php?t=851, https://gendev.spritesmind.net/forum/viewtopic.php?t=2036, https://gendev.spritesmind.net/forum/viewtopic.php?t=291, https://gendev.spritesmind.net/forum/viewtopic.php?t=1206, https://gendev.spritesmind.net/forum/viewtopic.php?t=1420, https://gendev.spritesmind.net/forum/viewtopic.php?t=1368]
---

# Hilos de SpritesMind sobre internals del VDP

## Resumen

Índice curado de hilos del foro SpritesMind.Net que profundizan en el comportamiento real del VDP (315-5313) más allá de lo documentado en `plutiedev.com` — sobre todo el trabajo de ingeniería inversa de **Nemesis** con analizador lógico sobre el FIFO, timing de acceso a VRAM y registros. Ver la advertencia de fidelidad en [`../herramientas/spritesmind-overview.md`](../herramientas/spritesmind-overview.md): esto es un índice construido desde fragmentos de búsqueda, no una transcripción completa — hay que abrir cada hilo para el detalle real.

## Detalle técnico

### [VDP Internals](https://gendev.spritesmind.net/forum/viewtopic.php?t=1291)
Hilo largo (múltiples páginas) con el análisis más profundo del funcionamiento interno del VDP. Incluye el trabajo de Nemesis sobre la ROM "VDP FIFO Testing" para caracterizar el FIFO con hardware real. Según los fragmentos disponibles:
- Normal writes y las DMA de 68K→VRAM pasan por el FIFO.
- Los DMA fill no pasan por el FIFO pero lo usan como fuente de valor.
- Los DMA copy no interactúan con el FIFO en absoluto, usan los mismos slots externos que otros accesos.
- El flag de DMA probablemente NO se marca si una escritura que debería iniciar una DMA está retenida en el FIFO sin procesar todavía.

### [VDP Status Register](https://gendev.spritesmind.net/forum/viewtopic.php?t=740)
El registro de estado del VDP es de 10 bits; una lectura de 16 bits devuelve esos 10 bits en la parte baja.

### [VDP VRAM access timing](https://gendev.spritesmind.net/forum/viewtopic.php?t=851)
Timing de acceso a VRAM (discusión multi-página, no resumida en detalle aquí — abrir el hilo).

### [Understanding VDP](https://gendev.spritesmind.net/forum/viewtopic.php?t=2036)
Hilo divulgativo sobre el funcionamiento general del VDP, probablemente buen punto de entrada antes de "VDP Internals".

### [VDP registers timings](https://gendev.spritesmind.net/forum/viewtopic.php?t=291)
Timings asociados a los registros del VDP (no resumido en detalle — abrir el hilo).

### [VDP Debug Register - $C0001C](https://gendev.spritesmind.net/forum/viewtopic.php?t=1206)
Documenta el registro de depuración del VDP en `$C0001C`, probablemente usado durante el diseño del propio chip. Complementa la entrada [`vdp-debug.md`](vdp-debug.md) (fuente plutiedev) y [`kabuto-hardware-notes.md`](../memoria/kabuto-hardware-notes.md).

### [Mega Drive VDP Debug Settings Window](https://gendev.spritesmind.net/forum/viewtopic.php?t=1420)
Relacionado con el mismo registro de depuración; posible herramienta/ventana de configuración asociada.

### [VDP 128Kb Extended VRAM mode](https://gendev.spritesmind.net/forum/viewtopic.php?t=1368)
El VDP 315-5313 tiene un bit `AP16` para la dirección base del generador de patrones de sprites, que solo tiene efecto en modo VRAM extendida (128KB).

## Notas / gotchas

- Todo lo anterior son resúmenes de segunda mano vía fragmentos de búsqueda — antes de implementar algo basado en un timing o comportamiento concreto de aquí, abrir el hilo original y contrastar.

## Referencias cruzadas

- [`vdp-debug.md`](vdp-debug.md), [`vdp-registers.md`](vdp-registers.md), [`dma-transfer.md`](dma-transfer.md) (fuente plutiedev.com)
- [`../memoria/kabuto-hardware-notes.md`](../memoria/kabuto-hardware-notes.md)
- [SpritesMind — overview](../herramientas/spritesmind-overview.md)

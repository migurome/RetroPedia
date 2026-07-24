---
titulo: Hilos de SpritesMind sobre timing de bus, DMA y Z80
plataforma: [MegaDrive]
categoria: [cpu]
tags: [spritesmind, foro, bus, dma, z80, 68000, timing, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://gendev.spritesmind.net/forum/viewtopic.php?t=2943, https://gendev.spritesmind.net/forum/viewtopic.php?t=2470, https://gendev.spritesmind.net/forum/viewtopic.php?t=1842, https://gendev.spritesmind.net/forum/viewtopic.php?t=1661, https://gendev.spritesmind.net/forum/viewtopic.php?t=1091]
---

# Hilos de SpritesMind sobre timing de bus, DMA y Z80

## Resumen

Índice curado de hilos sobre la arbitración de bus entre el 68000, el VDP y el Z80 durante operaciones DMA — un área donde el comportamiento real del hardware tiene matices que no siempre están en los tutoriales de alto nivel. Ver la advertencia de fidelidad en [`../herramientas/spritesmind-overview.md`](../herramientas/spritesmind-overview.md): esto es un índice construido desde fragmentos de búsqueda, no una transcripción completa.

## Detalle técnico

### [Genesis 68K bus timing](https://gendev.spritesmind.net/forum/viewtopic.php?t=2943)
Según los fragmentos disponibles:
- `BR` (bus request) lo activa el VDP inmediatamente después de la escritura que dispara una DMA 68K→VDP, lo bastante pronto como para que la segunda word de una escritura long se retrase hasta que termine la DMA si la primera escritura ya la dispara.
- En pruebas con el 68K constantemente en el bus, el tiempo `!BR` a `!BG` fue a veces de 3 ciclos del 68K con un total de 4 wait states; el retraso medio entre accesos fue justo por debajo de 3 ciclos.

### [Z80 and DMA](https://gendev.spritesmind.net/forum/viewtopic.php?t=2470)
Según los fragmentos disponibles: hay que detener (halt) el Z80 al hacer una transferencia DMA — no hay forma de evitarlo salvo indicarle al driver de sonido que se mantenga fuera del bus del 68000 durante ese tiempo.

### [Question about bus arbiter](https://gendev.spritesmind.net/forum/viewtopic.php?t=1842)
Tanto el VDP como el árbitro de bus del Z80 usan el mecanismo de bus request del 68K para obtener acceso; si el VDP ya ha solicitado el bus, el Z80 no obtiene acceso hasta que termine la operación DMA.

### [Wait States on Z80 Bank Access](https://gendev.spritesmind.net/forum/viewtopic.php?t=1661)
Con el 68000 funcionando con normalidad, hay típicamente un retraso de 3 ciclos de Z80 para acceder al área bankeada (banked area), aunque puede llegar a 4 ciclos en el peor caso.

### [M68000 Microcode-level emulation](https://gendev.spritesmind.net/forum/viewtopic.php?t=1091)
Hilo sobre emulación a nivel de microcódigo del 68000 — más relevante para quien escriba un emulador que para desarrollo de juegos, pero con detalle profundo sobre el comportamiento interno de la CPU.

## Notas / gotchas

- Estos datos de timing son citas de fragmentos de búsqueda, no verificadas contra el hilo completo — para código sensible al timing (efectos raster, sincronización de DMA con el Z80), contrastar en el hilo original antes de confiar en el número exacto.
- Relacionar con [`../graficos/dma-transfer.md`](../graficos/dma-transfer.md) (fuente plutiedev), que ya advierte sobre el riesgo de DMA con el Z80 activo pero sin el detalle de ciclos exactos que aparece aquí.

## Referencias cruzadas

- [`using-the-z80.md`](using-the-z80.md) (fuente plutiedev.com)
- [`../graficos/dma-transfer.md`](../graficos/dma-transfer.md)
- [SpritesMind — overview](../herramientas/spritesmind-overview.md)

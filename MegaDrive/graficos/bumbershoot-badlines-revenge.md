---
titulo: "Bumbershoot Software — presupuesto de ancho de banda a VRAM y la \"venganza\" de las badlines del C64"
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bumbershoot, vdp, dma, vram, timing, contencion-de-bus, c64, badlines]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/04/08/genesis-badlines-revenge/]
---

# Bumbershoot Software — presupuesto de ancho de banda a VRAM y la "venganza" de las badlines del C64

## Resumen

Artículo corto (publicado un día después del de "pseudo-bitmap mode", como continuación directa de ese mismo proyecto) donde el autor descubre que, al intentar refrescar la pantalla de su autómata celular, el VDP de Genesis tiene su propia versión del problema de las "badlines" del VIC-II del Commodore 64: un límite duro de cuántos bytes se pueden transferir a VRAM según en qué parte del frame esté la pantalla. Documenta las tasas exactas de transferencia y cómo repartió una actualización de 8KB entre pantalla activa y VBLANK para no romper el scroll.

## Detalle técnico

### El problema general de contención de bus (contexto que da el autor)

Varios sistemas resuelven de forma distinta el conflicto de que "varios chips quieren leer o escribir en memoria a la vez, pero un cable solo puede llevar una señal cada vez":

- **NES / Atari 2600:** sin arbitraje real; los desarrolladores evitaban conflictos de escritura haciendo coincidir valores con lo que ya había en memoria.
- **Game Boy:** la PPU protege el acceso a su propia VRAM durante el renderizado activo, pero la responsabilidad de no pisarse recae en el programador.
- **Commodore 64:** en operación normal, el acceso a memoria está repartido de antemano entre VIC-II y CPU; pero durante las **badlines**, el VIC-II actúa de árbitro y puede **detener la CPU por completo** cuando necesita acceso a memoria al doble de velocidad de lo normal (para releer los datos de la pantalla de texto/attribute).
- **IBM PC (1981):** el 8088 tiene un bus externo de 8 bits pero un set de instrucciones pensado para 16, así que el bus se bloquea en operación normal para poder servir ambos anchos.

### La "venganza": el VDP de Genesis no detiene la CPU, pero raciona el DMA a VRAM

A diferencia de una badline del C64 (que roba ciclos de CPU directamente), el VDP de Genesis nunca detiene al 68000. Lo que hace es limitar drásticamente cuántos bytes puede mover una transferencia a VRAM según si el rayo está en pantalla activa o en VBLANK:

- **Durante VBLANK:** 205 bytes por línea de scan (pantalla de 320px de ancho); 167 bytes/línea en modo de pantalla estrecha (256px).
- **Durante pantalla activa:** solo 18 bytes por línea (320px); 16 bytes/línea en modo estrecho (256px).

Es decir, la ventana de VBLANK transfiere a más de **10 veces** la velocidad de la pantalla activa, porque durante el refresco activo el propio VDP ya está ocupando casi todo el ancho de banda de VRAM para leer patrones y nametables de cara al renderizado.

### Presupuesto de un frame completo

Con 36 líneas de VBLANK y 224 líneas de pantalla activa (total NTSC = 260 líneas visibles+blanking relevantes a este cálculo):

- VBLANK: 36 × 205 = 7.380 bytes
- Pantalla activa: 224 × 18 = 4.032 bytes
- **Total transferible por frame: 11.412 bytes**

### El problema concreto del proyecto (autómata celular / modo pseudo-bitmap)

El autor necesitaba refrescar del orden de 16KB de datos de patrón por frame para su modo pseudo-bitmap (ver entrada relacionada sobre el diseño de ese modo) — cifra que **supera el total transferible en un solo frame** (11.412 bytes), incluso usando el 100% del VBLANK y de la pantalla activa. La actualización directa, ingenua, causaría "hitches" visibles en otras partes de la pantalla (notablemente el scroll) cada vez que se intentara.

**Solución adoptada:** partir la actualización en dos frames, transfiriendo unos 8KB por frame en vez de 16KB de golpe:

- Blit a mitad de frame (durante pantalla activa): 224 líneas × 18 bytes/línea = **4.032 bytes**.
- Resto pendiente para VBLANK: 8.192 − 4.032 = 4.160 bytes, que a 205 bytes/línea requieren **21 líneas de VBLANK**.
- Como el VBLANK dispone de 36 líneas, quedan **15 líneas de margen** ("wiggle room") para lo que el resto del motor necesite hacer durante ese tiempo (paletas, sprites, sonido, etc.).

## Notas / gotchas

- El mecanismo es distinto en su naturaleza al de las badlines del C64 (allí se detiene la CPU; aquí se limita el caudal de DMA), pero el efecto práctico es el mismo tipo de dolor de cabeza: hay que presupuestar cuántos bytes caben en cada tramo del frame antes de diseñar una estructura de datos que dependa de refrescos grandes y frecuentes.
- Las tasas de 205/18 bytes por línea (320px) y 167/16 (256px) no aparecen documentadas con esa granularidad en la entrada general de [Transferencias DMA](dma-transfer.md) de esta base (que cubre registros y el límite de 128KB, pero no un presupuesto de bytes-por-línea); esta entrada aporta ese dato concreto que falta allí.
- La recomendación ya existente en `dma-transfer.md` de encolar los comandos DMA durante el frame y lanzarlos todos en VBLANK cobra más sentido con estos números: en pantalla activa el margen es tan estrecho (18 bytes/línea) que cualquier DMA no trivial debe reservarse casi por completo para VBLANK.
- Aplica sobre todo a diseños que necesitan refrescos grandes y periódicos de VRAM (como el modo pseudo-bitmap de la entrada relacionada); para actualizaciones puntuales y pequeñas (unos pocos tiles por frame) el margen de pantalla activa suele ser suficiente.

## Referencias cruzadas

- [Bumbershoot Software — modo pseudo-bitmap mediante dos planos de scroll superpuestos](bumbershoot-pseudo-bitmap-dos-planos.md) — el proyecto concreto (autómata celular) que motivó este análisis de ancho de banda; el límite de 16KB/frame descrito aquí es justo el problema que surge al intentar refrescarlo.
- [Transferencias DMA](dma-transfer.md) — registros, direcciones y límite de 128KB para configurar una transferencia DMA; esta entrada añade el dato de tasa de bytes por línea que falta allí.
- [Registros del VDP](vdp-registers.md)

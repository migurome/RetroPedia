---
titulo: Caso práctico — hacer un juego de Mega Drive en 2019 (Doctor Ludos / SGDK)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, caso-practico, workflow, vram, sprites, paleta, homebrew]
estado: borrador
fecha: 2026-07-21
fuentes: [https://www.gamedeveloper.com/design/making-a-sega-mega-drive-genesis-game-in-2019]
---

# Caso práctico — hacer un juego de Mega Drive en 2019 (Doctor Ludos / SGDK)

## Resumen

Artículo de Doctor Ludos (Gamedeveloper.com, 19/10/2019) contando su experiencia real desarrollando tres juegos homebrew para Mega Drive publicados en cartucho físico (por Cote Gamers): *30 Years of Nintendon't* (tributo por el 30 aniversario de la consola), *Break An Egg* (hecho en un game jam de 60 minutos) y *MeteoRain* (con efectos de partículas). Es un complemento práctico a los tutoriales de SGDK ya documentados: muestra el flujo de trabajo real y los límites de hardware con los que te topas en un proyecto de verdad, no solo en el "hello world".

## Detalle técnico

### Herramientas usadas

- **[SGDK](sgdk/overview.md)** como framework principal — permite programar en C, con gestión automática de sprites, VRAM y varios drivers de audio (PCM y XGM).
- **BlastEm** — emulador usado para pruebas rápidas durante el desarrollo.
- **Mega Everdrive** — flashcart para probar en hardware real.
- **Deflemask** — tracker usado para componer la música chiptune.
- Editores gráficos habituales (Photoshop, GIMP, Aseprite) para el arte — SGDK convierte automáticamente a la paleta de Mega Drive.
- Efectos de sonido en `.wav`; música exportada a formato VGM para que SGDK la convierta.

### Los tres cuellos de botella principales de la plataforma

1. **Transferencia a VRAM:** presupuesto de **~7524 bytes por frame** (unos 7KB) para mover datos gráficos de la ROM a la VRAM, a 60fps. Este es el límite real que condiciona cuánto arte nuevo puedes mostrar en pantalla por frame.
2. **Límite de sprites:** hasta **80 sprites en total**, con tamaños de hasta 32x32 píxeles; la prioridad/orden se gestiona mediante el sistema de "cadena de sprites" (sprite chain) — coincide con lo ya documentado en [`../graficos/sprites.md`](../graficos/sprites.md) y [`../graficos/sprite-table.md`](../graficos/sprite-table.md) (fuente plutiedev.com).
3. **Restricción de paleta de color:** solo **64 colores en pantalla simultáneamente** de una paleta total de 512 — en comparación, la SNES permite 256 colores simultáneos (dato relevante también para cuando se documente SNES en esta base).

### Lecciones técnicas clave

- El 68000 compila C de forma eficiente, lo que hace de la Mega Drive "posiblemente la máquina más fácil para desarrollar un juego homebrew" en comparación con otras consolas retro.
- El autor consiguió 45 sprites de meteoritos y explosiones de 40 partículas corriendo a 60fps, como demostración práctica de que los límites de arriba son manejables con buena gestión.
- El motor de sprites de SGDK automatiza la transferencia a VRAM, la conversión de sprite lógico a sprite de hardware, y la gestión de prioridad — resolviendo la complejidad con la que lidiaban los desarrolladores de los 90 trabajando directamente en ensamblador.
- Sobre audio: el driver PCM se usa para muestras de voz de alta calidad, y el driver XGM para música más 4 efectos simultáneos — la elección depende de las necesidades concretas del juego.

### Flujo de trabajo de assets

- Arte: se crea con el editor 2D preferido; SGDK convierte automáticamente a la paleta de Mega Drive.
- Música: se exporta a VGM y SGDK la convierte a su formato interno.
- Pruebas: primero en emulador (BlastEm), luego en hardware real vía Everdrive.
- Organización: drivers de audio distintos según el contexto de juego (por ejemplo, XGM durante el gameplay normal, PCM para momentos con voces).

## Notas / gotchas

- El límite de ~7KB/frame de transferencia a VRAM es probablemente el dato más accionable de este artículo para diseñar un juego: condiciona cuánta variedad de tiles nuevos puedes cargar por frame sin ralentizar el juego.
- El logro de hacer un juego jugable completo en 60 minutos (limitado por el cambio de hora de verano) ilustra bien cuánto simplifica SGDK el desarrollo frente a la Mega Drive "cruda" en ensamblador de las entradas de plutiedev.com.

## Referencias cruzadas

- [SGDK — overview](sgdk/overview.md), [Tutorial: Sprites](sgdk/tuto-sprites.md)
- [`../graficos/sprites.md`](../graficos/sprites.md), [`../graficos/sprite-table.md`](../graficos/sprite-table.md), [`../graficos/dma-transfer.md`](../graficos/dma-transfer.md) (fuente plutiedev.com)
- [`../sonido/ym2612-operations.md`](../sonido/ym2612-operations.md), [`../sonido/psg.md`](../sonido/psg.md)

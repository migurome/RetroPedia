---
titulo: Comparativa de emuladores para desarrollo en Mega Drive/Genesis
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [emuladores, gens-kmod, dgen, exodus, bizhawk, jgenesis, blastem, debug, precision, referencia-indirecta]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/platform-guide-sega-genesis-mega-drive/]
---

# Comparativa de emuladores para desarrollo en Mega Drive/Genesis

## Resumen

La guía de plataforma de Bumbershoot Software incluye una valoración personal del autor sobre qué emulador conviene usar según la tarea: visualización de gráficos/sonido, depuración interactiva, caza de bugs de timing, o precisión general. No hay un único "mejor" emulador — cada uno cubre un hueco distinto del flujo de trabajo de un desarrollador homebrew.

## Detalle técnico

| Emulador | Punto fuerte según el autor | Cuándo usarlo |
|---|---|---|
| **Gens KMod** | Los mejores visualizadores de gráficos y sonido de todos los emuladores de Genesis que ha probado (paletas, tiles, sprites, registros de los chips). | Inspeccionar visualmente VRAM/CRAM/estado de sprites mientras se depura una escena. |
| **DGen** | Los depuradores interactivos más usables que ha encontrado; el único que le permitió depurar correctamente código para el coprocesador Z80. | Depuración paso a paso, en particular del lado Z80 (sonido/PSG). |
| **Exodus** | Emulador "precisión a cualquier precio" ("accuracy at any cost"). Requiere más configuración inicial, pero atrapa problemas que otros emuladores dejan pasar. | Cazar bugs sutiles de timing/hardware que solo aparecen con emulación muy fiel. |
| **BizHawk** | Colección multiplataforma de emuladores pensada para tool-assisted speedruns; su núcleo de Genesis se basa en Genesis-Plus GX, con emulación excepcionalmente precisa. | Casos donde interesa la robustez de un núcleo ampliamente probado (Genesis-Plus GX) más que herramientas de debug dedicadas. |
| **JGenesis** | A partir de 2024 alcanzó precisión suficiente para ejecutar las dos demos Overdrive de Titan; según el autor, probablemente el emulador más preciso de la lista en ese momento. | Validar que un efecto exigente a nivel de hardware (rasters, timing fino) se comporta igual que en las demos más exigentes conocidas. |
| **BlastEm** | No destaca especialmente en ninguna faceta que necesite un desarrollador homebrew, pero es un emulador sólido para jugar y aun así le ha pillado algún bug real. | Uso general / como segunda referencia para contrastar comportamiento, además de su depurador de línea de comandos (ver referencia cruzada). |

## Notas / gotchas

- La recomendación no es "usa solo uno": el propio autor combina varios según la tarea (visualización en Gens KMod, depuración Z80 en DGen, verificación de casos límite en Exodus/JGenesis).
- BlastEm aparece aquí solo por su calidad como emulador de referencia; su faceta como *depurador* (comandos de breakpoints, stub remoto de GDB, etc.) ya está documentada con detalle en la entrada de referencia cruzada de abajo — no se repite aquí para evitar duplicar contenido.
- Esta comparativa es una opinión personal recogida en un blog, no una medición formal de precisión de cada núcleo; conviene contrastarla si se necesita justificar una elección de emulador para un proyecto serio.

## Referencias cruzadas

- [Build y debug de ASM puro para Mega Drive (sin SGDK)](build-debug-asm-puro.md) — documenta en detalle los comandos del depurador de BlastEm y las fortalezas de Gens KMod/DGen/Exodus para debug; esta entrada añade a esa comparativa BizHawk y JGenesis, ausentes allí.

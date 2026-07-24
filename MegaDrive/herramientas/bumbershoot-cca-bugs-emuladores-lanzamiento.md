---
titulo: Bugs de compatibilidad entre emuladores detectados al lanzar CCA (Genesis)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [emuladores, kega-fusion, exodus, dma, z80, mando, joypad, compatibilidad, release]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/04/29/release-the-cyclic-cellular-automaton-for-the-sega-genesis-mega-drive/]
---

# Bugs de compatibilidad entre emuladores detectados al lanzar CCA (Genesis)

## Resumen

Al preparar el lanzamiento de CCA para Genesis (rejilla de 128×128 celdas, 16 estados, ventana visible de 80×56, ROM final de 64KB), Michael Martin encontró y corrigió tres bugs que solo se manifestaban al probar en determinado emulador: un cuelgue en Kega Fusion al usar DMA de ROM a VRAM, una desincronización de música reproducible solo en Exodus, y una lectura de mando incorrecta por falta de configuración de puerto. El post no es solo un anuncio de lanzamiento: documenta estas tres correcciones concretas como lección de por qué conviene validar en varios emuladores de distinta fidelidad antes de dar un proyecto por cerrado.

## Detalle técnico

### Bug 1 — Cuelgue en Kega Fusion con DMA ROM→VRAM

Kega Fusion resultó más estricto de lo esperado con las transferencias DMA desde ROM directamente a VRAM: el emulador colgaba la consola por completo, pese a que, según el autor, la operación en cuestión equivalía en la práctica a una copia memoria-a-memoria válida en hardware real. Solución: sustituir esa transferencia DMA por un bucle de copia tradicional (la CPU escribiendo a VRAM directamente en vez de programar el motor DMA del VDP).

### Bug 2 — Desincronización de música, reproducible solo en Exodus

En Exodus (emulador orientado a máxima fidelidad de timing), la rutina de música en Z80 terminaba de ejecutarse tan rápido que la señal de IRQ de frame aún no había completado su ciclo, provocando que la música sonara a aproximadamente el doble de la velocidad prevista. Solución: añadir un bucle de espera breve al final de la rutina de interrupción de frame en el Z80, para garantizar que ya se hubiera avanzado al menos una línea de barrido completa antes de rehabilitar las interrupciones.

### Bug 3 — Lectura de mando incorrecta por falta de configuración de puerto

La rutina de lectura del mando no configuraba correctamente el registro de control del puerto antes de leer los datos. Efecto observado: izquierda y derecha no se podían leer en absoluto, y los botones A y START se leían como si B y C estuvieran también pulsados simultáneamente. Solución: escribir un valor de configuración adicional en el registro de control del puerto antes de ejecutar la rutina de lectura estándar.

### Especificaciones finales del proyecto (contexto)

- Rejilla de 128×128 celdas con 16 estados posibles; ventana visible en pantalla de 80×56 celdas; topología toroidal.
- ROM final de 64KB: aproximadamente 2KB de código (68000 + Z80); el resto se reparte entre imagen de mapa de bits, efectos de sonido digital y música.
- Hardware usado: mando, chip de música YM2612, ambos planos de scroll, un efecto de sonido digital.
- Hardware deliberadamente no usado: sprites, chip de sonido "legacy" (PSG/SN76489 vía Z80).
- Desarrollado en aproximadamente dos meses, con librerías propias escritas a mano y herramientas adaptadas, sin usar un SDK formal (etapa previa a que el autor adoptara herramientas más estandarizadas en proyectos posteriores).

## Notas / gotchas

- Los tres bugs son específicos de emulador, no de hardware real: el patrón que ilustran es que cada emulador implementa con distinto rigor casos límite de DMA, timing de IRQ y protocolo de lectura de mando; solo probar en varios (idealmente incluyendo uno de alta fidelidad como Exodus) los saca a la luz.
- El bug de Kega Fusion es un caso de emulador *más estricto* que el hardware real (según el autor): no siempre que algo falla en un emulador significa que el código era incorrecto en hardware real; conviene verificar cuál de las dos partes tiene razón antes de reescribir código que en realidad era válido.
- Omitir sprites y el chip PSG fue una decisión de alcance del proyecto, no una limitación técnica encontrada durante el desarrollo.

## Referencias cruzadas

- [Comparativa de emuladores para desarrollo en Mega Drive/Genesis](bumbershoot-emuladores-desarrollo.md) — valoración general de Gens KMod/DGen/Exodus/BizHawk/JGenesis/BlastEm; esta entrada aporta bugs concretos con Kega Fusion y Exodus no cubiertos allí.
- [Bumbershoot Software — lectura de mando de 3 botones en ASM puro](../perifericos/bumbershoot-lectura-mando-3-botones.md) — misma rutina de lectura de mando de este proyecto, en una entrada anterior de la serie; no menciona explícitamente el bug de configuración de puerto corregido aquí.
- [Primer diseño e implementación del CCA en Genesis](../cpu/bumbershoot-cca-primera-implementacion.md) y [Optimización del bucle principal del CCA en 68000 (2018)](../cpu/bumbershoot-cca-optimizacion-inicial.md) — desarrollo del algoritmo cuyo pulido final y lanzamiento documenta esta entrada.

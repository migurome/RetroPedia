---
titulo: SGDK — Tutorial de introducción
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, tutorial, arquitectura, rescomp, debug]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/Tuto-Introduction]
---

# SGDK — Tutorial de introducción

## Resumen

Página introductoria de la serie de tutoriales "Start Coding" de SGDK: qué conocimientos previos hacen falta, la filosofía de diseño de la librería (desarrollo orientado a recursos), cómo está organizada la serie de tutoriales, y el enfoque recomendado para depurar.

## Detalle técnico

### Requisitos previos

- Conocer C: "necesitas saber que SGDK usa el lenguaje C (el ensamblador también es posible pero no necesario), así que es muy recomendable estar familiarizado con la programación en C antes de intentar desarrollar".
- Conocer la arquitectura de la Mega Drive, en particular el sistema de vídeo, con lecturas recomendadas de fuentes especializadas como Raster Scroll y el "Genesis Software Manual".

### Filosofía y arquitectura de SGDK

El framework se apoya fuertemente en el concepto de **recursos** (`resources`), compilados mediante la herramienta `rescomp`. Esta herramienta compila varios tipos de recursos; su documentación está en `rescomp.txt` (en el repo) y hay ejemplos en el proyecto de muestra de Sonic.

Un pilar central de la filosofía de SGDK es ofrecer documentación exhaustiva de funciones vía Doxygen, animando a consultar esa referencia para capacidades concretas en vez de depender solo de los tutoriales.

### Organización de la serie de tutoriales

La secuencia sigue esta progresión lógica:

**Instalación y básicos → Integración con IDE → Empezar a programar**

Los tutoriales de código avanzan desde lo fundamental ("Hello World") pasando por los sistemas visuales (backgrounds, sprites) hasta la interacción con el usuario y las operaciones matemáticas — un enfoque escalonado que permite ir subiendo la complejidad de forma incremental.

### Enfoque de depuración

SGDK reconoce las limitaciones de GDB sobre emuladores y en su lugar recomienda el sistema de logging de mensajes de Gens KMod mediante los métodos `KLog`, animando a validar el comportamiento en emuladores precisos como BlastEm o en hardware real.

## Notas / gotchas

- Esta página es puramente introductoria/orientativa — no contiene código. El código empieza en [Tutorial: Hello World](tuto-hello-world.md).

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Hello World](tuto-hello-world.md)

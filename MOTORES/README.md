---
titulo: MOTORES — arquitectura y diseño de motores de juego
plataforma: [General]
categoria: [general]
tags: [motores, arquitectura, diseno, multiplataforma]
estado: borrador
fecha: 2026-07-21
---

# MOTORES — arquitectura y diseño de motores de juego

## Propósito

A diferencia de `MegaDrive/` y `SNES/` (documentación específica de una plataforma), esta carpeta recoge **conocimiento de arquitectura de motores de juego que no depende de una consola concreta**: cómo se estructuran, qué decisiones de diseño toman, y cómo se adaptan a hardware con recursos muy distintos. Nace como referencia explícita para las fases de diseño de una **segunda versión de motor-cenital** (ver memoria del proyecto) y de futuros motores, no como documentación de un hardware concreto.

Primera fuente incorporada: [fabiensanglard.net](https://fabiensanglard.net/), blog de Fabien Sanglard (autor de los "Game Engine Black Books" sobre Wolfenstein 3D y DOOM) — análisis técnico en profundidad de motores clásicos y sus ports multiplataforma.

## Estructura

- `arquitectura/` — diseño interno de motores clásicos (bucle principal, timing, capas de abstracción motor/plataforma), incluida la serie "Another World" que analiza el MISMO motor de polígonos 2D portado a 7 plataformas distintas (Amiga, Atari ST, PC-DOS, **Sega Genesis**, **SNES**, GBA, Jaguar) — el caso de estudio más directamente comparable a la separación `ENG_*`/SGDK de motor-cenital.
- `efectos/` — algoritmos de efectos visuales concretos y muy citados (fuego de DOOM, FizzleFade).
- `graficos-hardware/` — sistemas de vídeo/sonido de hardware retro comparable (PPU de SNES, sistema gráfico y de sonido de placas arcade CPS-1, técnicas de sprite de Street Fighter II) — útil para comparar decisiones de diseño frente al VDP de Mega Drive.

## Cómo usar esto en el diseño de un motor

Cada entrada documenta una decisión de arquitectura y su motivación (por qué esa plataforma/motor lo resolvió así, qué restricciones de hardware lo forzaron). Al diseñar motor-cenital v2, la pregunta útil no es "¿qué hizo Doom/Another World?" sino "¿qué restricción tenían que nosotros también tenemos (o no), y por qué eligieron esa solución?" — comparar restricciones antes de copiar soluciones.

## Entradas

Ver [INDEX.md](../INDEX.md) para el listado completo enlazado por categoría.

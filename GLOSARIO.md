---
titulo: Glosario transversal
plataforma: [General]
categoria: [glosario]
tags: []
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com, proyectos SSB Port y hyperdbz-dreamcast-port]
---

# Glosario transversal

Conceptos que aparecen en varias plataformas retro. Cuando un concepto tenga particularidades importantes en una consola concreta, se documenta aquí en general y se enlaza el detalle específico desde la carpeta de esa plataforma.

## Reglas de proceso

- [Reglas comunes a todo proyecto](GLOSARIO/reglas-comunes-de-proyecto.md) — RetroPedia como consulta, herramientas en `Desktop\Tools`, ROADMAP, diario de builds, nombre de build visible en el juego, savestate como log, QR en hardware real, marcador con Start, regresión, `publicar.py`, pantalla de cuelgue con QR e ideas pendientes
- [Depuración avanzada dentro del juego](GLOSARIO/depuracion-avanzada-en-el-juego.md) — flight recorder, invariantes, cápsula de fallo, replay diferencial y demás técnicas para que el juego genere sus propias pruebas en hardware real

## Fundamentos de programación de juegos (carpeta `GLOSARIO/`)

Extraídos de [plutiedev.com](https://plutiedev.com) — no son específicos de Mega Drive, aplican a cualquier plataforma retro con CPU sin FPU:

- [Números en punto fijo (Fixed Point)](GLOSARIO/fixed-point.md)
- [Matemáticas de ángulos (Angle Math)](GLOSARIO/angle-math.md)
- [Velocidad y gravedad en plataformas (Speed and Gravity)](GLOSARIO/speed-and-gravity.md)
- [Detección de colisiones básica (Basic Collision)](GLOSARIO/basic-collision.md)
- [Ordenación de sprites (Sorting Sprites)](GLOSARIO/sorting-sprites.md)
- [Sistemas numéricos — decimal, binario y hexadecimal](GLOSARIO/dec-bin-hex.md)

Sacados de proyectos propios:

- [Colisión con máscaras de píxel — trapecios y rampas de 1 píxel](GLOSARIO/mask-collision-slopes.md)

## Términos de hardware pendientes de documentar

Términos candidatos, comunes a varias consolas, todavía sin entrada propia (se documentarán con detalle real cuando aparezcan en un proyecto concreto, comparando su implementación entre plataformas):

- DMA (Direct Memory Access)
- VRAM / paletas de color
- Tile-based rendering / tilemaps
- Sprites y limitaciones por línea
- Bank switching / mapeo de memoria
- Interrupciones (NMI/IRQ) y VBlank

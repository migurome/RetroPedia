---
titulo: Snippets — código mínimo que compila, con su toolchain
plataforma: [MegaDrive, Dreamcast]
categoria: [herramientas]
tags: [sprites, mando, sgdk, kos, toolchain, emuladores]
estado: revisado
comprobado: emulador
comprobado_en: "parcial: BlastEm 1.0.0 y Flycast 2.7, solo arranque y dibujo; la cruceta no se ha probado"
fecha: 2026-11-06
fuentes: [SGDK 2.11 (inc/maths.h, inc/sprite_eng.h), KOS de DreamSDK R4 (ejemplos pvr, utils/makeip), proyecto hyperdbz-dreamcast-port (tools/make_ip.py, tools/make_disc.sh, tools/kos.sh)]
---

# Snippets

## Resumen

Programas completos y pequeños, compilados de verdad. El código de las entradas de RetroPedia sirve para explicar, pero
no consta que compile; esto sí. El robot de los ejemplos es un dibujo propio.

## Detalle técnico

| Snippet | Qué hace | Toolchain | Compila | Se ha visto en |
|---|---|---|---|---|
| [`megadrive-sgdk/sprite-mando`](megadrive-sgdk/sprite-mando/src/main.c) | Sprite de `rescomp` movido con la cruceta, en punto fijo | SGDK 2.11, GCC 13.2 | Sí | BlastEm 1.0.0: texto y sprite |
| [`dreamcast-kos/sprite-mando`](dreamcast-kos/sprite-mando/main.c) | PNG a textura ARGB4444 y sprite con transparencia, en un CDI | DreamSDK R4, GCC 13.2 | Sí | Flycast 2.7: sprite; el fondo sale verde ([síntoma](../Dreamcast/SINTOMAS.md)) |

Cada uno se construye con `sh construir.sh` desde el bash de Git, y deja el resultado en `out/`, que no se versiona.

- La receta del de Dreamcast, paso a paso: [del PNG al sprite en pantalla](../Dreamcast/herramientas/receta-png-a-sprite.md).
- Falta un snippet de Neo Geo: el toolchain está instalado ([inventario](../GLOSARIO/herramientas-compartidas.md)). De SNES
  no hay toolchain instalado.

## Notas / gotchas

- **Lo que enseñó compilar:** en SGDK 2.11, `fix16ToInt` da error por obsoleta (`F16_toInt`); `F16()` y el tipo `f16` son
  los nombres actuales.
- **Al añadir un snippet:** una carpeta por plataforma, un `construir.sh` y una fila en esta tabla con lo que de verdad se
  ha comprobado.

## Referencias cruzadas

- [Del PNG al sprite en Dreamcast](../Dreamcast/herramientas/receta-png-a-sprite.md)
- [Sprites de SGDK](../MegaDrive/herramientas/sgdk/tuto-sprites.md)
- [Herramientas compartidas](../GLOSARIO/herramientas-compartidas.md)

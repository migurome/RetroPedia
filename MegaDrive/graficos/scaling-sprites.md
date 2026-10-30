---
titulo: Escalado de sprites (técnicas para simularlo)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [sprites, efecto, escalado, tecnica-visual]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/scaling-sprites]
---

# Escalado de sprites (técnicas para simularlo)

## Resumen

La Mega Drive no puede escalar sprites por hardware, y la CPU es demasiado lenta para hacerlo por software a buen framerate. Esta entrada recoge las técnicas conocidas para fingir que un sprite crece o encoge mientras se mantiene la velocidad completa, incluyendo el enfoque combinado usado en el juego _Miniplanets_.

## Detalle técnico

### Enfoques existentes

El enfoque más común era tener múltiples versiones del sprite a distintos tamaños, y luego elegir la más cercana a lo que se necesita mostrar en ese momento. Es el enfoque más simple, pero hay un límite a cuántos pasos caben en memoria, y a menudo los saltos entre pasos son demasiado obvios y quedan feos.

Un enfoque más interesante es el que usa _Yū Yū Hakusho Makyō Tōitsusen_: los sprites se dividen en "slices" (cortes) más pequeños, que luego se acercan entre sí para dar la impresión de que el sprite se encoge. Es suave, pero solo se ve bien cuando el tamaño del sprite no cambia mucho.

### Técnica combinada (Miniplanets)

_Miniplanets_ lleva los enfoques anteriores a su conclusión lógica en las tarjetas de título de sus escenarios (de ahí se sacó el sprite de ejemplo del artículo original, "Qisha"). Como en el primer enfoque, tiene múltiples versiones del sprite para los pasos gruesos, pero luego usa el segundo enfoque para suavizar la transición entre ellas. Esto permite escalar a prácticamente cualquier tamaño mientras se mantiene fluido:

1. Se parte del sprite más grande y se encoge acercando sus "slices" entre sí.
2. Una vez que el sprite es lo bastante pequeño, se cambia a una versión pre-renderizada más pequeña del mismo sprite.
3. El proceso se repite, permitiendo que la técnica cubra un rango de tamaños mucho mayor que cualquiera de los dos enfoques por separado.

## Notas / gotchas

- El enfoque de "múltiples versiones pre-renderizadas" tiene un límite de memoria claro, y si los pasos de tamaño están muy separados los saltos visuales resultan obvios y feos.
- El enfoque de "slices" (usado en Yū Yū Hakusho) solo se ve bien cuando el cambio de tamaño no es muy grande; no sirve por sí solo para rangos de escalado amplios.
- La página fuente no incluye código en C ni en ensamblador 68000, ni tablas de datos ni fórmulas matemáticas: es una explicación puramente conceptual acompañada de capturas y GIFs animados.

## Referencias cruzadas

- [../herramientas/md-engine/camara-y-scroll.md](../herramientas/md-engine/camara-y-scroll.md): versiones pre-escaladas y cambio de fondo por niveles en MD Engine (DB Fighter)

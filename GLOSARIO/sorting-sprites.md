---
titulo: Ordenación de sprites (Sorting Sprites)
plataforma: [General]
categoria: [fundamentos]
tags: [sprites, orden-de-dibujado, sorting, rendimiento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/sorting-sprites]
---

# Ordenación de sprites (Sorting Sprites)

## Resumen

Esta entrada describe tres enfoques habituales para gestionar el orden de dibujado (profundidad/prioridad) de los sprites en un juego, cada uno con sus propios costes y beneficios según el tipo de juego.

## Detalle técnico

A continuación, los tres enfoques principales para gestionar el orden de dibujado de sprites:

### Separar por categoría

En lugar de ordenar todos los sprites juntos, se organizan en listas separadas por tipo (jugadores, enemigos, objetos, proyectiles). Se procesan las listas en el orden deseado. El coste implica mantener múltiples colecciones, aunque este enfoque ofrece beneficios en la detección de colisiones, al permitir comprobar sólo los tipos de objeto relevantes.

### Mantener los objetos ordenados

Esta técnica es adecuada para juegos con percepción de profundidad, como los isométricos o los beat'em-up. El método consiste en "keep the objects themselves sorted by depth and sort again on every frame" (mantener los propios objetos ordenados por profundidad y volver a ordenar cada fotograma). El autor señala que usar insertion sort (ordenación por inserción) resulta eficiente porque "objects rarely swap places across frames" (los objetos rara vez intercambian posiciones entre fotogramas), lo que lo hace casi óptimo para datos que ya están casi ordenados. Al añadir nuevos sprites, basta con añadirlos al final de la lista y dejar que el algoritmo de ordenación los coloque en su sitio. El autor confirma que "The occassional outlier isn't much of a performance hit compared to the overall savings" (el ocasional caso atípico no supone un gran coste de rendimiento comparado con el ahorro general).

### Ordenar al insertar

Este enfoque funciona cuando cada sprite individual requiere un valor de profundidad independiente. En lugar de ordenar después de insertar, se "sort them the moment each sprite is inserted" (ordenan en el momento en que se inserta cada sprite). El algoritmo recomendado:

1. Reservar un slot vacío al final de la lista.
2. Si no es el primer slot y el sprite anterior está más profundo, desplazar ese sprite hacia adelante y repetir.
3. En caso contrario, colocar el nuevo sprite y terminar.

Alternativamente, se puede usar búsqueda binaria para localizar el punto de inserción. Esto deja los sprites ordenados de delante hacia atrás para renderizarlos secuencialmente.

## Notas / gotchas

- El insertion sort es adecuado precisamente porque los sprites rara vez cambian de orden entre fotogramas — no es una elección de ordenación arbitraria, depende de que los datos estén casi ordenados de antemano.
- Separar por categoría añade complejidad de mantenimiento (varias listas) pero puede simplificar/acelerar la detección de colisiones.
- El enfoque de "ordenar al insertar" es más adecuado cuando cada sprite tiene su propio valor de profundidad independiente, no ligado a categorías.

## Referencias cruzadas

- (pendiente de enlazar)

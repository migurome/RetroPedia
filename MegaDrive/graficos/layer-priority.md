---
titulo: Prioridad de capas (layer priority)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, capas, prioridad, sprites, plano-a, plano-b]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/layer-priority]
---

# Prioridad de capas (layer priority)

## Resumen

Explica el orden de renderizado de las capas de vídeo de la Mega Drive (sprites, plano A, plano B, fondo) y cómo el bit de prioridad de cada tile/sprite reordena ese apilamiento, permitiendo por ejemplo que un sprite se oculte detrás del primer plano.

## Detalle técnico

### Estructura de capas

El documento identifica cuatro capas fundamentales del hardware de vídeo:

1. **Sprite layer** (toda capa de sprites)
2. **Plane A layer** (típicamente foreground)
3. **Plane B layer** (típicamente parallax)
4. **Background layer** (color de fondo)

El comportamiento predeterminado muestra: "sprites on top of plane A, on top of plane B, on top of background" (sprites encima del plano A, encima del plano B, encima del fondo).

### Sistema de prioridad con flag de prioridad

Cada sprite y tile en los planos A y B posee un "priority flag" que modifica el orden de renderizado. El documento especifica el orden resultante cuando se aplica este flag:

**Orden de renderizado completo:**

1. Sprite layer (high priority)
2. Plane A layer (high priority)
3. Plane B layer (high priority)
4. Sprite layer (low priority)
5. Plane A layer (low priority)
6. Plane B layer (low priority)
7. Background layer

Esta estructura permite que "a sprite can hide behind the foreground" (un sprite puede ocultarse detrás del primer plano) manipulando el flag de prioridad.

### Ordenamiento entre sprites

Los sprites se ordenan según su posición en la tabla de sprites (sprite table), no por el flag de prioridad. "Sprites earlier in the list show up on top of sprites later in the list" (los sprites anteriores en la lista aparecen encima de los sprites posteriores) determina la jerarquía visual dentro de la capa de sprites.

## Notas / gotchas

- **Window plane:** comparte la misma capa que Plane A en términos de ordenamiento.
- **Efecto de silueta:** "A low priority sprite layer _can_ end up on top of a high priority one" (una capa de sprite de baja prioridad puede terminar encima de una de alta prioridad) cuando intersecta con plano A/B de alta prioridad, permitiendo efectos especiales si se explota adecuadamente.
- **Recomendación para HUD:** los sprites de interfaz deben posicionarse al inicio de la tabla de sprites y marcarse como alta prioridad para asegurar visibilidad completa.

## Referencias cruzadas

- (pendiente de enlazar)

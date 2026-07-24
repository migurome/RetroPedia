---
titulo: Detección de colisiones básica (Basic Collision)
plataforma: [General]
categoria: [fundamentos]
tags: [colisiones, matematicas, punto-fijo, circulos, cajas]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/basic-collision]
---

# Detección de colisiones básica (Basic Collision)

## Resumen

Esta entrada cubre los métodos básicos de detección de colisiones para distintas combinaciones de formas (puntos, cajas, círculos) en 2D y 3D, un tema habitual en el desarrollo de videojuegos retro.

## Detalle técnico

### Consideraciones sobre punto fijo

El autor recomienda usar la parte entera de los valores en punto fijo para las comprobaciones de colisión, señalando que bastan números de 16 bits en lugar de valores de 32 bits. Cuando se usa "1px = 65536", la primera palabra (word) contiene la mitad entera, legible directamente sin necesidad de desplazamiento de bits.

### Métodos de colisión en 2D

**Punto vs. Caja**

Usa comprobación de rango en ambos ejes contra los límites de la caja:

```
if (point_x >= box_x1) and
   (point_x <= box_x2) and
   (point_y >= box_y1) and
   (point_y <= box_y2)
then
   ...
end
```

**Caja vs. Caja**

Comprueba si dos rangos se solapan en ambos ejes:

```
if (box1_x1 <= box2_x2) and
   (box1_x2 >= box2_x1) and
   (box1_y1 <= box2_y2) and
   (box1_y2 >= box2_y1)
then
   ...
end
```

**Punto vs. Círculo**

Usa el teorema de Pitágoras sin calcular la raíz cuadrada:

```
delta_x = circle_x - point_x
delta_y = circle_y - point_y
limit = circle_radius

if (delta_x * delta_x) +
   (delta_y * delta_y) <= (limit * limit)
then
   ...
end
```

**Círculo vs. Círculo**

Compara la suma de los radios contra la distancia entre centros:

```
delta_x = circle2_x - circle1_x
delta_y = circle2_y - circle1_y
limit = circle2_radius + circle1_radius

if (delta_x * delta_x) +
   (delta_y * delta_y) <= (limit * limit)
then
   ...
end
```

### Métodos de colisión en 3D

Todas las variantes 3D extienden sus equivalentes en 2D añadiendo comprobaciones en el eje Z. Punto vs. Esfera, Esfera vs. Esfera, Punto vs. Cilindro, y Cilindro vs. Cilindro siguen los mismos patrones con cálculos de coordenadas adicionales.

### Estrategia para formas complejas

Para objetos de forma irregular, la guía sugiere usar múltiples comprobaciones de colisión más simples — por ejemplo, tratar un objeto en forma de L como dos cajas.

**Nota importante:** el autor enfatiza el pragmatismo: aproximar las formas del jugador o mezclar tipos de colisión entre distintos objetos es aceptable por motivos de rendimiento.

## Notas / gotchas

- Para las comprobaciones de colisión basta usar la parte entera (16 bits) de un valor en punto fijo de 32 bits; no hace falta el valor completo.
- Las comprobaciones de círculo evitan deliberadamente la raíz cuadrada comparando los cuadrados de las distancias contra el cuadrado del límite.
- Es aceptable simplificar la forma de colisión de un objeto (por ejemplo, aproximar con cajas) por rendimiento, aunque no sea geométricamente exacto.

## Referencias cruzadas

- (pendiente de enlazar)

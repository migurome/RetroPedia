---
titulo: Matemáticas de ángulos (Angle Math)
plataforma: [General]
categoria: [fundamentos]
tags: [matematicas, angulos, seno, coseno, trigonometria, tablas]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/angle-math]
---

# Matemáticas de ángulos (Angle Math)

## Resumen

Esta entrada explica cómo trabajar con ángulos, seno y coseno en un contexto de desarrollo de videojuegos retro sin coma flotante, usando tablas precalculadas y unidades de ángulo en potencias de dos para optimizar el rendimiento en el 68000.

## Detalle técnico

### Conceptos clave

La guía explica que "sin(angle) is how much you move up" (sin(ángulo) es cuánto te mueves hacia arriba) y "cos(angle) is how much you move right" (cos(ángulo) es cuánto te mueves hacia la derecha), con la fórmula práctica:

```
x = cos(angle)
y = -sin(angle)
```

(teniendo en cuenta el eje Y invertido en los juegos).

### Unidades de ángulo

El autor recomienda usar "a power of two as a full circle" (una potencia de dos como círculo completo) — 256, 1024 o 65536 — en lugar de grados o radianes. Esto permite la normalización mediante operaciones AND a nivel de bit, lo cual es computacionalmente eficiente en procesadores 68000.

La fórmula de conversión proporcionada es: "angle ÷ circle in old unit × circle in new unit" (ángulo ÷ círculo en la unidad antigua × círculo en la unidad nueva).

### Implementación de seno/coseno

Dado que el 68000 carece de capacidad de coma flotante, las tablas de consulta (lookup tables) son esenciales. La guía muestra el patrón de generación:

```
for angle = 0 to MAX_ANGLE
  table[angle] = sin(angle) * premultiply
end
```

Los valores se "premultiplican" (comúnmente por 256) para mantener precisión en punto fijo. Los cosenos reutilizan las tablas de seno mediante la relación: "cos(angle) is the same thing as sin(angle+90°)" (cos(ángulo) es lo mismo que sin(ángulo+90°)).

### Movimiento en ángulos

La fórmula combina la distancia con los valores trigonométricos:

```
new_x = x + (cos(angle) * distance)
new_y = y - (sin(angle) * distance)
```

Con tablas premultiplicadas, los desplazamientos de bits reemplazan la división: "new_x = x + (cos(angle) >> 4)" al avanzar 16 píxeles con premultiplicación de 256x.

### Tabla de rotación de 90°

La guía proporciona transformaciones para giros de un cuarto de vuelta sin tener que recalcular los valores:

| Rotación | X | Y |
|----------|---|---|
| 0° | cos(angle) | -sin(angle) |
| 90° | -sin(angle) | -cos(angle) |
| 180° | -cos(angle) | sin(angle) |
| 270° | sin(angle) | cos(angle) |

### Rotación de puntos

Rotar alrededor de un pivote usa un movimiento compuesto:

```
new_x = origin_x + (cos(angle)*distance_x) + (sin(angle)*distance_y)
new_y = origin_y - (sin(angle)*distance_x) + (cos(angle)*distance_y)
```

### Tablas de consulta de ángulos

Para calcular ángulos entre puntos, el enfoque usa rangos de consulta acotados (32-64 unidades) con división iterativa para distancias mayores: "divide them by 2 until both fit" (divídelos entre 2 hasta que ambos quepan).

### Interpolación de ángulos

Para un giro suave hacia un objetivo con unidades de ángulo en potencia de dos:

```
temp = (target_angle - angle) & ANGLE_MASK

if temp < 180°
   increment angle
else
   decrement angle
end
```

El autor señala que este método falla con unidades que no son potencia de dos, debido a los requisitos de normalización más complejos.

## Notas / gotchas

- Usar grados o radianes complica la normalización de ángulos; una potencia de dos como círculo completo permite usar AND a nivel de bit para "envolver" el ángulo.
- El método de interpolación de ángulos mostrado (con `ANGLE_MASK`) sólo funciona correctamente si la unidad de ángulo es una potencia de dos.
- Las tablas de seno/coseno deben premultiplicarse para conservar precisión al trabajar sin coma flotante.

## Referencias cruzadas

- (pendiente de enlazar)

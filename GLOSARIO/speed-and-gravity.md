---
titulo: Velocidad y gravedad en plataformas (Speed and Gravity)
plataforma: [General]
categoria: [fundamentos]
tags: [fisica, plataformas, gravedad, velocidad, subpixels, sonic]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/speed-and-gravity]
---

# Velocidad y gravedad en plataformas (Speed and Gravity)

## Resumen

Esta entrada cubre la física básica de un juego de plataformas retro: cómo representar posición y velocidad con subpíxeles en punto fijo, y cómo implementar aceleración, deceleración, salto y gravedad, incluyendo constantes de referencia usadas en juegos clásicos (estilo genérico y estilo Sonic).

## Detalle técnico

### Conceptos centrales

**Subpíxeles y punto fijo:**
La guía enfatiza que "using pixels for position is a bad idea" (usar píxeles para la posición es mala idea) porque la velocidad mínima se convertiría en 60px/segundo. En su lugar, hay que usar subpíxeles con aritmética de punto fijo, escalando 1 píxel a 65536 (una potencia de dos) para permitir movimiento fraccionario como 0.75px por fotograma (almacenado como 49152).

**Fórmula de actualización de posición:**
```
x = x + x_speed
y = y + y_speed
```

### Mecánica de carrera (running)

**Lógica de aceleración:**
```
if acceleration == 0
   do nothing

else if acceleration > 0 and x_speed < limit
   x_speed = x_speed + acceleration
   if x_speed > limit then x_speed = limit

else if acceleration < 0 and x_speed > -limit
   x_speed = x_speed + acceleration
   if x_speed < -limit then x_speed = -limit
end
```

**Deceleración:**
```
if x_speed == 0
   do nothing

else if x_speed > 0
   x_speed = x_speed - deceleration
   if x_speed < 0 then x_speed = 0

else if x_speed < 0
   x_speed = x_speed - deceleration
   if x_speed > 0 then x_speed = 0
end
```

La colisión con una pared pone la velocidad horizontal a 0.

### Salto y gravedad

**Aplicación de gravedad:**
```
y_speed = y_speed + gravity
```

**Disparo del salto:**
```
if jump_button and on_floor
   y_speed = -jump_speed
   play_sfx jump
end
```

**Fricción en el aire (aceleración/deceleración reducidas a la mitad):**
```
if not on_floor
   acceleration = acceleration >> 1
   deceleration = deceleration >> 1
end
```

**Altura de salto variable:**
```
y_speed = y_speed + gravity
if y_speed < 0 and (not jump_button)
   y_speed = y_speed + extra_gravity
end
```

### Constantes recomendadas

**Plataformas estándar:**
- Velocidad máxima: 2.5px/fotograma
- Aceleración: 0.25px/fotograma
- Velocidad de salto: -7px/fotograma
- Gravedad: 0.25px/fotograma
- Gravedad extra: 0.25px/fotograma

**Estilo Sonic (basado en momentum):**
- Velocidad máxima: 5px/fotograma
- Aceleración: 0.125px/fotograma

## Notas / gotchas

- Usar píxeles enteros para la posición limita la velocidad mínima representable a 60px/segundo; hay que usar subpíxeles.
- La fricción en el aire se implementa reduciendo a la mitad (`>> 1`) la aceleración y deceleración cuando el personaje no está en el suelo.
- La altura de salto variable depende de comprobar si el botón de salto sigue pulsado mientras `y_speed < 0`.

## Referencias cruzadas

- (pendiente de enlazar)

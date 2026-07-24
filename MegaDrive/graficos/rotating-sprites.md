---
titulo: Rotación de sprites (técnicas para simularla)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [sprites, efecto, rotacion, tecnica-visual, sonic]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/rotating-sprites]
---

# Rotación de sprites (técnicas para simularla)

## Resumen

Al igual que con el escalado, la Mega Drive no puede rotar sprites por hardware y la CPU tampoco da para hacerlo en tiempo real. Esta entrada recoge las formas de fingir una rotación "suficientemente buena" para sprites, incluyendo el truco de descomponer el sprite en piezas independientes (usado en jefes de Alien Soldier) y el ejemplo de las special stages de Sonic 1.

## Detalle técnico

### Rotación pre-renderizada

Como ocurría con el escalado, el enfoque más común es tener múltiples versiones del mismo sprite en distintos ángulos. De nuevo, esto puede consumir mucha memoria, y no se ve suave cuando la rotación no va lo bastante rápido. En el ejemplo del artículo (una rueda dentada girando en cuatro frames), aunque cada frame solo gira unos 11 grados, sigue sin ser suficiente para que la rotación parezca fluida.

Esto no supone mucho problema para algo que gira todo el rato (se ve como una animación dibujada a mano agradable), pero puede ser un problema grande para cosas que intentan apuntar hacia un objetivo o similar, donde hay bastante probabilidad de quedarse "atascado" entre pasos.

### Suavizando la rotación

Algo mencionado también en la página de escalado es dividir un sprite en piezas más pequeñas, y esto vuelve a ser relevante para la rotación. El truco consiste en dividir un sprite en piezas más pequeñas que se puedan mover de forma individual, teniendo variantes rotadas para cada pieza en vez de para el sprite completo (de forma similar a los jefes de _Alien Soldier_).

En el ejemplo de la rueda dentada: hay un sprite grande que forma la rueda en sí, y luego sprites individuales para cada diente a distintos ángulos (la caja roja es la rueda y las cajas blancas son los dientes). Esto permite una rotación percibida mucho más suave, ya que aunque las piezas individuales sigan sufriendo la falta de frames, al estar en movimiento el cerebro percibe el objeto completo como si rotara, y hay que estar mirando explícitamente para notar los fallos.

### Otros ejemplos

Las special stages de Sonic 1 son esencialmente este efecto aplicado a _todo el primer plano_ (el GIF de referencia del artículo original usaba códigos Game Genie para quitar la rigidez artificial que tiene la rotación de las special stages). Cada bloque es un sprite, y todos se mueven en conjunto para crear el efecto de rotación.

## Notas / gotchas

- El enfoque de múltiples ángulos pre-renderizados consume mucha memoria y puede quedarse "atascado" visualmente entre pasos si el objeto no gira lo bastante rápido (problemático para apuntado hacia un objetivo).
- Descomponer el sprite en piezas menores con rotaciones individuales mejora mucho la rotación percibida gracias al movimiento, aunque cada pieza siga teniendo pocos frames de rotación.
- La página fuente no incluye código en C ni en ensamblador 68000, ni tablas de datos matemáticas: es una explicación conceptual con GIFs de ejemplo.

## Referencias cruzadas

- (pendiente de enlazar)

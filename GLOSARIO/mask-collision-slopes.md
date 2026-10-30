---
titulo: Colisión con máscaras de píxel en consolas — trapecios y rampas de 1 píxel
plataforma: [General]
categoria: [fundamentos]
tags: [colisiones, mascaras, rampas, slopes, game-maker, port, rendimiento, 68000]
estado: revisado
fecha: 2026-10-22
fuentes: [port de Super Smash Land (Game Maker 8) a Mega Drive, B011-B016: tools/stages.py y core/ssb_core.c; scripts/movement_scr.gml del original]
---

# Colisión con máscaras de píxel: trapecios y rampas de 1 píxel

## Resumen

Los motores de PC (Game Maker, Construct…) chocan contra **máscaras de píxel**: cualquier forma vale, también rampas y bordes redondeados. En una consola hay que pasarlas a algo que la CPU recorra deprisa. Hay dos errores habituales, y el port de Super Smash Land a Mega Drive los cometió los dos:
- **Una caja por fila** de píxeles: es exacto, pero lento.
- **Simplificar la forma:** es rápido, pero cambia cómo se juega.

La solución que funcionó es un **trapecio por tramo**: una pieza con los bordes izquierdo y derecho inclinados, que reproduce la máscara píxel a píxel. Junto con ella hay que portar también la **lógica de rampas** del motor original, que es la que sube y baja escalones de 1 píxel.

## Detalle técnico

### Por qué una caja por fila es lenta

Un motor de plataformas «estilo Jenner» (el de muchos juegos de Game Maker) mueve al personaje **de píxel en píxel**: `repeat(abs(hsp)) { if !place_meeting(x+sign, y, par_block) x += sign }`. En cada paso consulta todos los sólidos.
- Una rampa de 48 filas pasada a cajas de 1 píxel de alto da 48 cajas.
- En Air Ride salían 99 cajas. Por 2 jugadores, ~10 pasos por eje y varias consultas por paso, son miles de comparaciones por fotograma.
- Resultado: el escenario iba un 36 % más lento en un 68000 a 7,6 MHz.

### Por qué no vale simplificar

Agrupar las filas de 4 en 4 (1 píxel de pantalla si el original va a 4x) dejó Air Ride en 27 cajas y quitó la ralentización. Pero convirtió los escalones de 1 píxel en escalones de 4. El motor original solo sube escalones de 1 píxel, así que los personajes **se quedaban trabados** en las rampas (el edificio de Saffron). Fallo reportado por el usuario.

### Trapecios exactos

Cada sólido es `{y, h, l, r, dl, dr}`: filas `y .. y+h-1`, bordes inclusivos `l`/`r` en la primera fila y pendientes `dl`/`dr` en 8.8 por fila:

```c
static s16 solid_edge(s16 e, s16 d, s16 k) { return e + (s16)(((s32)d * k + 128) >> 8); }
```

**Generación (herramienta, en Python):**
1. Unir las máscaras de todos los bloques de la sala fila a fila (tramos `[x0, x1)`).
2. Recorrer las filas de arriba abajo. Un tramo amplía el trapecio abierto que toca si existe una pendiente `d` que da **exactamente** cada borde.
3. La pendiente posible es un intervalo que se va estrechando. Para cada fila `k`:
   ```
   d*k >= (e[k]-e[0])*256 - 128     y     d*k < (e[k]-e[0]+1)*256 - 128
   ```
   Si el intervalo se vacía, empieza otro trapecio.
4. Comprobar que, al rasterizar los trapecios, sale la misma máscara. En SSB salió idéntica en los 10 escenarios, con 3-15 piezas cada uno (Air Ride bajó de 99 a 6).

**Prueba de caja contra trapecio sin recorrer filas.** Los dos bordes son monótonos en `k`, así que «borde izquierdo ≤ x2» y «borde derecho ≥ x1» se cumplen cada uno en un tramo de filas que toca un extremo.
- Si los dos valen en el mismo extremo, hay choque.
- Si valen en extremos opuestos, una búsqueda binaria encuentra la frontera del primero y se prueba el segundo allí.
- Los rectángulos (`dl == dr == 0`) van por el camino rápido.

### La lógica de rampas (movement_scr de Game Maker)

En el paso horizontal, además de «¿hay hueco delante?»:

```
si delante está libre:
    si había suelo al acabar el paso anterior (freemd), delante-abajo no hay suelo ni plataforma:
        bajar 1 píxel, avanzar 1.5*signo + hsp/15, hsp aumenta 0.08 en su sentido, pisar si hace falta
    si no se ha tocado ninguna rampa en este paso: x += signo
si no:
    si delante-arriba está libre: subir 1 píxel, avanzar 1.5*signo + hsp/21, hsp pierde 0.1
    si no se ha tocado ninguna rampa: chocar (hsp = 0, o rebotar si está herido)
```

- La variable `trin` recuerda si en ese fotograma ya se tomó una rampa. Desde entonces, en el resto de los pasos solo se avanza por las rampas.
- El avance de `1.5 + hsp/15` deja `x` con decimales, que hay que guardar (un acumulador 8.8).
- Al salir andando de un borde también se aplica la bajada: el personaje se hunde un poco antes de caer, como en el original.

### Que el paso a paso no se coma el fotograma

Aunque haya pocos sólidos, mover de píxel en píxel sale caro en un 68000. Caer a 12-20 píxeles por fotograma son 20-40 consultas, y cada una cuesta cientos de ciclos.

En SSB la actualización de los dos jugadores llegaba a **235 de las 262 líneas** de un fotograma NTSC. Tres arreglos lo dejaron en un pico de 79 sin cambiar el comportamiento:

1. **Caja envolvente por sólido** (`x0`/`x1`, las columnas extremas de todas sus filas). La consulta descarta con 4 comparaciones los sólidos lejanos antes de la prueba fina de trapecio.
2. **Barrido.** Si la caja que recorre el personaje en todo el movimiento del fotograma (de `y+1` a `y+n`) no toca ningún sólido ni plataforma, ningún paso intermedio podía chocar: se avanza `n` de una vez. Si toca algo, se hace paso a paso como el original. En horizontal solo se aplica en el aire, porque en el suelo cada paso puede tomar una rampa.
3. **No recorrer lo que no existe.** Las plataformas temporales (Rush) se buscaban en los 10 huecos de proyectil en cada consulta; un indicador «hay alguna» lo evita.

Cómo se midió: el núcleo lee `VDP_getAdjustedVCounter()` al principio y al final de cada parte y guarda las líneas en una estructura que leen las pruebas automáticas. Una instrumentación temporal por tramos dentro de la función sospechosa señaló el culpable.

## Notas / gotchas

- **Máscara precisa o caja.** En Game Maker cada sprite puede chocar por píxel o por su caja (`bbox`). El exportador usado no guardaba ese ajuste, así que se supuso píxel. Si se porta otro juego, conviene comprobarlo, porque cambia rampas y bordes.
- **Probar la rampa con una prueba automática:** el personaje anda N fotogramas hacia la rampa y se comprueba que `x` avanza y que `y` sube. Y en una plataforma plana, que `x` no retrocede nunca con la dirección pulsada (los tirones se ven así).
- **Medir antes y después.** Un contador de fotogramas de juego comparado con los del emulador da el porcentaje perdido. Así se vio que el 36 % de Air Ride era la colisión. **Cuidado con muestrear el PC al final del fotograma:** en Clock Town apuntaba a `SPR_update` y se concluyó que no era la colisión, pero lo era. El PC solo dice dónde acabó el fotograma que se pasó, no qué lo alargó. Medir el tiempo de cada parte (líneas del VDP) dio la respuesta correcta: el movimiento paso a paso.
- Los proyectiles del original tienen su propia copia de la lógica de rampas (`ball_movement`, `bomb_movement`…). Hay que portarla también si ruedan por el suelo.

## Referencias cruzadas

- [Detección de colisiones básica](basic-collision.md) · [Velocidad y gravedad en plataformas](speed-and-gravity.md) · [Punto fijo](fixed-point.md)
- [MD Engine — pruebas automáticas y perfilado](../MegaDrive/herramientas/md-engine/depuracion-y-generacion.md)
- [Portar un juego de Game Maker a una consola](../MOTORES/arquitectura/port-desde-game-maker.md)

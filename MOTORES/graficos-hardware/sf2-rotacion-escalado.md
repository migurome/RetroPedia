---
titulo: "Street Fighter II: Rotating and Scaling"
plataforma: [General]
categoria: [graficos-hardware]
tags: [cps1, arcade, street-fighter-2, sprites, rotacion-escalado, mame]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/sf2_spin/index.html]
---

# Street Fighter II: Rotating and Scaling

## Resumen

El artículo (título original "Street Fighter II, Spin when you can't") explica cómo el modo "attract" de Street Fighter II logra el efecto de girar y escalar el logo, con un texto "World Warrior" translúcido debajo, a pesar de que el hardware de la CPS-1 es un motor de tiles que **solo puede voltear un tile vertical y/u horizontalmente, sin capacidad de blending real**. Es relevante porque documenta, con capturas fotograma a fotograma extraídas de MAME, los trucos concretos usados para simular rotación, escalado y transparencia sin hardware dedicado para ello.

## Detalle técnico

### El problema

El hardware que ejecuta Street Fighter II es un motor de tiles que solo puede voltear un tile en vertical y/o en horizontal, sin capacidad de blending. Esto era un problema para un juego como SF2, cuyo modo attract implica hacer girar el logo, escalarlo hacia arriba, y mostrar un texto "World Warrior" translúcido debajo. Quien estuviera presente en 1991 quizá recuerde que el efecto se veía "raro", pero era difícil precisar por qué, ya que giraba muy rápido y el CRT difuminaba bastante la imagen.

Explorando la GFXROM mediante el "sheet explorer" del autor, los tiles usados para el logo se encuentran en la sheet **`0x8200`**. Como era de esperar, se encuentran dos versiones del arte: una pequeña y una grande. La pregunta es cómo se usaron.

### Metodología: MAME al rescate

Gracias a MAME es posible ralentizar la animación grabando una secuencia de imágenes pixel-perfect en formato `.mng`:

```
$ mame sf2 -mngwrite sf2.mng
```

Analizando los fotogramas 00001205 a 00001300 con un slider interactivo, se establece que:

- El logo pequeño se mueve hacia abajo para dar espacio (clearance) suficiente al logo grande.
- El logo gira **dos veces**.
- Tras el primer giro, el logo pequeño es reemplazado por el grande.
- **Los tiles no se voltean ni horizontal ni verticalmente**: sus posiciones simplemente se rotan alrededor del centro del logo (es decir, el "giro" es un reposicionamiento de tiles calculado por software, no una rotación de hardware real).
- La translucidez se logra **dibujando el texto solo en fotogramas alternos** (uno de cada dos).
- Si te mueves lo bastante rápido, el efecto resulta visualmente "aceptable".

### Más rotaciones y escalados: la secuencia de créditos

Si el jugador gana el torneo sin perder ni un asalto, se le premia con una secuencia especial de créditos donde ocurren más efectos de escalado y rotación. Con ayuda de MAME de nuevo es posible observar esto en cámara lenta, en cuatro pantallas distintas:

- **Pantalla 1** (fotogramas 10535–10577): el escalado hacia arriba se logra **"apilando" (stacking) tiles unos encima de otros y separándolos progresivamente**.
- **Pantalla 2** (fotogramas 10843–10884): Trivia — los sprites de los luchadores aparecen un fotograma después que el fondo. Esto se debe a que la **CPS-1 usa un framebuffer de sprites que retrasa la actualización de la capa de sprites en un fotograma**.
- **Pantalla 3** (fotogramas 11152–11214).
- **Pantalla 4** (fotogramas 11461–11505).

### Street Fighter II Champion Edition (marzo 1992)

Un año después, Capcom lanzó una versión mejorada llamada "Champion Edition". Para esta ocasión se rehizo el modo attract: el logo ya no gira, pero hace un "fake-scale" hacia abajo y hacia arriba. El cambio entre el logo pequeño y el grande se oculta **cuando todos los tiles están concentrados en un único punto** — el resultado visual es mucho más convincente que el efecto de SF2: World Warrior.

### Street Fighter II Hyper Fighting (diciembre 1992)

El último juego lanzado en CPS-1 (después del cual Capcom pasó a CPS-2). El modo attract no cambió demasiado salvo por el texto: la animación de texto se eliminó por completo, quedando solo el escalado del logo.

### Mirando de reojo: el puerto de SNES

Un año después del lanzamiento arcade, Capcom publicó un puerto para Super Nintendo (15 de julio de 1992). A pesar de usar una consola con menos capacidad y menor resolución, lograron incluir todos los personajes, movimientos y gráficos, y sorprendentemente consiguieron un **efecto de giro mejor** gracias a **Mode 7 + HDMA** de la SNES. Para la translucidez del subtítulo, un simple **dithering con un toque de "wobbling"** (bamboleo) hizo el truco.

Comando usado para inspeccionar la animación de la versión SNES fotograma a fotograma con MAME:

```
$ mame snes -cart "sf2.zip" -mngwrite sf2snes.mng
```

Fotogramas inspeccionados: 00000495 a 00000767.

## Notas / gotchas

- Punto clave de diseño: en la CPS-1, "rotar" un logo no es una operación de hardware — es una reordenación de las posiciones de tiles individuales alrededor de un centro, calculada por software cuadro a cuadro.
- El "escalado" tampoco es un remuestreo real: se logra apilando y separando progresivamente múltiples tiles pre-dibujados en distintos tamaños (versión pequeña / versión grande), no interpolando una única imagen.
- La transparencia/translucidez es en realidad parpadeo (dibujar el elemento cada dos fotogramas), no blending por alpha — depende de la persistencia visual humana y del desenfoque del CRT para "leerse" bien.
- Dato de arquitectura importante para timing de renderizado: la CPS-1 usa un framebuffer de sprites que introduce un retraso de un fotograma en la capa OBJ respecto a los fondos — visible en la secuencia de créditos cuando los luchadores aparecen un frame después que el escenario.
- Capcom abandonó el efecto de rotación real en Champion Edition, prefiriendo ocultar el cambio de sprite en un punto de concentración — una solución más barata y visualmente más limpia que mantener la rotación por software.
- El puerto de SNES logra mejor calidad visual que el arcade original gracias a características de hardware específicas de esa consola (Mode 7 + HDMA), no por tener más potencia bruta.

## Referencias cruzadas

- (pendiente de enlazar)

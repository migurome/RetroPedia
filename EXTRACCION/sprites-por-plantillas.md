---
titulo: Extraer sprites de vídeo con una biblioteca de poses (búsqueda por plantilla y refinado "Venn")
plataforma: [General]
categoria: [extraccion]
tags: [sprites, template-matching, opencv, poses, animacion, paleta, contorno, deduplicacion]
estado: borrador
fecha: 2026-10-22
fuentes: [proyecto Extractor (validación con Rick, 6.143 fotogramas), Guzdial & Riedl "Game Engine Learning from Video" (IJCAI 2017), MarioNette (NeurIPS 2021)]
---

# Extraer sprites de vídeo con una biblioteca de poses

## Resumen

En un juego (y en muchas animaciones pixel art) cada personaje se dibuja con un **conjunto finito de sprites que se reutilizan al píxel**. Se arma una biblioteca de poses por personaje y se busca cada pose en cada fotograma con `cv2.matchTemplate` y máscara. La comparación solo mira los píxeles del sprite, así que **da igual el fondo, la cámara o el parallax**. Es el enfoque de Guzdial y Riedl para reconstruir el motor de Super Mario Bros. desde vídeo. Además del sprite limpio, da **qué pose sale en cada fotograma, dónde y durante cuánto tiempo**.

## Detalle técnico

### 1. Plantillas iniciales

- Se sacan de donde el personaje se aísla fácil: tramos de cámara fija contra su fondo (ver [fondos-desde-video.md](fondos-desde-video.md)) o recortándolo a mano una vez.
- Hay que agrupar los recortes por personaje antes. Con histograma de paleta 6x6x6, coseno < 0,12 y altura parecida (ratio 0,6–1,6) salen grupos razonables, aunque con ruido que conviene revisar a ojo.

### 2. Búsqueda en todos los fotogramas

```python
core = cv2.erode(alpha, np.ones((3,3), np.uint8))          # ignorar el borde del sprite
mask = np.repeat(core[..., None], 3, 2).astype(np.float32)
r = cv2.matchTemplate(frame, tpl, cv2.TM_SQDIFF, mask=mask)  # frame y tpl en float32
v, _, loc, _ = cv2.minMaxLoc(r)
rms = (v / (core.sum() * 3)) ** 0.5                          # error por canal, 0-255
```

- Hay que probar también la **versión en espejo**: en el caso real, el 68% de las coincidencias estaban volteadas.
- Umbral: un `rms < 6` es prácticamente idéntico y `< 10` es fiable. Lo que hay entre 10 y 30 suele ser una pose parecida, un tinte de color o un efecto encima.
- Coste: unos 4 ms por plantilla y fotograma a 320x180. Con 84 plantillas y su espejo sale a ~0,7 s por fotograma, así que hay que paralelizar (`multiprocessing.Pool`, `cv2.setNumThreads(1)` en cada worker).

**Validación real (Rick, 84 plantillas, 6.143 fotogramas de 6 niveles):**
- El 25% de los fotogramas tenía coincidencia `rms < 6` y el 35% `< 10`, sin añadir ninguna pose nueva.
- Una sola pose (stand) apareció **1.507 veces** en 143 posiciones distintas.
- Cobertura por nivel: del 13% al 56%. Las causas de que no coincidiera fueron poses nuevas, el personaje disfrazado (samurái en un nivel) y fotogramas sin él.

### 3. Refinado "Venn": quedarse con lo que coincide

Cada pose aparece muchas veces sobre fondos distintos. Si se apilan esas apariciones, **lo que no cambia es el sprite y lo que cambia es el fondo**:

1. Se recorta un recuadro con 8 px de margen alrededor de cada aparición, volteando las que van en espejo.
2. Se eligen apariciones con el **fondo alrededor realmente distinto**: un anillo fuera del sprite que difiera más de 25 de media respecto a todas las ya elegidas, hasta 40.
3. Un píxel es del sprite si coincide (tolerancia 16) en al menos el 80% de las elegidas y está conectado al núcleo de la pose.
4. **Se itera.** La pose limpia sustituye a la plantilla, encuentra más apariciones (incluidas las que la plantilla sucia no encontraba) y se repite.
5. Con menos de 5 fondos distintos la pose queda **pendiente**; no se inventa.

### 4. Paleta y contorno (complemento, sobre todo para poses vistas pocas veces)

- La pose de stand de Rick usa **13 colores** y el **100% de sus píxeles de borde son oscuros**. Son tres colores de contorno: azul marino #0d172c para bata y pelo, negro #090308 y marrón oscuro #361e0d para el pantalón.
- **Filtro de paleta:** un píxel que no esté en la paleta del personaje no es suyo. Así caen los restos de fondo.
- **Contorno como muro:** se rellena desde fuera del recuadro y el relleno se para en los colores de contorno; lo que queda encerrado es el personaje. Funciona con una sola aparición.
- **Ajuste a paleta final:** se imanta cada píxel al color más cercano de la paleta. Quita el ruido del códec y hace exactas las comparaciones.

### 5. De poses a movimientos

Con la secuencia "pose por fotograma" de cada personaje:
- las transiciones frecuentes forman los ciclos (stand, andar, puñetazo…),
- la duración de cada pose da el timing de la animación,
- el desplazamiento entre fotogramas da la velocidad.

Es la materia prima para reprogramar la lógica, igual que en Game Engine Learning from Video.

## Notas / gotchas

- **Plantilla sucia, búsqueda sesgada.** Si la plantilla lleva pegado a otro personaje (Morty junto a Rick), solo encuentra fotogramas donde ambos están así, y la intersección lo conserva. Se arregla iterando con la plantilla ya limpia o recortándola de nuevo.
- **Apariciones en el mismo sitio no cuentan como fondos distintos.** Un personaje quieto 200 fotogramas sobre el mismo fondo hace que ese fondo "coincida" y se cuele como halo. Hay que elegir por diversidad del anillo, no por número de apariciones.
- **No rellenar los huecos interiores** (entre brazo y cuerpo): son fondo. La intersección pura ya los deja transparentes.
- **Fondos oscuros vs. contorno oscuro:** en niveles azul marino o negros, el muro de contorno se "pega" al fondo. Hay que combinarlo con el Venn y no usarlo solo.
- **Tintes y flashes:** el parpadeo blanco al recibir daño o los filtros de color de una escena no coinciden con la plantilla. O se tolera con un umbral más alto, o se compara la forma (máscara) en vez del color.
- **Escalas distintas entre escenas** (×12 frente a ×16): el personaje puede estar dibujado a otro tamaño. Hace falta una biblioteca por escala.
- **Deduplicar al guardar:** se usa una clave exacta (colores >> 4 más máscara) y, como respaldo, una comparación con los sprites del mismo tamaño (máscara distinta < 6%, color medio < 12).

## Referencias cruzadas

- [rejilla-pixel-art-desde-video.md](rejilla-pixel-art-desde-video.md)
- [fondos-desde-video.md](fondos-desde-video.md)
- [segmentacion-ia-y-descomposicion.md](segmentacion-ia-y-descomposicion.md): SAM 2 para poses nuevas y MarioNette como versión automática.
- [caso-rick-and-morty-pixel.md](caso-rick-and-morty-pixel.md)
- [MegaDrive/graficos/bec-animated-sprites.md](../MegaDrive/graficos/bec-animated-sprites.md): cómo se organiza una animación en el lado del motor.
- [Game Engine Learning from Video (IJCAI 2017)](https://ijcai.org/proceedings/2017/518), [uso de OpenCV con paleta de sprites](https://arxiv.org/pdf/1603.02738).

---
titulo: Cómo se hizo el efecto de fuego de DOOM (PSX)
plataforma: [General]
categoria: [efectos]
tags: [doom, playstation, psx, fuego, efectos-visuales, framebuffer]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/doom_fire_psx/index.html]
---

# Cómo se hizo el efecto de fuego de DOOM (PSX)

## Resumen

El artículo analiza el célebre efecto de fuego animado que aparece en la intro (y en el juego) de la versión PSX de Final DOOM. Es un algoritmo minúsculo — un buffer de "temperatura" por píxel que se propaga hacia arriba con un decaimiento aleatorio — capaz de generar una animación de fuego convincente con muy poco cálculo por frame, algo relevante para cualquier motor con presupuesto de CPU ajustado (como un motor top-down en Mega Drive).

## Detalle técnico

### Contexto histórico

El efecto se implementó en la versión PSX de Final DOOM. Según el artículo, el equipo de desarrollo, tras optimizar el resto del código, se encontró con ciclos de CPU disponibles y decidió usarlos para generar el fuego animado durante la intro y parte del gameplay. El análisis técnico moderno de este efecto fue posible gracias a que Samuel Villarreal había hecho ingeniería inversa previamente de la versión N64, lo que proporcionó la base para reconstruir el algoritmo.

### Estructura de datos base

- Cada valor del framebuffer de fuego está en el rango **[0, 36]**.
- Ese rango se asocia a una paleta de color que transiciona desde blanco (valor máximo, 36) hasta negro (valor 0), pasando por amarillo, naranja y rojo — modelando visualmente la temperatura de una partícula de fuego a medida que asciende y se enfría.
- El buffer se inicializa completamente a cero (negro), excepto una única línea de píxeles en la base, que se fija a blanco (valor 36) y actúa como "fuente" de calor constante.
- El origen (índice 0) del array corresponde a la esquina superior izquierda de la pantalla.

### Algoritmo core

La idea central: para cada píxel del buffer, el "calor" se propaga al píxel situado directamente encima, disminuyendo en el proceso. La línea inferior (la fuente) nunca se actualiza — actúa como generador infinito de calor.

**Versión base (sin aleatoriedad):**

```javascript
function doFire() {
    for(x = 0 ; x < FIRE_WIDTH; x++) {
        for (y = 1; y < FIRE_HEIGHT ; y++) {
            spreadFire(y * FIRE_WIDTH + x);
        }
    }
}

function spreadFire(from) {
    var to = from - FIRE_WIDTH;
    firePixels[to] = firePixels[from] - 1;
}
```

Aquí `spreadFire` toma el índice `from` del píxel actual, calcula `to` (el píxel de la fila justo encima, restando el ancho del buffer) y copia el valor decrementado en 1. Recorriendo todas las columnas y todas las filas (de abajo hacia arriba, empezando en `y = 1` para no tocar la fuente en `y = 0`), se actualiza el buffer completo cada frame.

**Versión mejorada, con decaimiento variable (introduce aleatoriedad):**

```javascript
function spreadFire(from) {
    var rand = Math.round(Math.random() * 3.0) & 3;
    var to = from - FIRE_WIDTH;
    firePixels[to] = firePixels[from] - (rand & 1);
}
```

Aquí en lugar de decrementar siempre en 1, se genera un número aleatorio `rand` en `[0,3]` y se decrementa en `(rand & 1)`, es decir, en 0 o en 1 con probabilidad aproximadamente igual. Esto rompe la uniformidad del decaimiento lineal y da al fuego un aspecto más orgánico, con "lenguas" de fuego de longitud variable.

**Versión con propagación lateral (simula viento):**

```javascript
function spreadFire(from) {
    var rand = Math.round(Math.random() * 3.0) & 3;
    var to = from - FIRE_WITH - rand + 1;
    firePixels[to] = firePixels[from] - (rand & 1);
}
```

(Nota: `FIRE_WITH` aparece así en el código fuente citado por el artículo — probablemente una errata de `FIRE_WIDTH` en el original.) Aquí el destino `to` ya no está exactamente encima del origen: se desplaza lateralmente según el mismo valor aleatorio `rand` (`-rand + 1`, es decir, un desplazamiento en el rango [-2, 1]). El resultado es que el calor no solo sube sino que también se inclina lateralmente, simulando el efecto del viento sobre las llamas.

### Mecánica de propagación y renderizado

- **Propagación de calor**: para cada píxel, el calor se propaga al píxel directamente encima (o, en la versión con viento, a uno de los píxeles vecinos de la fila de encima).
- **Decaimiento**: el calor decrece (resta 1, o 0/1 aleatoriamente) en cada propagación hacia arriba. La aleatoriedad es clave para la ilusión visual: varía la velocidad de "enfriamiento" de cada columna de píxeles.
- **Buffer y dibujado en pantalla**: el array se recorre y se dibuja directamente en pantalla; el índice 0 corresponde a la esquina superior izquierda, y el array se extiende hasta `FIRE_HEIGHT × FIRE_WIDTH - 1`.
- El bucle exterior recorre `x` (columnas) y el interior recorre `y` (filas, de abajo hacia arriba empezando en `y=1`), llamando a `spreadFire` con el índice lineal `y * FIRE_WIDTH + x`.

### Optimización y diseño

El efecto aprovecha operaciones a nivel de bit (`& 3`, `& 1`) en vez de aritmética más costosa, apropiado para hardware limitado como la PSX. La paleta de colores — cuidadosamente elegida para ir de blanco a negro pasando por amarillo/naranja/rojo — es la que hace el "trabajo pesado" visual: el algoritmo en sí solo maneja un índice de intensidad por píxel (0-36), y es la tabla de color la que traduce ese índice a un color de fuego convincente. Esto separa completamente la simulación (aritmética entera simple) de la presentación (lookup de paleta), un patrón económico en cualquier hardware con paleta indexada.

## Notas / gotchas

- El algoritmo es extremadamente barato: cada píxel del buffer requiere solo una resta y, en las versiones mejoradas, una operación aleatoria y un `&`. Nada de trigonometría, ruido Perlin ni física real de fluidos.
- La calidad visual del efecto depende tanto del algoritmo de propagación como de la paleta de colores elegida — sin una rampa de color adecuada (blanco→amarillo→naranja→rojo→negro) el resultado no se lee como "fuego".
- La versión con desplazamiento lateral (viento) introduce un sesgo direccional simplemente cambiando cómo se calcula el índice `to`, sin añadir estructuras de datos nuevas.
- El artículo menciona una versión limpia/reescrita disponible en GitHub (`DoomFirePSX/flames.html`) y la versión original (interactiva, en CodePen) de Samuel Villarreal, quien había hecho la ingeniería inversa original sobre la versión N64 de DOOM.
- Al portar esto a un motor con paleta indexada real (como el VDP de Mega Drive), el patrón se traduce directamente: un buffer de índices de intensidad en RAM que se recorre por CPU y se traduce a color vía la CRAM/paleta de hardware, sin necesidad de tocar directamente el framebuffer de color.

## Referencias cruzadas

- (pendiente de enlazar)

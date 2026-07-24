---
titulo: Diseñando el sistema de vídeo de la SNES
plataforma: [General]
categoria: [graficos-hardware]
tags: [snes, ntsc, pal, crt, video-timing, overscan, modo-alta-resolucion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/snes_video/index.html]
---

# Diseñando el sistema de vídeo de la SNES

## Resumen

Tercera entrada de la serie: aquí Fabien Sanglard se aleja de la arquitectura interna del PPU (cubierta en las otras dos entradas) y reconstruye el proceso de ingeniería que llevó a Nintendo a elegir la resolución, el timing y el reloj de vídeo concretos de la SNES, partiendo de cómo funciona físicamente un televisor CRT NTSC/PAL de la época. Es la entrada más relevante para entender **por qué la SNES tiene la resolución y el framerate que tiene**, y cómo esas cifras están atadas a un oscilador maestro compartido por todo el sistema.

## Detalle técnico

### Fundamentos: cómo funciona un televisor CRT de la época

Un televisor analógico de la época recibe una señal NTSC por antena, la pasa por un sintonizador, y la muestra en un tubo de rayos catódicos (CRT). Además, suele tener conectores auxiliares: un jack compuesto (amarillo) para vídeo y jacks estéreo (blanco/rojo) para audio.

**El CRT como máquina de dibujar líneas:** se clasifican por su frecuencia horizontal — 15 kHz significa capacidad de dibujar unas 15.000 líneas por segundo. Internamente el CRT tiene tres cañones de electrones (rojo, verde, azul), dos conjuntos de electroimanes (control vertical y horizontal), y una máscara de sombra situada delante de las tiras de fósforo que dirige los electrones a la tira de color correcta.

**Concepto crítico señalado por el autor: no existen "píxeles" dentro de un CRT.** Las ranuras de fósforo no son píxeles; los electrones garantizan impactar en la tira de color correcta, pero la precisión espacial real depende de la densidad de ranuras de la máscara (una pantalla HD tiene mayor densidad de ranuras).

**Señales de control del CRT:** cinco señales viajando por cuatro hilos:

- Rojo, Verde, Azul (RGB): conectados directamente a los cañones de electrones; a mayor señal, más electrones, más brillo.
- Un cable blanco de sincronización compuesta (CSYNC), que en realidad contiene dos señales entrelazadas: HSYNC (sincronización horizontal) y VSYNC (sincronización vertical).

Aclaración importante: el CRT **no emite** VSYNC — solo lo consume, intentando sincronizar el cañón de electrones con la señal recibida.

### Mecánica de dibujado del CRT

El CRT dibuja línea a línea (raster), de izquierda a derecha. HSYNC resetea la posición X a 0 (vuelve al inicio de la línea); VSYNC resetea la posición Y a 0 (vuelve al inicio de la pantalla).

**El descubrimiento clave que señala el autor**: el cañón se mueve hacia la derecha con una ligera pendiente descendente. Al recibir un HSYNC, el cañón vuelve a X=0 pero apuntando un poco más abajo que antes, de forma que la siguiente línea se dibuja justo debajo de la anterior.

**Campos progresivos vs. entrelazados:**

- Modo progresivo ("p"): VSYNC se emite junto con el último HSYNC del cuadro; las líneas se dibujan siempre en la misma posición en cada ciclo.
- Modo entrelazado ("i"): VSYNC se emite a mitad de un HSYNC; los campos alternos quedan intercalados verticalmente entre sí.

La compensación del entrelazado es: se duplica la resolución vertical percibida, pero la tasa de refresco de cada línea concreta se reduce a la mitad. La especificación NTSC entrega dos campos a ~30 Hz cada uno. Todos los CRT de la época estaban preparados con el espaciado adecuado entre líneas para el entrelazado — dibujar en modo progresivo en un CRT preparado para entrelazado deja huecos negros visibles entre líneas (los "scanlines" característicos de estas pantallas).

**Contenido de línea: digital vs. analógico.** El CRT es "digital" respecto a las líneas (la sincronización es discreta, HSYNC/VSYNC), pero es completamente **analógico en el contenido horizontal**: como los tres cañones están conectados directamente a los hilos RGB, la señal de color puede cambiar de forma continua dentro de una línea, permitiendo en teoría cualquier resolución horizontal. El límite real viene de la propagación de la señal y de la densidad de la máscara de sombra.

### Restricciones NTSC que había que cumplir

Especificación NTSC (el estándar que todos los CRT de consumo de 1989 esperaban recibir):

- Relación de aspecto: 4:3
- Líneas por campo: 262,5 (la media línea existe precisamente para permitir el entrelazado)
- Puntos por línea: 341,25
- Frecuencia de campo: 59,94 Hz

**Curiosidad sobre el 59,94 Hz:** los televisores en blanco y negro NTSC históricos usaban 60 Hz (la frecuencia de la red eléctrica). Cuando los ingenieros de radiodifusión añadieron color sin romper la compatibilidad con los receptores en blanco y negro existentes, tuvieron que reducir la frecuencia en un 0,1% para evitar artefactos de color — de ahí el 59,94 en vez de 60,00.

### Diseño inicial ingenuo

Primeras decisiones de partida (antes de topar con las restricciones reales):

- Líneas: 262 (en modo progresivo, sin la media línea)
- Puntos horizontales: 350 (coincide exactamente con la relación de aspecto 4:3)
- Tasa de fotogramas objetivo: 59,94 Hz

Cálculo del reloj de puntos necesario para esas cifras:

```
262 líneas × 350 puntos × 59.94Hz = 5,496,498Hz
```

Un ASIC simplificado para esto sería: un contador de ticks del reloj de puntos que emite HSYNC cada 350 ticks, y VSYNC cada 350 × 262 = 91.700 ticks.

**Problemas descubiertos con este diseño ingenuo:**

1. No existe un reloj de ~5,5 MHz disponible de forma barata en una consola con presupuesto de silicio restringido.
2. No se puede dibujar color de forma continua sin más (problema de overscan, ver más abajo).

### La restricción crítica: un reloj maestro compartido

La SNES comparte un **único oscilador maestro** que todos los subsistemas deben usar mediante divisores de frecuencia.

- Reloj maestro NTSC: 21,47727 MHz
- Reloj de puntos resultante: 21,47727 MHz ÷ 4 = **5,3693175 MHz** (la aproximación real al ideal de 5,5 MHz calculado antes)

Esta restricción de compartir un único oscilador es la que termina empujando al alza el número de puntos horizontales necesarios (ver más abajo).

### Overscan: solución a los artefactos visuales

**Por qué hace falta overscan:**

- Al resetear verticalmente (tras VSYNC), el cañón sigue disparando electrones durante varios microsegundos mientras el sistema se estabiliza (oscilaciones/"wobbling" verticales).
- Al resetear horizontalmente (tras HSYNC), ocurre el mismo problema en el eje X.
- Los televisores de la época tienden a hacer "over-scan" (mostrar una imagen ligeramente más grande que la pantalla visible) precisamente para ocultar ese wobbling en los bordes.

**Solución: VBLANK y HBLANK.** VBLANK es el período de silencio vertical tras el VSYNC (sin disparo de electrones); HBLANK es el período de silencio horizontal tras el HSYNC.

**Comparativa de sistemas de la época (1989-1990):**

| Máquina | Año | Líneas totales | Líneas VBLANK | Líneas visibles | Líneas/seg | Fotogramas |
|---------|-----|---|---|---|---|---|
| Capcom CPS-1 | 1989 | 262 | 38 | 224 | 15.622 | 59,63 Hz |
| Sega Genesis | 1989 | 262 | 38 | 224 | 15.700 | 59,92 Hz |
| Neo-Geo AES | 1990 | 264 | 40 | 224 | 15.734 | 59,18 Hz |

### Selección de la resolución vertical de la SNES

**Decisión: 224 líneas visibles + 38 líneas en blanco = 262 líneas totales.**

Justificación de ingeniería que da el autor:

- Todos los competidores directos (Mega Drive, Neo-Geo, CPS-1) usaban también 224 líneas visibles.
- 224 es divisible por 16 (`224 ÷ 16 = 14`), lo que encaja perfectamente con un pipeline gráfico basado en tilemaps de 16×16.
- Nintendo optó por no reinventar la rueda en este aspecto: su apuesta era lograr gráficos superiores mediante PPUs más capaces, no mediante mayor resolución bruta.

**Dato anecdótico — R-Type (Irem, 1987):** el sistema arcade M72 personalizado usaba 284 líneas × 512 puntos con un reloj de 8 MHz, con overscan de 128 puntos (HBLANK) y 28 líneas (VBLANK), logrando una resolución activa de 384×256 — mayor que la de los títulos arcade contemporáneos. La contrapartida fue una tasa de refresco de 55,017605 Hz, un 10% por debajo de los valores recomendados para CRT, lo que lo hace difícil de emular hoy en día con precisión.

### Selección de la resolución horizontal de la SNES

Ecuación fundamental de tasa de fotogramas:

```
tasa refresco = (líneas × puntos) / reloj puntos
```

Con el reloj de puntos ya fijado en 5.369.317,5 Hz (21,47727 MHz ÷ 4), 262 líneas, y una tasa objetivo de 59,94 Hz:

```
puntos = 5,369,317.5 / (262 × 59.94) ≈ 342
```

**Restricción adicional:** por razones de artefactos de la portadora de color en la salida de vídeo compuesto, los ingenieros de Nintendo necesitaban **341 puntos por línea** en lugar de 342.

Tasa de fotogramas real resultante con 341 puntos:

```
60.098Hz = 5,369,317.5 / (341 × 262)
```

60,098 Hz frente al NTSC nominal de 59,94 Hz: una desviación dentro de la tolerancia que un CRT puede absorber, como ya demostraba el caso de R-Type citado antes.

### Resolución horizontal visible: overscan horizontal

De los 341 puntos por línea, no todos son utilizables (aplica el mismo problema de wobbling/artefactos que justifica el overscan vertical). Restricciones que entran en juego:

1. **Relación de aspecto 4:3**: exigiría 224 × (4/3) ≈ 298 puntos visibles.
2. **Alineación con tilemaps de 16×16**: los valores viables serían 304 (19 tiles), 288 (18 tiles), 272 (17 tiles), 256 (16 tiles), 240 (15 tiles)... El valor "óptimo" por distorsión mínima sería 304 puntos.
3. **Restricción del PPU**: el buffer de línea de sprites debe llenarse durante el HBLANK. Con 128 sprites esto exige un tiempo considerable; usando 304 puntos visibles, el HBLANK resultante sería de solo 37 puntos (~7 µs), insuficiente para ese llenado.

**Decisión final de la SNES:**

- Puntos visibles: **256**
- HBLANK resultante: 85 puntos = **16 µs**, tiempo suficiente para la recuperación de datos de sprites.
- Relación de aspecto resultante: **8:7** (no 4:3) — como consecuencia, hay una ligera distorsión cuando el CRT convierte la señal a 4:3 en pantalla.

### Modos de alta resolución

**Modo de alta resolución vertical (entrelazado):** duplicar la resolución vertical a 448 líneas modificando el contador de VSYNC — emitiendo VSYNC media línea después del último HSYNC. Resultado: 262,5 líneas por fotograma, con una tasa de refresco de campo de 60,098 Hz ÷ 2 = **30,049 Hz**. Efecto secundario: parpadeo (flicker) visible y molesto, a cambio de mayor resolución vertical.

**Modo de alta resolución horizontal ("el hack"):** duplicar la resolución horizontal era imposible por falta de un reloj de puntos más rápido disponible. La solución fue desplazar cada segundo campo horizontalmente, de forma que sus puntos quedaran intercalados entre los puntos del campo anterior. Resultado: mitad de la tasa de fotogramas efectiva y un desbordamiento de color considerable; se usó principalmente en pantallas de menú.

El artículo incluye un listado (28 títulos) de juegos que usaron modos de alta resolución, con ejemplos documentados como:
- "Ball Bullet Gun (lower screen)" — 512×224, BgMode5.
- "Donkey Kong Country 1 (Nintendo logo)" — 512×224, BgMode5.
- "Chrono Trigger (crash into Lavos sequence)" — modo entrelazado.

### PAL vs. NTSC: mercados regionales

**Especificación PAL** (Europa, Australia, etc.): tasa de refresco exactamente a 50 Hz, 312,5 líneas por campo.

**Solución de hardware en la SNES PAL:**

- Oscilador: 17,7344750 MHz (frente a los 21,4772700 MHz de la versión NTSC).
- Etapa S-CLK con un factor 6/5.
- Divisor final de /4.

```
Reloj puntos PAL = 17.734475 × (6/5) ÷ 4 = 5.32034250MHz
```

**Problema grave: bandas negras.** Usar las mismas 224 líneas gráficas en un sistema de 312,5 líneas por campo deja bandas negras masivas arriba y abajo de la imagen. La solución fue un modo "Overscan" que incrementa las líneas visibles a 240 (16 líneas adicionales, exactamente la altura de un tile).

**Adopción real del overscan en PAL:** solo 12 títulos lo llegaron a usar nunca, principalmente porque los desarrolladores no sabían qué contenido poner en esas 16 líneas adicionales. La excepción notable fue Super Mario World, que sí aprovechó ese espacio extra para ampliar la vista vertical.

**Comparativa Super Mario World NTSC vs PAL:**
- NTSC: 256×224 con relación 4:3.
- PAL: 256×240 con relación 4:3 (imagen comprimida verticalmente para encajar el contenido extra).

**Problema de timing en PAL:** el código de los juegos rara vez se revisaba específicamente para el VSYNC de PAL (50,00697891 Hz frente a los 60,098 Hz de NTSC):

```
Ralentización = 50.009 / 60.098 = 83.1% velocidad original
Resultado: juegos ~17% más lentos en PAL (sin que hubiera forma sencilla de que el público de la época lo supiera, sin internet)
```

### Salidas y conectividad

En vez de descartar las señales "puras" generadas internamente, la SNES realiza una conversión activa a múltiples formatos según lo que la TV del usuario soporte. Tres formatos soportados:

1. RGB/CSync puro
2. Vídeo compuesto
3. S-Video

**Conector AV de la SNES (multipin, 9 pines relevantes descritos):**

```
1. Rojo (RGB)
2. Verde (RGB)
3. C-Sync
4. Azul (RGB)
5. Tierra
6. Tierra
7. Luminancia (S-Video)
8. Crominancia (S-Video)
9. Vídeo Compuesto
10. +5V DC
11. Audio Izquierdo
12. Audio Derecho
```

La ventaja de este diseño es que el usuario tiene acceso "à la carte" a señal RGB/CSync pura, compuesta, o S-Video, según lo que su televisor soporte.

**Contexto europeo — SCART/Prise Péritel:** los televisores europeos (especialmente en Francia) equipados con conectores SCART podían recibir cables que alimentaban el CRT directamente con la máxima fidelidad visual posible, aunque limitados siempre al formato PAL (250-312 líneas, ~17% más lento que NTSC).

### Resumen final de especificaciones SNES

**NTSC (Norteamérica):**
- Resolución visible: 256×224 píxeles
- Resolución total con overscan: 341×262 píxeles
- Tasa de fotogramas: 60,098 Hz
- Reloj de puntos: 5,3693175 MHz (derivado de 21,47727 MHz ÷ 4)
- Relación de aspecto: 8:7 (distorsionada respecto a 4:3)
- Líneas de VBLANK: 38
- Puntos de HBLANK: 85 (16 µs para el buffering de sprites)

**PAL (Europa/Australia):**
- Resolución visible: 256×224 (o 256×240 con overscan)
- Tasa de fotogramas: 50,007 Hz
- Reloj de puntos: 5,32034250 MHz
- Líneas de VBLANK: 40
- Resolución alternativa (overscan): 256×240, adoptada solo por 12 títulos

### Fuentes citadas por el autor

El artículo cita, entre otras: el vídeo de YouTube "Inside 1990 Nintendo Headquarters in Kyoto" (con el equipo de Masayuki Uemura), analog.com ("Basics of analog video"), foros de NESdev sobre especificaciones NTSC, Wikipedia (reducción del 0,1% por la introducción del color), Digilent ("VGA Display Controller"), el documento fullsnes.txt de Problemkaputt (listado exhaustivo de software en alta resolución), manuales oficiales (SNES Pal Manual, esquemático de la SNES), y el blog Nerdly Pleasures (análisis de framerate en sistemas clásicos y entrelazado en consolas).

## Notas / gotchas

- Ninguna cifra de la SNES (224 líneas, 256 puntos visibles, 60,098 Hz) es un número "bonito" elegido a mano — todas son consecuencia de encadenar restricciones reales: un único oscilador maestro compartido por todo el sistema, presupuesto de silicio, alineación con tilemaps de 16×16, tiempo de buffering de sprites en HBLANK, y compatibilidad con las especificaciones NTSC/PAL de los CRT existentes.
- El hecho de que la SNES corra realmente a 60,098 Hz (no 59,94 Hz) y que PAL corra a 50,007 Hz (no exactamente 50,00) tiene implicaciones prácticas de emulación y de temporización de juego que rara vez se documentan explícitamente en el propio juego.
- La relación de aspecto real de la SNES es 8:7, no 4:3 — cualquier emulación o análisis que asuma 4:3 sin corregir introduce una distorsión que no estaba en el hardware original de la forma esperada por el usuario.
- El "hack" de alta resolución horizontal (desplazar campos alternos) es un buen ejemplo histórico de cómo resolver una limitación de reloj disponible mediante astucia de timing en vez de más silicio — a costa de la mitad del framerate.
- La discrepancia de rendimiento del ~17% entre versiones NTSC y PAL de un mismo juego (por diferencia de VSYNC) es un problema de clase completamente distinta al de "diferencias de resolución": afecta a la velocidad de la lógica de juego si el código no compensa explícitamente el framerate real del sistema.

## Referencias cruzadas

- (pendiente de enlazar)

---
titulo: Dibujar formas con las ventanas (windows) de la PPU
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, ventanas, windows, hdma, mascaras, efectos]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Drawing_window_shapes]
---

# Dibujar formas con las ventanas (windows) de la PPU

## Resumen

Las "ventanas" (windows) de la PPU de SNES son máscaras rectangulares por scanline que se pueden combinar entre sí y, controladas por HDMA scanline a scanline, permiten recortar formas arbitrarias (círculos, triángulos, diamantes, octágonos...) sobre fondos y sprites. Esta entrada recoge las técnicas para construir esas formas a partir de las dos ventanas de hardware disponibles.

## Detalle técnico

### Hardware de ventanas

La PPU (S-PPU) ofrece dos ventanas que pueden combinarse mediante operaciones lógicas (OR, AND, XOR, XNOR), controladas por los registros **WBGLOG** y **WOBJLOG**. Cada ventana individual se puede además invertir usando los registros **W12SEL**, **W34SEL** y **WOBJSEL**. Cada ventana se define por una posición izquierda y una derecha, escritas en **WH0** y **WH1** (para la ventana 1) — de forma análoga existen WH2/WH3 para la segunda ventana.

### Formas de combinar dos ventanas

- **OR lógico**: una ventana marca el borde izquierdo de la forma final, la otra el borde derecho; el resultado es la unión de ambas.
- **XOR lógico**: una ventana forma el contorno exterior y la otra recorta un hueco interior (cuando una está contenida dentro de la otra), útil para anillos o marcos.
- **AND con la segunda ventana invertida**: `(ventana1 AND (NOT ventana2))` resta la ventana 2 de la ventana 1, es decir, "ventana1 menos ventana2".

Cuando las dos ventanas necesarias para una forma tienen alturas (rangos de scanlines) distintas, suele consumir menos ROM usar dos canales de HDMA independientes en vez de un único canal en modo "cuatro registros".

### Requisitos de implementación con HDMA

- Una única ventana puede montarse con un canal de HDMA en modo "dos registros, escritura única" apuntando a WH0/WH1, o bien con dos canales en modo "un registro, escritura única" cada uno.
- Las tablas de HDMA que cambian dinámicamente deben ir con doble buffer, para evitar parpadeos o "tearing" cuando se están reconstruyendo mientras el HDMA aún las está leyendo.
- Cada entrada de HDMA cubre como máximo 127 scanlines; los tramos más largos hay que partirlos en varias entradas.
- Las posiciones izquierda/derecha se recortan a 8 bits (0-255): si el cálculo da un valor por debajo de 0, se convierte en 0; si se pasa de 255, se convierte en 255.
- Hace falta comprobar explícitamente si la forma se sale de pantalla, salvo que se pueda garantizar de antemano que siempre está dentro del rango horizontal visible.

### Ventanas "vacías"

Se puede conseguir un estado de "sin ventana" (ninguna zona seleccionada) haciendo que la posición izquierda sea mayor que la derecha — por ejemplo, fijando izquierda > 0 y derecha = 0 en una entrada de HDMA sin repetición.

### Construcción de un rectángulo

Se compone de hasta cuatro tramos: un tramo inicial opcional "sin ventana" (si el rectángulo no empieza en el scanline 0), el tramo con la ventana activa, un último tramo de un solo scanline "sin ventana" para cerrar, y el terminador de la tabla HDMA (`0x00`).

Se describe un pseudocódigo de una función `non_repeating_entry()` que gestiona entradas de hasta 254 scanlines partiéndolas automáticamente en dos si superan el límite de 127 scanlines por entrada.

### Trapecios

Las formas con lados rectos (no verticales) se consiguen dibujando trapecios, variando la posición izquierda y/o derecha scanline a scanline. Métodos de cálculo de esa variación línea a línea:

- **Algoritmo de Bresenham**.
- **Delta-X en punto fijo 8.8**: sumar o restar una cantidad fraccionaria por scanline, con caminos de código separados según el delta sea positivo o negativo.
- **Incrementos de valor fijo**: el método más rápido, pero limitado a proporciones enteras; en pendientes muy pronunciadas puede hacer falta repetir la misma posición en varios scanlines seguidos.
- **Tablas de deltas precalculadas en ROM**: valores de desplazamiento en X precalculados para ángulos concretos, lo que además permite curvas y círculos, no solo líneas rectas.

Se incluye pseudocódigo (sin optimizar) que muestra una entrada de HDMA en modo repetición combinada con aritmética de punto fijo, recorte a los límites de pantalla y comprobación de si la forma se sale de pantalla.

### Ventaja de los trapecios "agudos" (acute trapezium)

- Si la base corta del trapecio está dentro de pantalla, no hace falta comprobar si se sale de pantalla en ese extremo.
- Con la base corta arriba: el borde izquierdo siempre se mueve hacia la izquierda y el derecho siempre hacia la derecha; una vez que un borde llega a su límite (0 o 255), los valores siguientes ya no cambian, lo que simplifica el recorte.
- Se puede construir de abajo hacia arriba para simplificar aún más ese recorte.

### Ejemplos de formas construidas combinando trapecios

- **Triángulo** (con base horizontal): un único trapecio agudo con una base superior mínima.
- **Triángulo apuntando a la derecha**: dos trapecios con ángulos de pendiente distintos.
- **Diamante**: dos trapecios agudos, el segundo como espejo vertical del primero.
- **Octágono**: dos trapecios agudos separados por un tramo rectangular en el medio.

### Ventanas precalculadas

Se pueden precalcular las posiciones izquierda/derecha de cada scanline y guardarlas en una tabla de datos en ROM, construyendo la tabla de HDMA a partir de esos datos en tiempo de ejecución.

**Traslación vertical**: para mover la forma hacia abajo, se inserta un tramo "sin ventana" antes; para moverla hacia arriba, se saltan directamente los primeros scanlines de la tabla precalculada.

**Traslación horizontal**: se suma o resta un desfase a las posiciones izquierda/derecha; opcionalmente se comprueba si se sale de pantalla; separar el código según la traslación sea hacia la izquierda o hacia la derecha permite hacer los cálculos con el acumulador en modo 8 bits.

### Ventanas precalculadas simétricas horizontalmente

Para formas simétricas respecto a un eje vertical central, basta con guardar un único valor de "media anchura" por scanline, y calcular las posiciones reales así:

```
left = xCenter - halfWidth
right = xCenter + halfWidth
```

Separar el cálculo en tres caminos de código (xCenter < 0, 0 ≤ xCenter ≤ 255, xCenter > 255) evita comprobaciones innecesarias y permite trabajar con el acumulador en modo 8 bits sin signo en el caso común.

## Notas / gotchas

- Máximo 127 scanlines por entrada de HDMA en modo repetición; las formas más altas requieren partir la tabla en varias entradas.
- Las posiciones izquierda/derecha se recortan siempre a 0-255; hay que decidir explícitamente qué hacer cuando el cálculo se sale de ese rango (clamping) para no obtener formas corruptas.
- Las tablas de HDMA que se regeneran cada frame (formas animadas/dinámicas) necesitan doble buffer para evitar tearing.
- Con trapecios "agudos" construidos de abajo hacia arriba y con la base corta dentro de pantalla, se puede evitar por completo la comprobación de fuera-de-pantalla en ese extremo, ahorrando ciclos.
- Elegir el método de interpolación línea a línea (Bresenham, punto fijo 8.8, incrementos fijos, o tabla de deltas en ROM) es una decisión de compromiso entre precisión, velocidad y tamaño en ROM.

## Referencias cruzadas

- Ver `hdma-ejemplos.md` (mecánica general de tablas de HDMA, formatos de entrada y límites de scanlines usados aquí).
- Ver `vblank-routine.md` (dónde y cuándo se actualizan de forma segura las tablas de HDMA dinámicas mencionadas en esta entrada).

---
titulo: "Bumbershoot Software — modo pseudo-bitmap mediante dos planos de scroll superpuestos"
plataforma: [MegaDrive]
categoria: [graficos]
tags: [bumbershoot, vdp, tiles, planos-de-scroll, bitmap, vram, nametable]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/04/07/genesis-designing-a-pseudo-bitmap-mode/]
---

# Bumbershoot Software — modo pseudo-bitmap mediante dos planos de scroll superpuestos

## Resumen

El autor porta a Genesis un **autómata celular cíclico** (Cyclic Cellular Automaton) que en su versión original de DOS BASIC dibujaba directamente en un framebuffer de píxeles. Como el VDP de Genesis no tiene ningún modo bitmap real (todo se compone a partir de tiles de 8×8 referenciados desde un nametable), diseña una codificación que aprovecha dos planos de scroll y un truco de volteo vertical de tile para simular una rejilla de "píxeles" (celdas) direccionable, sin pagar el coste de una tabla de patrones 1:1 por celda.

## Detalle técnico

### El autómata que hay que dibujar

Cyclic Cellular Automaton: una rejilla de celdas en estados aleatorios de 1 a N; en cada tick, si una celda en estado X tiene un vecino en estado X+1, es "devorada" y pasa a ese estado. Objetivo de diseño en Genesis: rejilla de **128×128 celdas con 16 estados** (colores), de los cuales serían visibles en pantalla unos 80×46 a la vez; el original en BASIC usaba 100×100 celdas con 12-14 estados, cifra que el autor toma como referencia de "qué tan grande hace falta que sea esto para que se vea bien".

### Por qué un bitmap real no es viable

Un framebuffer de verdad, a razón de un tile por celda en una rejilla de 64×64 tiles (necesaria para cubrir 128×128 "medios-tiles"), costaría **131.072 bytes** solo de patrones — más que toda la VRAM disponible (64KB). De ahí la necesidad de una codificación más compacta.

### El truco: nybbles como color de cuadrante, no patrón por celda

En vez de un patrón por celda, el autor define **256 patrones de tile fijos** donde, cita textual, "el nybble bajo y el nybble alto representan el color del cuadrante superior izquierdo y superior derecho, respectivamente" — dejando el resto del tile (la mitad inferior) en blanco. Es decir: un único tile de 8×8 codifica, mediante su *índice* de patrón (0-255), el color de **dos celdas horizontalmente adyacentes** (4 bits cada una = 16 estados posibles, que casan justo con los 16 estados del autómata). El contenido real de los píxeles del tile es fijo para cada uno de los 256 valores posibles de (nybble bajo, nybble alto); lo único que cambia por celda de pantalla es qué patrón de esos 256 se referencia desde el nametable.

### Duplicar la densidad vertical sin duplicar la tabla de patrones

Como cada patrón solo rellena la mitad superior del tile, hace falta otro mecanismo para la mitad inferior. El autor plantea dos alternativas y se queda con la segunda por más simple de implementar:

1. Desplazar el segundo plano de scroll 4 píxeles hacia abajo respecto al primero (offset físico de scroll).
2. **Solución adoptada:** en vez de tocar el registro de scroll, marcar las celdas del nametable correspondientes a las filas impares de celdas como **volteadas verticalmente** (vertical flip, un bit normal de cualquier entrada de nametable de Genesis). Cita textual: *"podríamos hacer el offset teniendo la segunda capa de scroll cuatro píxeles por debajo de la primera, pero en realidad es más fácil atar las dos capas por completo y simplemente decirle a cada una de las celdas de las filas impares que estén volteadas verticalmente."*

Esto evita depender de un desplazamiento de scroll fraccionario/independiente entre capas y reduce el problema a un simple flag por entrada de nametable, ya soportado de forma nativa por el hardware.

### Presupuesto de memoria

- **Tabla de patrones:** 256 patrones × 32 bytes = **8 KB** de VRAM (compartida por ambos planos).
- **Nametables:** dos planos de 64×64 celdas = **16 KB** combinados.
- **RAM de trabajo (68000):** la rejilla de estado del autómata, 128×128×2 bytes = **32.768 bytes** (dos copias, para poder calcular la siguiente generación sin pisar la actual mientras se lee), dejando aproximadamente **31 KB libres** de los 64KB de RAM principal para el resto del programa.
- **VRAM restante** (64KB − 8KB − 16KB = 40KB) queda libre para ventana (window) o fuente de texto.

Cita textual sobre por qué esto es viable justo ahora: *"la Genesis es el primer sistema que hemos tenido donde esto es remotamente razonable"* — en plataformas de 8 bits anteriores del propio autor, la VRAM disponible no habría alcanzado ni de lejos para esta codificación.

## Notas / gotchas

- El artículo no detalla explícitamente cómo se reparte el trabajo entre el plano A y el plano B en términos de transparencia (p. ej. si el color de paleta índice 0 del tile se deja transparente para que un plano se vea "a través" del otro en la mitad no rellenada); solo confirma que son "dos capas de scroll" atadas entre sí mediante el truco del flip vertical. Si se reimplementa esta técnica, conviene validar en hardware/emulador real cómo se comporta el compuesto final antes de asumir el mecanismo exacto de superposición.
- El emparejamiento de 2 celdas horizontales por patrón (nybble bajo/alto) significa que la "resolución" real de la rejilla de color no es libre por píxel — sigue atada a la granularidad de tile del VDP, solo que reorganizada para maximizar cuántos estados de autómata caben por tile en vez de cuánto detalle gráfico arbitrario.
- Esta es la misma serie de artículos que documenta, al día siguiente, el problema de ancho de banda de VRAM al intentar refrescar esta rejilla completa cada tick (ver referencia cruzada) — es decir, el diseño de datos aquí descrito es precisamente lo que obliga a repartir la actualización en dos frames en la entrada relacionada.
- Técnica pensada para contenido generado proceduralmente (estados de autómata), no para arte pintado a mano tile a tile como en el resto de esta serie (comparar con `bumbershoot-hello-world-y-bitmap-vdp.md`, que a pesar de su nombre usa tilemap convencional, no esta codificación de cuadrantes).

## Referencias cruzadas

- [Bumbershoot Software — presupuesto de ancho de banda a VRAM y la "venganza" de las badlines del C64](bumbershoot-badlines-revenge.md) — el problema de refresco de VRAM que surge directamente de este diseño de datos.
- [Tiles y paletas](tiles-and-palettes.md) — formato base de tile/paleta que esta técnica reutiliza de forma no convencional.
- [Bumbershoot Software — VDP desde cero: "Hello World" y bitmap con ASM puro](bumbershoot-hello-world-y-bitmap-vdp.md) — otro artículo de la misma serie que, pese al nombre, usa un tilemap normal en vez de esta codificación por cuadrantes.

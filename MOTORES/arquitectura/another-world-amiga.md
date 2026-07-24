---
titulo: Another World en Amiga 500 - Blitter, bitplanes y renderizado de polígonos
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, amiga-500, blitter, bitplanes, poligonos-2d, motorola-68000]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons_amiga500/index.html]
---

# The polygons of Another World: Amiga 500

## Resumen

Segundo artículo de la serie de Fabien Sanglard sobre los ports de Another World. Analiza la implementación original (no un port, sino la versión de desarrollo) en Amiga 500, y en concreto cómo Eric Chahi usó el coprocesador Blitter del chipset Agnus para rasterizar polígonos rellenos y líneas directamente en los bitplanes del framebuffer. Es relevante para diseño de motores porque muestra cómo un motor de polígonos 2D puede apoyarse en hardware de "area fill" y "line draw" en lugar de rasterizar por software, y cómo se resuelve el problema de escribir un color de 4 bits en 4 bitplanes separados sin corromper el framebuffer entre polígono y polígono.

## Detalle técnico

### Historia del Amiga

El proyecto Amiga nace a mediados de 1982 cuando Larry Kaplan llama a Jay Miner. Ambos habían trabajado en Atari en los 70 y ambos se habían marchado: Kaplan por falta de reconocimiento, Miner porque la dirección le impedía usar el entonces caro Motorola 68000. Tras dejar Atari, Kaplan había fundado Activision. Al ser contactado por inversores para desarrollar una nueva plataforma de juego, reclutó a Miner para dirigir el hardware de la nueva empresa, "Hi-Toro". El sistema recibió el nombre en clave "Lorraine".

A finales de 1983 se ensambló un prototipo. Sin sistema operativo, la máquina impresionó a los visitantes del CES de enero de 1984 gracias a la demo "Boing Ball", capaz de mover sprites enormes a 60 fps. La primera máquina se anunció en 1985 como "The Amiga from Commodore", más tarde renombrada Amiga 1000.

**Trivia:** los Amiga 1000 no podían arrancar por sí mismos, no tenían ROM. El bootloader estaba en un disquete que más te valía no perder ni dañar.

### El A500

En 1985, tras una serie de errores, Commodore se encontró en una situación de caja alarmantemente baja. Thomas Rattigan, entonces COO, hizo cambios drásticos. Además de un plan ambicioso que cubría casi todas las operaciones de la empresa, refinó el Amiga 1000 en dos productos: una versión de gama alta orientada al mercado creativo, el Amiga 2000, y una versión de bajo coste para sustituir al Commodore 64 en el mercado de gama baja: el Amiga 500.

El Amiga 500 (A500) se lanzó en 1987. Impulsado por un 68000 a 7.16 MHz con 512 KiB de RAM, se convirtió en una máquina enormemente exitosa, querida por jugadores, programadores y en particular por la gente de la demoscene. Fue el segundo producto más vendido de Commodore, con una estimación de 6 millones de unidades entre 1987 y 1991 (el más vendido fue el C64, con 17 millones de unidades).

El A500 venía con un par de ROMs con el bootstrap Kickstart integrado, una mejora significativa respecto al A1000, donde los usuarios tenían que arrancar con Kickstart en disquete y luego cambiar al disquete del programa (juego o Workbench) que quisieran ejecutar. Era posible conectar un disco duro al A500, aunque eran unidades grandes y aparatosas de 5.25" o 3.5". Modelos posteriores como el A1200 ya tenían espacio incorporado para un HDD de 2.5".

### Arquitectura

Como su tecnología estaba originalmente pensada para impulsar una videoconsola, el Amiga no estaba construido alrededor de una CPU "todopoderosa" con capacidad de audio y vídeo como añadido tardío, al modo de la mayoría de ordenadores de la época. El querido 68000 (32/16 bits) operaba junto a un Chipset que alojaba tres chips potentes llamados Paula (audio), Denise (vídeo) y Agnus (manipulación de datos y sincronización).

El diseño limpio, con un sistema de memoria que ofrecía no sólo direccionamiento plano sino también RAM compartida accesible tanto por la CPU como por el Chipset, contribuyó enormemente a la popularidad del Amiga entre desarrolladores. En comparación, ni la Sega Genesis ni la Nintendo SNES, dos sistemas potentes lanzados años después (enero de 1989 y noviembre de 1990 respectivamente), tenían memoria compartida.

El bus del Chipset tiene un sistema de prioridad complejo que aprovecha que el 68000 está inactivo en los ciclos pares. El DMA del Chipset intenta usar sólo los ciclos impares para multiplexar el acceso al bus sin afectar a la CPU. Las cosas no siempre eran perfectas: Agnus en particular tiende a "acomodarse" hasta el punto de que el 68000 sufre inanición (starvation).

Para resolver esto, los clientes podían comprar una extensión de "Fast RAM" con un "CPU Bus" dedicado al 68000. Con las instrucciones del 68000 alojadas ahí, la CPU no sufría inanición cuando el canal DMA del Blitter estaba activo. Esto duplicaba efectivamente la velocidad de ejecución del 68000.

### Sistema de vídeo

El sistema de vídeo está completamente gobernado por Denise, que ofrece un total de veinte modos gráficos. La resolución más popular era 320x200, cuyo aspect ratio de 1.6 no coincidía con el de los monitores de la época (4/3 = 1.3). Ese desajuste de aspect ratio produce una distorsión al enviar el framebuffer al monitor CRT.

El framebuffer no se almacena de forma contigua, sino en áreas de memoria separadas llamadas bitplanes. Se pueden asignar hasta cinco bitplanes de 8 KiB cada uno, resultando en 5 bits por píxel, es decir, 32 índices de color. A primera vista este enfoque planar parece un diseño torpe (especialmente para un desarrollador con background de PC), pero Agnus, y especialmente su Blitter, aportan mucha claridad al conjunto.

El sistema de color de la paleta se basa en un espacio de color RGB de 4 bits por canal. El resultado, 12 bits por color, permite hasta 4.096 colores distintos, muchos más de los habituales en ordenadores de precio similar de la época. Había muchos trucos para mostrar más colores, como usar el Copper para cambiar la paleta en HSYNC, o el modo Hold-And-Modify (HAM), que permitía hasta 4.096 colores fotorrealistas en pantalla simultáneamente.

Ejemplo de coordenadas en el espacio de color RGB de 4 bits: negro en (0x0, 0x0, 0x0), rojo en (0xF, 0x0, 0x0), verde en (0x0, 0xF, 0x0), azul en (0x0, 0x0, 0xF) y blanco en (0xF, 0xF, 0xF).

### Another World en Amiga

Técnicamente, Another World en Amiga no es, hablando con propiedad, un port. Como el A500 era la máquina de desarrollo, es la versión original, construida entre 1989 y 1991 por Eric Chahi, entonces con 21 años, trabajando solo en su habitación.

Dos razones hicieron del Amiga la máquina de desarrollo perfecta:
1. El **GenLock** permitía superponer la salida de una cámara de vídeo sobre la salida propia del ordenador, lo que habilitó el rotoscopiado.
2. Y lo más importante: **Agnus** facilitaba enormemente el renderizado de polígonos.

### El Blitter

La idea de basar el juego exclusivamente en polígonos surgió de una suposición errónea: que "Dragon's Lair, Escape from Singe's Castle" en Amiga usaba polígonos. La pregunta que Eric necesitaba responder era si podía averiguar cómo hacerlo con un framerate razonable. Fue durante esta fase de I+D cuando el Blitter del Amiga jugó un papel clave.

### Dibujando polígonos

La documentación del Blitter menciona una característica llamada **"Area Fill Mode"**. No se trata de proyección 3D ni de texturizado sofisticado: el Blitter opera en espacio de pantalla sobre líneas de bits con la capacidad de "rellenar el hueco". Funciona escaneando de derecha a izquierda (el Blitter debe estar en modo descendente en area fill mode). Mientras el Blitter ve 0s, no ocurre nada. En cuanto pasa el primer 1, el Blitter rellena la línea con 1s hasta el siguiente 1. El dibujo de la documentación ilustra bien cómo funciona. Nótese que incluso un polígono cóncavo se renderiza correctamente con este método:

```
     bit array before                  bit array after
  ______________________           ______________________
 |                      |         |                      |
 |                      |         |                      |
 |                      |         |                      |
 |   1   1      1   1   |         |   11111      11111   |
 |    1  1       1  1   |         |    1111       1111   |
 |     1 1        1 1   |         |     111        111   |
 |      11         11   |         |      11         11   |
 |     1 1        1 1   |         |     111        111   |
 |    1  1       1  1   |         |    1111       1111   |
 |   1   1      1   1   |         |   11111      11111   |
 |                      |         |                      |
 |                      |         |                      |
 |______________________|         |______________________|
```

Esta solución de "Area Fill Mode" sólo lleva a un segundo problema: cómo dibujar los "bordes" del polígono. Si se mira de cerca el dibujo anterior, se ve que no es un algoritmo de línea de Bresenham estándar, ya que las líneas horizontales deben saltarse. Por suerte, los diseñadores del Amiga dotaron al Blitter de un **"line draw mode"**.

Aun así quedan problemas por resolver:

1. **Coste por bitplane:** "dibujar las líneas y luego rellenar el área" debe hacerse cuatro veces (una por cada bitplane), lo que parece muy costoso.
2. **Degradación del buffer:** el motor necesita renderizar cientos de polígonos. El Blitter necesita un buffer limpio lleno de ceros y bordes limpios hechos de unos para operar. Tras unos pocos polígonos, el framebuffer se convertiría con toda seguridad en una "sopa de bits" muy propensa a confundir a Agnus.
3. **Salida de color:** el Blitter sólo puede escribir 1s, pero se necesita poder escribir también ceros en algunos de los cuatro bitplanes para generar el color de 4 bits correcto.

**Solución (scratchpad / "cookie cut"):** la solución consiste en configurar las entradas A, B y C del Blitter mediante operaciones lógicas (min-terms). Se cita al blogger Scali, que lo explica así (cita textual preservada):

> ... there is a solution to this problem, and it is not even that difficult. The blitter can render anywhere in chipmem, so it is easy to just set up a temporary clean buffer, a 'scratchpad', and render the polygon into that. Then you copy it to the actual screen area using a masked blit operation. This is also affectionately known as a 'cookie cut'. It is basically the same sort of operation that you would use with 2D bitmaps, where you only write pixels when they are set in the source image, and leave the target pixels untouched otherwise (a logical OR operation), a simple form of transparency. This will combine the polygons properly on screen.
>
> In fact, perhaps this would be a good time to explain the blitter in a bit more detail. The blitter has 3 inputs and 1 output. These are all handled by DMA, so it can run completely independently from the CPU, once it is set up. The 3 inputs can be combined via logical operations, so-called min-terms. The result is then written to the output channel. In the case of a masked blit, you would basically perform an operation like this:
>
> output = (mask AND bitmap) OR (NOT mask AND output)
>
> — Scali's OpenBlog™

Con el conocimiento de las entradas del Blitter, el pipeline completo de renderizado de un polígono queda así (5 pasos):

1. Asignar un buffer de scratch (scratchpad).
2. Limpiarlo con 0s usando el Blitter.
3. Dibujar los bordes del polígono en "draw line mode" con 1s.
4. Rellenar el área del buffer con 1s en "area fill mode".
5. Blitear el buffer de scratch cuatro veces (una por cada bitplane), usando la operación de "cookie cut" (mask AND bitmap) OR (NOT mask AND output) para componer sin corromper el framebuffer final.

No fue un paseo, pero funcionó. El esfuerzo mereció la pena: la rutina fue capaz de manejar **hasta 50 polígonos (dependiendo de su tamaño) a 20 fotogramas por segundo**.

### Copiar el framebuffer (caché de fondo)

Por rápido que fuera el Blitter dibujando polígonos, seguía sin ser suficientemente rápido desde el punto de vista creativo. Para que las escenas fueran visualmente atractivas, cada frame estaba compuesto de miles de polígonos. Algunos son tan pequeños (1x1, llamados "pixígonos") que no justifican la sobrecarga de procesarlos individualmente. Uno de los primeros fondos del juego (cuando Lester emerge de la piscina) está compuesto por **981 polígonos**.

La solución fue cachear el fondo en un buffer especial llamado **BKGD**, mediante una simple copia. Una vez más, era una tarea hecha a medida para el Blitter. Se cita la documentación del Amiga Developer CD v2.1 (OS 3.5):

> The Blitter is one of the two co-processors in the Amiga. Part of the Agnus chip, it is used to copy rectangular blocks of memory around and to draw lines. When copying memory, it is approximately twice as fast as the 68000, able to move almost four megabytes per second. It can draw lines at almost a million pixels per second.

Con una tasa de 4.000 bytes por milisegundo, blitear el buffer BKGD al principio de cada nuevo frame llevaba "sólo" **8 ms**.

**Trivia:** aunque Another World era un juego pulido, algunos bordes quedaron "afilados". Si un jugador intentaba ser listo durante el minijuego de la rueda de protección anticopia y pulsaba "c" para introducir un código y saltar directamente al gameplay, la pantalla se ponía verde y el Amiga se congelaba. La única salida era un reset físico (hard reset).

### Rellenar el framebuffer (Fill)

Aunque la CPU 68000 puede escribir 16 bits a la vez, limpiar un framebuffer también se hacía probablemente con el Blitter. La documentación del Amiga incluye un ejemplo de código, "clearmem", que se apoya en el Blitter para limpiar 128 KiB de RAM.

### Lecturas adicionales

El artículo remite al repositorio de GitHub de Gregory Montoir (rawgl) para más trivialidades y detalles sobre el port de Amiga.

## Notas / gotchas

- El framebuffer planar (bitplanes) obliga a repetir cada operación de dibujo de polígono (línea + relleno) **cuatro veces**, una por bitplane, lo cual a priori parece muy costoso; el hardware compensa esto porque el Blitter opera de forma autónoma vía DMA, en paralelo a la CPU.
- El Blitter sólo puede escribir 1s en "area fill mode" / "line draw mode"; no puede escribir directamente sobre el framebuffer real sin corromperlo tras muchos polígonos. Por eso es obligatorio el patrón de "scratchpad + cookie cut" (blit con máscara) en vez de dibujar polígono a polígono directamente sobre el destino.
- El "Area Fill Mode" del Blitter requiere que el Blitter esté configurado en modo descendente (scanning de derecha a izquierda); si no, el algoritmo de relleno no funciona como se espera (nota aclaratoria acreditada a "@DrSlem" en el artículo original).
- El caché de fondo (BKGD) no es opcional: sin él, sería inviable renderizar per-frame los ~981 polígonos de un fondo típico manteniendo 20 fps. La estrategia de diseño es "componer una vez, copiar muchas veces" en lugar de "redibujar cada frame".
- El aspect ratio de 320x200 (1.6) no coincide con el 4:3 (1.3) de los monitores CRT de la época, lo que introduce distorsión visual — una limitación de hardware que el motor no intenta corregir por software.
- La "Fast RAM" opcional resuelve la inanición del 68000 cuando el Blitter usa el bus intensivamente, pero es una compra aparte; el motor no puede asumir su presencia en todas las máquinas.

## Referencias cruzadas

- [another-world-overview.md](another-world-overview.md) (arquitectura general de la VM y opcodes)
- [another-world-atari-st.md](another-world-atari-st.md) (port a Atari ST, sin Blitter para dibujo de polígonos)
- (pendiente de enlazar)

---
titulo: Another World - Arquitectura general del motor de polígonos (VM y bytecode)
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, out-of-this-world, maquina-virtual, bytecode, poligonos-2d, eric-chahi]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons/index.html]
---

# The polygons of Another World (overview)

## Resumen

Artículo de introducción a una serie de Fabien Sanglard que analiza cómo "Another World" (Eric Chahi, 1991; conocido en Norteamérica como "Out Of This World") fue implementado y portado a numerosas plataformas de finales de los 80 / principios de los 90 (Amiga 500, Atari ST, IBM PC, Super Nintendo, Sega Genesis, etc.). Es relevante para el diseño de motores porque describe la arquitectura de una máquina virtual con bytecode que separa por completo la lógica de juego del hardware de renderizado, permitiendo portar el motor con un coste de reescritura mínimo: sólo hay que reimplementar el "host" de la VM y, sobre todo, optimizar tres o cuatro opcodes críticos de dibujo.

## Detalle técnico

### Por qué Another World es un mejor caso de estudio que DOOM (para el autor)

El autor explica que suele estudiar arquitecturas de hardware retro portando mentalmente software conocido. DOOM es una opción habitual porque su núcleo no tiene "layering violations" y sus seis subsistemas de I/O son fáciles de localizar y leer. Sin embargo, argumenta que Another World es más interesante de estudiar que DOOM precisamente por su naturaleza basada en polígonos, que se presta a optimizaciones muy agresivas dependientes del hardware. En algunos casos, trucos ingeniosos permitieron ejecutar Another World en hardware fabricado hasta cinco años antes del lanzamiento del juego.

Esta serie de artículos es un recorrido por el hardware de videojuegos de principios de los 90. Para cada máquina (Amiga 500, Atari ST, IBM PC, Super Nintendo, Sega Genesis...) el autor intentó descubrir cómo se implementó Another World, en algunos casos contactando con los desarrolladores originales, en otros analizando directamente el desensamblado.

### Another World 101 — la máquina virtual

Hay muy poco código fuente "real" en Another World. La versión original de Amiga tenía, según se reporta, 6.000 líneas de ensamblador. El ejecutable de la versión DOS de PC ocupa solamente 20 KiB, algo sorprendente para un juego tan extenso que cabía en un único disquete de 1.44 MiB. La razón es que la mayor parte de la lógica de negocio del juego está implementada como bytecode: el ejecutable de Another World es en realidad un host de máquina virtual que lee y ejecuta opcodes `uint8_t`.

La VM de Another World define:
- **256 variables** para gestión de estado
- **64 threads** (hilos) de ejecución concurrente
- **29 opcodes**
- **4 framebuffers**

Eso es todo. Si se construye un host de VM capaz de manejar estos cuatro elementos, se puede ejecutar el juego. Si además se consigue que la VM sea lo bastante rápida para correr a 20 fotogramas por segundo, se puede jugar de verdad.

### Sistema gráfico: resolución y paleta

El sistema gráfico de la máquina virtual usa un sistema de coordenadas de **320x200** con **16 colores** basados en paleta. La limitación a 16 colores puede sorprender dado que la plataforma de desarrollo, el Amiga 500, soportaba hasta 32 colores. Esta elección fue un punto óptimo que permitía compatibilidad gráfica con la otra gran plataforma de la época, el Atari ST, que sólo soporta 16 colores.

La limitación resultó ser una bendición disfrazada: dio como resultado un estilo visual único que ha envejecido bien.

Incluso cuando habría sido posible usar una paleta específica para ciertas escenas, Eric Chahi optó por no hacerlo. Durante el primer encuentro con "La Bestia", la criatura se dibuja usando sólo tres colores: negro para el cuerpo, rojo para los ojos y beige para los dientes. La imaginación del jugador hace el resto.

### Técnicas basadas en paleta

**Translucidez / transparencia:** el motor es capaz de efectos de translucidez si la escena usa solamente ocho colores. Los colores "sin iluminar" se almacenan en el rango `[0x0, 0x7]` mientras que las variantes "iluminadas" (lit shades) ocupan `[0x8, 0xF]`. Los haces de luz del Ferrari son translúcidos y se dibujan con un color especial `0x10`, que no existe como tal porque sólo hay 16 colores disponibles. Este valor especial se interpreta como: "leer el índice del framebuffer, sumarle 0x8 y escribirlo de vuelta". La última parte del truco consiste en elegir cuidadosamente los 8 colores siguientes de la paleta para que el resultado tenga sentido visualmente. Este efecto de translucidez no se usa mucho en el juego, pero reaparece durante la cinemática del accidente del experimento, cuando el rayo está a punto de teletransportar a Lester a Another World.

**Intercambio de paleta (palette swap):** un cambio de paleta poco costoso basta para evocar un relámpago. Se muestra un ejemplo con dos paletas (antes/después) y una fila de valores alternando negro/blanco que ilustra el "flash" del relámpago.

**Persistencia del framebuffer de fondo:** en las pantallas iniciales tras salir de la piscina donde empieza el juego, el juego dibuja huellas y marcas de derrape bajo los pies de Lester mientras camina por la arena. Si el jugador se desplaza una pantalla a la derecha y luego vuelve a la izquierda, el framebuffer modificado se restaura, preservando las huellas. Es un detalle tan sutil que puede pasar desapercibido en una partida normal, pero ilustra la atención al detalle de Eric Chahi (cita atribuida a Martin Atkins).

### Los cuatro framebuffers

De los cuatro framebuffers, dos se usan para double buffering (doble buffer) y dos se usan para guardar composiciones de fondo (BKGD1 y BKGD2). Esto es una optimización para evitar tener que redibujar todos los polígonos estáticos del fondo en cada frame, sustituyéndolo por una simple operación de copia.

Flujo de una nueva escena:
1. Una nueva escena se dibuja primero en el buffer **BKGD1**.
2. En cada nuevo frame, BKGD1 se copia por completo al buffer de doble buffer que no está siendo escaneado por la pantalla en ese momento.
3. Ahí se dibujan los elementos en movimiento (Lester, enemigos, etc.).
4. Una vez que un objeto como el coche se "aparca" (deja de moverse), también se dibuja en el buffer BKGD1, para minimizar el número de polígonos a renderizar en los frames siguientes.

La composición usa un algoritmo pintor (painter's algorithm) sencillo, lo que produce overdraw significativo. Esto no es un problema porque queda muy amortizado gracias a la copia de BKGD1 (no hay que repetir el overdraw en cada frame, sólo una vez al crear el fondo).

El segundo buffer, **BKGD2**, se usa para cachear el último fondo. Es útil durante las cinemáticas cuando la escena de fondo va y viene entre dos vistas distintas (como en la introducción, que alterna entre el escritorio de Lester y el interior del acelerador de partículas). Durante el juego normal, permite una aceleración significativa cuando el jugador vuelve a la pantalla anterior.

### Los 29 opcodes de la máquina virtual

El artículo presenta una tabla con los 29 opcodes organizados por rango hexadecimal. Incluye gestión de threads (THRD), gestión de framebuffers (FB) y todas las operaciones de gestión de registros. La mayoría de los opcodes son "fáciles" de implementar, excepto `COPY FB`, `FILL FB` y las variantes `DRAW_POLY*`, que son difíciles por razones de rendimiento.

**Operaciones núcleo (0x00–0x0F):**
`CMOV`, `MOV`, `ADD`, `CADD`, `CALL`, `RET`, `PAUSE THRD`, `COND JMP`, `SET VECT`, `JNZ`, `CJMP`, `SET PAL`, `RESET THRD`, `SLCT FB`, `FILL FB`, `COPY FB`

**Operaciones extendidas (0x10–0x1A):**
`BLIT FB`, `KILL THRD`, `DRAW TEXT`, `SUB`, `AND`, `OR`, `SHL`, `SHR`, `PLAY SOUND`, `LOAD RESC`, `PLAY MUSIC`

**Operaciones gráficas de dibujo de polígonos:**
- `DRAW_POLY_SPRITE` ocupa el rango de opcode 0x40–0x7F
- `DRAW_POLY_BACKGROUND` ocupa el rango de opcode 0x80–0xFF

**Truco de ahorro de espacio de opcode:** ambos opcodes `DRAW_POLY_*` ocupan más de una entrada en la tabla. En el caso de `DRAW_POLY_BACKGROUND`, ocupa la mitad de todo el espacio de opcodes, de 0x80 a 0xFF. A primera vista parece un desperdicio, pero es un truco de ahorro de espacio: usar todos los opcodes que empiezan por el bit "1" permite que los otros 7 bits viajen "de gorra" (piggy-back) en el propio espacio de opcode como parámetros de dibujo. Como este opcode se usa para fondos y cinemáticas, el ahorro de espacio es muy significativo.

La versión `SPRITE` usa todos los opcodes que empiezan por los bits "01", mientras que los 6 bits restantes codifican x, y y zoom, para poder dibujar "sprites" como Lester, sus amigos y los enemigos.

### Siguiente paso

Como se menciona en la sección anterior, 26 de los 29 opcodes son fáciles de implementar. El verdadero reto al portar este juego estaba en manipular píxeles dentro de las limitaciones de ancho de banda del bus y la CPU de cada máquina. El resto de la serie estudia cómo cada port manipuló los framebuffers y cómo resolvió los problemas de DRAW, FILL y COPY, empezando por Amiga 500 (la plataforma de desarrollo original), seguido de Atari ST, IBM PC, Super Nintendo y Sega Genesis.

## Notas / gotchas

- La elección de 16 colores no fue una limitación puramente técnica del Amiga (que soportaba hasta 32), sino una decisión de diseño para garantizar compatibilidad visual con el Atari ST, que sólo soportaba 16. Es un ejemplo de diseñar "para el mínimo común múltiplo" de las plataformas objetivo desde el principio, en vez de portar a posteriori.
- El painter's algorithm (sin z-buffer, sin oclusión) es deliberadamente simple, a costa de overdraw; la decisión de diseño que lo hace viable es amortizar ese coste cacheando el fondo compuesto en un buffer aparte (BKGD1) en lugar de optimizar el propio algoritmo de pintado.
- Codificar parámetros de dibujo dentro de los bits altos del propio opcode (en `DRAW_POLY_SPRITE`/`DRAW_POLY_BACKGROUND`) es un patrón de compresión de bytecode muy relevante para un juego que debía caber en 1.44 MiB con recursos, sonido y niveles incluidos.
- El diseño de VM+bytecode traslada el problema de portabilidad a un problema mucho más pequeño: sólo 3-4 opcodes (`COPY FB`, `FILL FB`, `DRAW_POLY_SPRITE`, `DRAW_POLY_BACKGROUND`) son críticos de rendimiento y requieren optimización específica por plataforma; el resto del intérprete es trivialmente portable.

## Referencias cruzadas

- [another-world-amiga.md](another-world-amiga.md) (port a Amiga 500, plataforma de desarrollo original)
- [another-world-atari-st.md](another-world-atari-st.md) (port a Atari ST)
- (pendiente de enlazar)

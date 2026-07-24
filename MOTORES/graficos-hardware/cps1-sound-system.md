---
titulo: CPS-1 Sound System
plataforma: [General]
categoria: [graficos-hardware]
tags: [cps1, arcade, street-fighter-2, sonido, z80, ym2151, oki-msm6295]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/sf2_sound_system/index.html]
---

# CPS-1 Sound System

## Resumen

El artículo (publicado como "Street Fighter II, Sound System internals") documenta la arquitectura interna del subsistema de sonido de la placa arcade CPS-1 de Capcom, usando Street Fighter II como caso de estudio. A diferencia del subsistema gráfico (bien documentado en MAME gracias al trabajo de Paul Leaman sobre los ASIC CPS-A/CPS-B), el subsistema de sonido estaba pobremente documentado, y el autor lo investiga con Ghidra. Es relevante porque expone con detalle un protocolo de comunicación entre CPUs sin líneas de handshake bidireccionales, y el reparto de trabajo entre síntesis FM y reproducción de muestras ADPCM — un patrón de diseño muy aplicable a un motor retro con recursos de sonido limitados.

## Detalle técnico

### Contexto: la era dorada del arcade

En los 80, cada juego arcade se diseñaba desde cero con una placa base nueva y combinaciones variadas de procesadores. Dos fabricantes decidieron estandarizar su hardware para adelantarse a la competencia: SNK con el Neo-Geo y Capcom con el CP-System (más tarde conocido como CPS-1). El artículo se centra en el sistema de sonido de la CPS-1.

### Estudiando la CPS-1

La CPS-1 está parcialmente documentada en MAME gracias a miles de horas de trabajo de la comunidad. El driver `drivers/cps1.cpp` de MAME, en las líneas `3614` y `6678`, da una buena visión general de cómo funciona la máquina.

**Arquitectura general** (según diagrama `cps1_arch.svg` del artículo):
- En el centro está un **Motorola 68000 a 10 MHz**, que actúa como sistema de control: ejecuta la lógica del juego, lee las entradas (inputs) y emite comandos al sistema gráfico y al sistema de audio.
- El sistema de control configura el sistema gráfico mediante una GFX RAM compartida donde se describe el layout de cada frame. Como están en el mismo bus, los accesos a la GFX RAM son arbitrados por el controlador de bus.
- Los ASIC gráficos **CPS-A** y **CPS-B** están emulados por MAME y su formato de comandos y registros está bien documentado (`video/cps1.cpp`), gracias al trabajo de Paul Leaman en 1998.
- Lo que está mucho menos claro es cómo opera el sistema de sonido. Involucra un **Z-80 a 3.579 MHz**, un **Yamaha YM2151** para la música y un **OKI MSM6295** para los efectos de sonido, pero más allá de eso es una caja negra: por diseño, MAME no tiene conocimiento de la lógica interna de cada juego, solo emula componentes y garantiza que los datos fluyan por los buses virtuales según lo esperado. El autor recurre a Ghidra donde termina el mundo de MAME.

### Interfaz de latches Control/Sonido

Desafío no trivial: garantizar que todos los comandos emitidos se reciban sin pérdidas ni duplicados, usando solo **dos latches de 1 byte**. El 68000 (10 MHz) puede escribir en ellos pero no leerlos; el Z-80 (3.579 MHz) puede leerlos pero no escribirlos. ¿Cómo lograr comunicación 100% fiable entre ambas CPUs?

**Problema 1 — el lector es más lento que el escritor**, lo que puede provocar que se "pierda" un comando. Esto se invierte mediante interrupciones de CPU:
- La línea **IPL1** del 68000 está conectada directamente a la línea **VSYNC** del sistema de vídeo.
- La línea **INT** del Z-80 está conectada a la línea de interrupción (**IRQ**) del YM2151 (cuyo temporizador está configurado para disparar cada **4 ms**).

De este modo el escritor "tiquea" cada 16 ms mientras que el lector tiquea cada 4 ms. Esto garantiza que no se pierdan comandos, pero introduce el problema de los duplicados.

**Problema 2 — duplicados.** Para evitarlos, el Z-80 descarta una entrada si es idéntica a la última recibida. Esto genera un problema adicional si el 68000 necesita enviar el mismo comando dos veces seguidas: para resolverlo, el escritor se compromete a nunca escribir el mismo byte dos veces seguidas, lo cual se logra escribiendo un **byte no-operación (`0xFF`)** después de cada comando.

**Nota**: el segundo latch no se utiliza (al menos no en Street Fighter II).

### Arquitectura del programa Z-80

El Z-80 no ejecuta ningún sistema operativo, pero gracias a su sistema de interrupciones aloja **dos hilos**:
- Un hilo encargado de leer los latches.
- Otro hilo que ejecuta toda la lógica de negocio.

Ambos hilos corren en lock-step gracias a sendos contadores: el hilo principal ejecuta un busy loop que solo se desbloquea cuando su contador local coincide con el contador del sistema de interrupciones.

- El **hilo de interrupción** almacena los valores entrantes del latch en un array e incrementa un cursor de escritura.
- El **hilo principal** lee el mismo array con un cursor de lectura.

El conjunto funciona como un **buffer circular**.

**Despacho de comandos**: el hilo principal despacha comandos leyendo byte a byte. Un valor **`0xF7`** indica que el siguiente byte es el ID de una música a reproducir. En cualquier otro caso, el byte actual es el ID de un efecto de sonido.

### Reproducción de efectos de sonido

Al determinar que un ID corresponde a un efecto de sonido, el Z-80 no lo reenvía directamente al OKI. En vez de eso, se usa una **tabla de traducción** donde se almacena no solo el ID real a usar, sino también el volumen y el canal. De los cuatro canales disponibles, la tabla siempre indica usar el 3 o el 4, por razones que se detallan más adelante. Esta tabla de traducción probablemente se implementó para que el equipo de programación de control tuviera IDs estables con los que trabajar, dando flexibilidad al equipo de sonido.

**Trivia**: la tabla de traducción almacena los datos en **big-endian** (volumen-canal, luego sample_ID), y el Z-80 invierte el orden de bytes al leerlos. Podría ser un artefacto de alguna herramienta que corría en una estación de trabajo con CPU big-endian, como el Sharp X68000.

Tras la consulta a la tabla, se entra en terreno bien documentado gracias al datasheet del MSM6295: el OKI está conectado directamente a su propia ROM de muestras mediante un bus local. La estructura de la ROM es sencilla: un índice de **127 entradas** que da el offset de inicio y fin de un stream ADPCM. El formato del stream también es conocido, aunque existe en una sorprendente cantidad de variantes.

### "Life of a punch" (vida de un puñetazo)

El primer efecto de sonido del juego es el puñetazo que Scott le da a Max. El 68000 escribe el valor **`0x2A`** en el latch, que se lee y traduce a la "frase" **`0x16`** en el canal **`0x4`** con volumen **`0x1`**. La tabla del OKI da un offset de payload ADPCM de **43,344**, longitud **2095 bytes**.

Dado que la estructura de la ROM del OKI está documentada, es sencillo explorarla. El tamaño máximo de ROM de **256 KiB** está casi completamente utilizado: hay **95 entradas** en el índice, totalizando **245,829 bytes**. Gracias al códec ADPCM, que usa **4 bits por muestra**, la duración total de los efectos de sonido de Street Fighter 2 asciende a **65 segundos**. La muestra más larga es la risa de Zangief (**`0x2A`**), que usa **2.000 ms** de voz.

La **frecuencia de muestreo** es de **7575 Hz**, bastante baja para hoy pero bastante alta para los estándares de 1989.

### Reproducción de música

Reproducir música es un proceso mucho más complejo. Mientras el OKI tiene su propio reloj y ROM en un bus local, requiriendo solo un ID para disparar la reproducción de una muestra, el YM2151 necesita ser alimentado constantemente con notas para cada instrumento a intervalos regulares.

El funcionamiento de síntesis del YM2151 y su estructura de registros son bien conocidos. El objetivo es pilotar **ocho canales ("voces")**, cada uno compuesto de **cuatro slots**, para un total de **32 slots**.

Para lograrlo, la CPS-1 usa un formato musical cercano en concepto a MIDI. Al recibir un ID de música a reproducir, el Z-80 crea y mantiene un cursor de tiempo gracias a su hilo de interrupción de **240 Hz**. En cada tick, los registros del YM2151 se alimentan mediante una tabla de instrumentos (mejor conocida como "sound font").

Una música es un stream de instrucciones que se traduce en un doble offset **[instrumento][nota]** en la tabla de sound font. El formato de instrucción está explicado mediante código por Ben Torkington (herramienta referenciada). Pero eso no es todo: escuchar solo la salida del YM2151 no reproduce el tema de "intro" tal como se conoce.

### Adorno musical (percusión)

Lo que falta al escuchar solo el YM2151 son las percusiones (tambores). Estas son mucho más difíciles de producir mediante síntesis FM, así que se reproducen vía el OKI. Estas reproducciones de muestra no son solicitadas por el 68000, por lo que no se escribe nada en los latches: son disparadas por el propio stream de instrucciones musicales que lee el Z-80.

Para evitar conflictos entre gameplay y música, el espacio de canales del OKI se divide de forma equitativa: esto explica por qué el gameplay solo usa los **canales 3 y 4**, mientras que los **canales 1 y 2** están reservados para el equipo de música.

**Trivia**: ¿cómo acelera Street Fighter la velocidad de reproducción cuando la situación de un asalto se vuelve crítica (salud baja de un contendiente)? No lo hace dinámicamente: son **pistas de música separadas, codificadas de forma fija (hard-coded)**.

### ¿Música o efecto de sonido?

Algunos efectos de audio parecen que deberían ser una muestra, pero en realidad se hacen con síntesis FM. El efecto de sonido que se escucha en la pantalla de selección de contendiente al mover el cursor **no es una muestra del OKI, sino un instrumento renderizado por el YM2151**. La razón (¿ROM del OKI llena? ¿otra razón de diseño?) queda sin respuesta documentada: "the answer my friend is blowin' in the wind".

### Fuentes citadas por el autor

- MAME history 1994 (`drivers/cps1.c`, historic-mame).
- Esquemas "Forgotten World CPS-1 schematics" (para ver las líneas de bus).
- Datasheet del MSM6295.
- Herramientas de ADPCM de Ian Karlsson (`github.com/fabiensanglard/adpcm`).
- Documentación de síntesis del YM2151.
- Datasheet del YM2151.
- Herramienta de extracción de música CPS-1 de Ben Torkington (`github.com/bentorkington/cps1-sound-extract`).
- Banda sonora de Street Fighter 2 (Spotify), citada como referencia de las pistas aceleradas hard-coded.

## Notas / gotchas

- El protocolo de latches es un ejemplo de diseño de comunicación entre CPUs sin canal de retorno (68000 solo escribe, Z-80 solo lee): la fiabilidad se logra completamente mediante temporización de interrupciones (VSYNC / temporizador del YM2151) y una convención de "byte nulo" (`0xFF`) en vez de handshake real.
- El reparto de canales OKI (1-2 música, 3-4 gameplay) es una convención de firmware, no una restricción física del chip.
- El "aceleramiento" de música con salud baja es una ilusión: no hay time-stretching, son pistas distintas pregrabadas.
- Persisten preguntas abiertas del propio autor (p. ej. por qué el sonido del cursor de selección usa FM en vez de muestra), documentadas como tales, no como hechos confirmados.

## Referencias cruzadas

- (pendiente de enlazar)

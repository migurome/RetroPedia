---
titulo: Another World en Atari ST - Rasterizado de polígonos por software sin Blitter
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, atari-st, motorola-68000, poligonos-2d, rasterizado-software, bresenham]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons_atariST/index.html]
---

# The polygons of Another World: Atari ST

## Resumen

Tercer artículo de la serie de Fabien Sanglard sobre los ports de Another World. Analiza el port a Atari ST, una máquina con la misma CPU (Motorola 68000) que el Amiga pero sin Blitter ni coprocesadores gráficos equivalentes a Agnus/Denise, y con un framebuffer de layout distinto ("woven"/entrelazado). Es relevante para diseño de motores porque muestra cómo reimplementar por software (sin hardware dedicado) las mismas operaciones DRAW/FILL/COPY que en Amiga resolvía el Blitter, y cómo el propio layout de memoria "hostil" para sprites resulta ser una ventaja para el caso de uso específico del motor (líneas horizontales largas de un solo color, típicas del relleno de polígonos).

## Detalle técnico

### Historia del Atari ST

La génesis del Atari ST es como mínimo tumultuosa: empieza como un cuento de hadas y termina en guerra y traiciones que involucran a Amiga Corp, Commodore, Atari y la división de computadoras de Atari.

En 1983, Hi-Toro era una startup con sede en Santa Clara que construía una videoconsola llamada "Lorraine". Con un potente 68000 y capacidades gráficas impresionantes, era una maravilla tecnológica muy superior a toda la competencia. Sin embargo, Hi-Toro se estaba quedando sin efectivo. Atari, entonces gigante de los videojuegos, inyectó 500.000 dólares para mantener el desarrollo a cambio del derecho a añadirle un teclado y venderla como ordenador. Todos ganaban.

Lejos de California, en Pensilvania, una lucha de poder en Commodore resultó en la salida de su fundador, Jack Tramiel. Este formó inmediatamente Tramel Technology, reclutando ingenieros de Commodore en el proceso. La separación no fue nada amistosa.

A mediados de 1984 le tocó a Atari pasar por dificultades financieras. Cuando Jack Tramiel expresó interés en comprar la división de computadoras de Atari, Commodore vio la oportunidad de actuar contra su ex-CEO: se hicieron con la tecnología comprando Hi-Toro (que para entonces se había renombrado como Amiga Corporation) y sólo reembolsaron a Atari su "préstamo".

Tramiel completó la adquisición de Atari Computers y demandó inmediatamente a Amiga Corporation. El asunto no se resolvería hasta marzo de 1987. Mientras tanto, Atari se encontró sin nueva tecnología para volver a ser rentable.

Durante un tiempo, Jack Tramiel consideró comprar la Genesis a Sega, que buscaba un socio con experiencia en Norteamérica. Finalmente la idea se rechazó. Tramiel mantuvo sólo 900 de los 10.000 empleados de Atari. Los supervivientes se embarcaron en una marcha forzada con el objetivo de diseñar y lanzar una nueva máquina en un año.

Sorprendentemente, lo consiguieron. El Atari ST se lanzó en junio de 1985, adelantándose un mes al Amiga 1000.

La máquina fue bien recibida por los clientes. En noviembre de 1985, Jack Tramiel anunció que Atari había vendido 50.000 unidades. El precio fue una hazaña particular: un 520ST en color con 512 KiB de RAM se vendía por 999 $, mientras que un Amiga 1000 con 256 KiB de RAM costaba 1.285 $.

Los consumidores de la época podían elegir entre un Amiga o una máquina 300 $ más barata, con más RAM, pero con "sólo" 16 colores. No hace falta decir que muchos optaron por un ST.

Más allá de su principal argumento de venta, la máquina tenía otras ventajas notables sobre la competencia: el modo monocromo de alta resolución 640x400 era una maravilla para los programadores; la interfaz MIDI integrada era apreciada por los músicos; y un detalle pequeño pero importante: el sistema operativo (TOS) y el kernel estaban en ROM, así que no hacía falta disquete para arrancar.

El Atari ST reunió rápidamente un culto de seguidores, particularmente en Europa. La devoción por el ST fue tal que los creadores de demos desarrollaron obsesivamente técnicas de overscan para eliminar los bordes negros de la pantalla, y finalmente lo consiguieron.

A lo largo de su vida, de 1985 a 1993, Atari Computers vendió 2 millones de unidades. Para muchos que vivieron esa época, se recuerda como la infame guerra Amiga contra Atari ST.

### Atari STE

Desde el punto de vista de CG, animación y videojuegos, sin embargo, no hubo mucha guerra, ni siquiera una batalla. Con sus coprocesadores Agnus y Denise, el Amiga era muy superior al Atari ST. Como ejemplo, el layout de memoria del framebuffer del Atari ST era desesperante a la hora de mover un sprite por la pantalla.

Para paliar sus carencias, Atari Computers renovó la línea Atari ST en 1989 con los 520 STE y 1040 STE. Entre las mejoras: Genlock, colores de 12 bits, audio PCM y, sobre todo, un **BLiTTER** que reforzaba al Atari ST con tecnología capaz de rivalizar con el Amiga.

### Arquitectura

Las capacidades del Blitter del STE habrían sido muy valiosas para un programador de juegos. Desgraciadamente llegó demasiado tarde. Había una base enorme de usuarios de Atari ST (sin STE) que disuadía a los desarrolladores de invertir tiempo en mejorar el juego sólo para una minoría de usuarios. Nadie se atrevía a lanzar un título "sólo para STE". Another World no es una excepción, según confirma el desarrollador de la versión Jaguar (que vio el código fuente de la versión Atari ST):

> Another World was entirely programmed to run on the 68000. The capabilities of the STE were not used at all.
> — Sébastien Briais

Hay un controlador DMA en todos los Atari ST, pero sólo puede transferir de disquete/HDD a RAM, lo cual no ayuda mucho a la programación de juegos. Esto dejaba al programador con muy poco: el procesador Motorola 68000 a 8 MHz y 512 KiB de RAM (el artículo ilustra la arquitectura resultante con la frase "Good luck Jim" como pie de un diagrama de bloques).

### Sistema de vídeo

El chip GLUE, encargado de generar la señal de vídeo, usa 3 bits por canal (9 bits por píxel), resultando en 512 colores posibles (espacio de color de 9 bits por píxel).

**Trivia:** manipulando el Shifter, el GLUE y la MMU, los creadores de demos consiguieron eliminar los infames bordes negros de pantalla del Atari. Las técnicas de "overscan" y "fullscreen" cambian la frecuencia de refresco entre 50Hz y 60Hz y también pasan de alta a baja resolución antes de que termine una scanline, para engañar al GLUE y retrasar las señales VSYNC/HSYNC.

### Another World en Atari ST

Another World en Atari ST fue programado enteramente por Eric Chahi en paralelo a la versión de Amiga. La CPU 68000, compartida con el Amiga, facilitó el port porque gran parte del ensamblador de la VM era reutilizable. Las cosas difieren en el apartado gráfico, donde hay tres modos disponibles:
- **Alta resolución:** 640x400, 2 colores (excelente para programar, pero poco útil aquí)
- **Resolución media:** 640x200, 4 colores (no muy interesante en este contexto)
- **Baja resolución:** 320x200, 16 colores — encaje perfecto para Another World

Todas las resoluciones usan 32 KiB por framebuffer. Aquí no hay bitplanes como en el Amiga: en alta resolución, cada bit corresponde a un píxel; en baja resolución, los índices de píxel están "entretejidos" (woven) en 4 bits repartidos a lo largo de intervalos de 64 bits.

### Woven Hell (el infierno del entrelazado)

Para un juego que usa sprites, el layout entrelazado de baja resolución era una dificultad seria. Hay que imaginar el ejercicio de enmascarar/escribir con resolución de 1 bit cuando la resolución del bus es de un byte (8 bits). Ahora imagina un sprite cruzando un límite de byte: la tarea se convierte en una pesadilla. Era, de hecho, tan intensivo en CPU que los juegos evitaban las operaciones de máscara/escritura y en su lugar preferían usar sprites pre-rotados (un juego de sprites por cada alineación de bit posible), lo cual, para crédito del Atari ST, no era demasiado problema dada la generosa cantidad de RAM disponible.

Curiosamente, para Another World, el infierno del layout entrelazado se convierte en un paraíso a la hora de escribir líneas largas de un solo color.

### Resolviendo FILL y COPY

Con sólo una CPU para realizar FILL y COPY, estas tareas se hacen de la forma más simple posible. No hay ningún truco elegante aquí, pero el layout no planar del framebuffer permite que el 68000 "vaya de crucero" (cruise). Con un framebuffer de 32 KiB, 8.000 operaciones `move.l` (32 bits) largas hacen el trabajo tanto para COPY como para FILL.

La salida en desensamblado de la parte 68000 de COPY es la siguiente:

```
    ...
    move.w   #49,d0        ;50*160*4 = 32000 octets

.copy_loop:   
    rept     160       
    move.l   (a1)+,(a0)+
    endr

    dbra     d0,.copy_loop
    ...
```

Como los tiempos del 68000 están muy bien documentados, se puede calcular exactamente cuánto tarda una operación COPY de framebuffer. Este tipo de `move.l` tarda 20 ciclos en ejecutarse, por lo que el número total de ciclos requeridos es 20 × 160 × 50 = 160.000 ciclos. A 8.000.000 de ciclos/segundo, **COPY tardaba 20 ms** en completarse.

La misma idea aplica para FILL, salvo que en vez del operando "Address register indirect with post-incrementing", el primer operando de la instrucción `move.l` es un operando "Data direct", que sólo tarda 12 ciclos. Esto da un total de **12 ms para FILL**:

```
    ...
    move.w   #49,d0        ;50*160*4 = 32000 octets
    
.fill_loop: 
    rept     160  
    ; d1 = color repeated 8 times since d1=32 bits and color index=4  bits)
    move.l d1,(a0)+
    endr

    dbra     d0,.fill_loop
    ...
```

**Trivia:** las versiones de Atari ST y Amiga se pueden distinguir fácilmente del resto de ports. Al principio del juego, Lester se encuentra con un escarabajo inofensivo que desaparece a partir de la versión de PC; estos ports posteriores tenían más niveles, lo que obligó a recortar la animación del escarabajo.

### Resolviendo el problema de DRAW

Sin un "line drawing mode" ni un "area filling mode" de hardware, el port de Atari ST usa un algoritmo de Bresenham sencillo para seguir los bordes del polígono. Con dos bordes, se generan coordenadas de inicio y fin en espacio de pantalla para líneas horizontales; después el 68000 rellena el espacio entre ellas. El conjunto funciona exactamente igual que el Blitter de Amiga, salvo que aquí todo se hace por software.

Puede parecer muy lento, pero también aquí se puede alcanzar la "velocidad de crucero" aprovechando el layout entrelazado. Si se observa de cerca, la mayoría de los frames de Another World están compuestos de líneas horizontales largas de un mismo color. Esto permite al 68000 escribir líneas en bloques de 16 píxeles usando las instrucciones `move.l` vistas antes.

La pantalla donde Lester conoce a su "salvador" es un buen ejemplo: se muestran en rojo las partes de las líneas que no se pueden renderizar en bloques de 16 píxeles debido al layout del framebuffer y a problemas de alineación. Estos píxeles "lentos" sólo representan el **23%** del framebuffer, en los bordes de los polígonos. El **77%** del framebuffer se renderiza con escrituras "rápidas" en bloques de 16 píxeles.

### Resolviendo el problema de la paleta

El último problema a resolver está relacionado con la escasa profundidad de color del Atari ST. Mientras que el Amiga permite 12 bits por color, el Atari ST sólo puede con 9 bits por color.

La solución fue distribuir el juego con las paletas del Amiga y dejar que el 68000 se encargara de convertir de 12 bits a 9 bits con un simple desplazamiento lógico a la derecha (LSR) de 1 bit por cada canal, en tiempo de carga.

El resultado es un poco más oscuro comparado con el Amiga, pero en general apenas perceptible.

### Veredicto

A pesar de claras desventajas, el Atari ST consiguió ejecutar una versión fiel de Another World. Contra todo pronóstico, el juego no tiene nada que envidiar comparado con el Amiga. El espacio de color más reducido apenas se notaba y el motor sólo rendía ligeramente más lento que el Amiga (que corría cerca de 24 fps). Chapeau bas.

## Notas / gotchas

- La falta de Blitter/Agnus/Denise obliga a resolver en software (Bresenham) lo que en Amiga resolvía hardware dedicado; el port conserva la misma CPU (68000) pero pierde toda la aceleración de dibujo de polígonos y copiado de framebuffer.
- El layout de framebuffer "woven"/entrelazado (4 bits de índice de píxel repartidos en palabras de 64 bits) es pésimo para sprites con máscara (obliga a usar juegos de sprites pre-rotados, uno por alineación de bit, a costa de RAM) pero resulta ser una ventaja para el caso de uso dominante del motor: líneas horizontales largas de color uniforme, que es exactamente lo que produce el relleno de polígonos.
- Aun con el layout favorable, sólo el 77% de los píxeles de una escena típica se benefician de la escritura rápida en bloques de 16 píxeles; el 23% restante (los bordes de los polígonos, donde la alineación de bits no coincide con los límites de 16 píxeles) requiere manejo lento, píxel a píxel.
- La conversión de paleta de 12 a 9 bits por canal mediante un simple LSR de 1 bit es una decisión pragmática: en vez de mantener dos juegos de paletas por plataforma, se reutiliza el asset del Amiga y se convierte en tiempo de carga, aceptando una ligera pérdida de fidelidad de color a cambio de no duplicar datos.
- La falta de STE-only features (Blitter STE, etc.) fue una decisión de negocio, no técnica: la base instalada de Atari ST "normales" (no STE) era demasiado grande para justificar una versión que sólo se beneficiara de hardware exclusivo de una minoría de usuarios.
- El rendimiento final (ligeramente por debajo de los ~24 fps del Amiga) demuestra que un enfoque 100% software, bien adaptado al layout de memoria disponible, puede acercarse al rendimiento de una solución con coprocesador dedicado si se explota a fondo el patrón de acceso dominante (líneas horizontales) en vez de optimizar el caso general.

## Referencias cruzadas

- [another-world-overview.md](another-world-overview.md) (arquitectura general de la VM y opcodes)
- [another-world-amiga.md](another-world-amiga.md) (port a Amiga 500, con Blitter dedicado para dibujo de polígonos)
- (pendiente de enlazar)

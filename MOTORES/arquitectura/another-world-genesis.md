---
titulo: Los polígonos de Another World — el port a Sega Genesis / Mega Drive
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, genesis, megadrive, vdp, polígonos-2d, dma]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons_Genesis/index.html]
---

# Los polígonos de Another World — el port a Sega Genesis / Mega Drive

## Resumen

Analiza cómo se adaptó el motor de polígonos 2D vectoriales de Another World al Sega Genesis/Mega Drive, cuyo VDP **no soporta relleno de polígonos arbitrario de forma nativa**. Es la entrada más relevante de esta serie para motor-cenital: documenta por qué el enfoque "ideal" (usar DMA FILL/DMA COPY del VDP como si fuera el blitter del Amiga) resultó inviable, y cómo el port acabó recayendo en un pipeline de rasterizado por CPU + framebuffer en RAM + DMA de blit final a VRAM, con una sola capa de scroll activa y doble buffer por conmutación de origen de dicha capa. El autor lo califica como "sin duda el port más desafiante estudiado hasta ahora."

## Detalle técnico

### Arquitectura de hardware del Genesis

**CPU y memoria:**
- Motorola 68000 a 7.61 MHz
- Cartuchos ROM de hasta 8 MiB
- 64 KiB de RAM
- 64 KiB de VRAM
- Z80 a 3.58 MHz con 8 KiB dedicados a audio

**Sistema de vídeo:**
El VDP (Video Display Processor), a 13 MHz, gestiona todos los gráficos. El 68000 no puede direccionar la VRAM directamente; se comunica mediante registros mapeados en RAM. La transferencia de datos habitualmente usa el controlador DMA del VDP.

**Comportamiento del DMA:** "El 68000 se detiene durante una transferencia DMA del VDP, pero el Z80 puede seguir operando mientras no acceda a la RAM." "El DMA es bastante rápido durante el BLANK, operando al doble de velocidad que el bucle más ajustado posible del 68000."

**Sistema de color:** 3 bits por canal, 9 bits totales por color (512 colores posibles en total). El VDP usa cuatro capas: Scroll B, Scroll A, Windows y Sprite.

### Capacidades y límites del VDP

**Sistema de capas:** todas las capas usan índices de 4 bits sobre paletas de capa (15 colores por capa, con el índice 0 siempre transparente). Las capas soportan renderizado por prioridad, sin soporte de blending — solo transparencia vía índice 0.

**Modos de resolución:**
- H32: 32×28 celdas (256×224 píxeles)
- H40: 40×28 celdas (320×224 píxeles)

**NTSC vs PAL:**
- NTSC: 486 líneas visibles, 28 celdas horizontales
- PAL: 576 líneas visibles, 30 celdas horizontales
- VBLANK de PAL: 20ms (50Hz); NTSC: 16ms (60Hz)

Los primeros juegos asumían un timing constante de 16ms, lo que causaba un 17% de ralentización en hardware PAL.

### El diseño que no pudo ser: por qué el VDP no sirvió para rasterizar polígonos

El artículo explora en detalle por qué las capacidades nativas del VDP no pudieron aprovecharse para el renderizado de polígonos.

**Enfoque teórico considerado:** la capacidad DMA FILL del VDP podría, en teoría, dibujar polígonos de forma análoga al Blitter del Amiga. DMA COPY podría implementar el opcode COPY de la VM del motor.

**Los problemas que lo hicieron inviable:**

1. **Capacidad de VRAM:** alojar cuatro framebuffers de 320×200 requiere más de los 64 KiB de VRAM disponibles.

2. **Problema de granularidad de nibble:** "las operaciones de escritura de DMA FILL se hacen con granularidad de 16 bits, lo cual es rápido, pero cuando se necesitaban nibbles individuales, leer de VRAM con los registros del VDP, enmascarar y volver a escribir habría hundido el framerate."

3. **Incompatibilidad de la disposición de celdas:** "Another World necesita escribir líneas largas de píxeles, pero las líneas de celda se dividen en dos words (4 bytes) con un stride de 28 bytes. Como DMA FILL no tiene stride, esto habría requerido muchas peticiones, y el overhead habría matado el framerate."

**Verificación empírica:** el trazado del registro #17 del VDP en el emulador Exodus confirmó que DMA COPY y DMA FILL **nunca se usan** en el port final.

### Arquitectura real usada en el Genesis

**Renderizado por CPU:**
El port usa un enfoque intensivo en 68000, similar al de la versión de Atari ST:
- Tres framebuffers (dos buffers BKGD más uno de trabajo) mantenidos en RAM
- Todas las operaciones de renderizado y copia las ejecuta el 68000
- Los frames completados se copian por DMA COPY al doble buffer libre en VRAM
- En el siguiente VBLANK, se "voltea" el buffer de VRAM activo

**Configuración del VDP:**
- Todas las capas deshabilitadas salvo Scroll A
- El origen de Scroll A se modifica según qué doble buffer se escribió por última vez

**Optimización respecto a Atari ST:** la única optimización señalada es la implementación de COPY usando la instrucción `movem`, con una ganancia de rendimiento estimada (no confirmada) de un 5%.

### Estrategia de resolución y framebuffer

**Resolución objetivo:** en vez de la nativa 320×200, la versión de Genesis usa el modo H32 (base 256×224). Algunas columnas y filas usan "Black Cells" que nunca se vuelven a actualizar. Área de dibujo activa: 28×22 celdas = 224×176 píxeles.

**Tamaño del framebuffer:** 224 × 176 / 2 = 19.712 bytes (almacenado en RAM, a 4 bits por píxel).

**Análisis de ancho de banda DMA:**

```
Frecuencia    Resolución    En VBLANK    En Display Activo    Total
60 Hz         320 x 224     7524         4032                 11556
50 Hz         320 x 224     17622        4032                 21654
```

**Cálculos de framerate:**

NTSC: 19.712 / 9702 = se pierden 2 frames (solo por la transferencia DMA). Añadiendo los opcodes de draw/copy/fill: 15-12 fps.

PAL: 19.712 / 17.913 = 1.1 frames. Con procesamiento adicional: 16-12 fps estimados.

**Relación de aspecto:** la resolución reducida preserva la relación de aspecto prevista de Another World de 1.3:1.

### El problema de la paleta de 16 colores

**El problema:** Another World necesita 16 colores por capa, pero el VDP trata el índice de color 0 como transparente, reduciendo los colores disponibles a 15.

**Hipótesis inicial del autor:** rellenar el Plano B con celdas repetidas fijadas al índice de color 0x01, copiar el color de paleta 0 en el índice 0x01 del Plano B, dejando que el Plano B (detrás del Plano A) aporte el 16º color cuando el Plano A use el índice 0.

**Solución real encontrada:** no hizo falta ningún workaround especial. Todas las paletas usan el mismo NEGRO en el índice 0. Configurar el registro BACKGROUND COLOR a negro proporcionó el 16º color necesario.

### Comparación de rendimiento y resolución

**Genesis (224×176) vs Amiga (320×200):** la reducción de resolución no es dramática. El artículo señala "rasgos faciales añadidos" en la versión de Amiga (Lester parece "más preocupado"), con un "bug de clipping en la parte inferior (la mano derecha de Lester)". La diferencia de profundidad de color es perceptible pero no problemática.

**Compromisos visuales:**
- Los bordes de Black Cells reducen la resolución y añaden bordes visibles en pantalla
- En NTSC: menos perceptible por el overscan natural
- En PAL: se agrava por los bordes negros naturales adicionales
- Renderizado de la luna: "al contrario que en la versión de PC, esta vez la luna es redonda!"

### Créditos de desarrollo

- **Port a Genesis:** Michael Burton (Interplay)
- **Otras plataformas:** Rebecca Heineman (Macintosh, SNES, Apple IIGS)
- La versión de Genesis fue el primer port de Another World no realizado por Delphine Software
- La base en 68000 permitió reutilizar código de la versión de Atari ST

### Nomenclatura regional

- Norteamérica/Canadá: "Out Of This World" (para evitar confusión con la serie de TV homónima)
- Europa: "Another World"
- Irónicamente, una serie de TV llamada "Out Of This World" se estrenó en 1991, después del lanzamiento norteamericano del juego

### Presupuesto de rendimiento

El artículo aporta tablas de framerate mostrando los VSYNCs perdidos:

```
VSYNCs perdidos:    0    1    2    3    4    5    6    7
Framerate NTSC :   60   30   20   15   12   10    8    7
Framerate PAL  :   50   25   16   12   10    8    7    6
```

La versión de Genesis corre entre 12-15 fps en NTSC, con mejor rendimiento en PAL gracias a los periodos de VBLANK más largos que permiten un DMA más rápido.

### Qué no se pudo hacer y qué sí se implementó

**Lo que no se pudo hacer:**
- Renderizado en tiempo real de un framebuffer completo a 320×200
- Rasterizado de polígonos acelerado por hardware del VDP
- Actualizaciones de pantalla completa por frame sin penalización de framerate

**Lo que sí se implementó:**
- Rasterizado de polígonos por CPU hacia un framebuffer en RAM
- Blit del framebuffer a VRAM mediante DMA
- Doble buffer mediante conmutación del plano Scroll A
- Resolución reducida (224×176) para minimizar los requisitos de ancho de banda
- Uso estratégico de celdas negras para preservar la relación de aspecto

**Restricciones de desarrollo:** el artículo especula: "quizás se podría haber hecho algo con un sistema de caché de celdas para transferir solo las 'sucias' y reducir el volumen de datos transferidos, pero es probable que al desarrollador no le dieran mucho tiempo para pulir el port."

### Veredicto del autor

El port se caracteriza como "sin duda el port más desafiante estudiado hasta ahora." A pesar de la CPU potente y el VDP del Genesis, el cuello de botella de hardware fue el ancho de banda de transferencia RAM/VRAM, nunca diseñado para actualizaciones de pantalla completa. La resolución reducida y los bordes negros fueron compromisos aceptables. El artículo señala que es probable que los jugadores no notaran los problemas de framerate, estableciendo un paralelismo con StarFox en SNES, que tuvo compromisos similares de framerate/bordes y sigue siendo bien valorado.

## Notas / gotchas

- El VDP de Mega Drive **no tiene un modo de relleno de polígonos arbitrario utilizable en la práctica**: DMA FILL solo escribe con granularidad de 16 bits (no nibble) y no soporta stride, lo cual es incompatible con la disposición de celdas (líneas partidas en 2 words con stride de 28 bytes) que necesitaría el motor para trazar líneas largas de píxeles. Esto se verificó empíricamente trazando el registro #17 del VDP en el emulador Exodus: el port final no usa nunca DMA FILL ni DMA COPY para dibujar.
- La solución fue "hacer trampa": renderizar por software (CPU 68000) en un framebuffer en RAM, y usar el VDP solo como destino final de un blit DMA — el mismo patrón que la versión de Atari ST, no una solución "nativa" de Genesis.
- El presupuesto de ancho de banda DMA es el cuello de botella real, no la CPU ni el VDP en sí: transferir un framebuffer de ~19.7KB tarda más de un VBLANK completo en NTSC, forzando 12-15 fps incluso con la CPU y el VDP infrautilizados para otras tareas.
- La reducción de resolución a 224×176 (usando el modo H32 de 256×224 con "celdas negras" de relleno) es una solución de compromiso para bajar el volumen de datos a transferir por DMA, no una limitación de la propia resolución del VDP.
- El "problema" del 16º color (índice 0 transparente) resultó no ser un problema real en este juego concreto porque todas las paletas comparten negro en el índice 0 — pero el razonamiento general (usar el registro BACKGROUND COLOR o robar un plano B) es aplicable a otros juegos donde el índice 0 de cada paleta no coincida.
- PAL sale beneficiado frente a NTSC en este caso concreto porque el VBLANK más largo (20ms vs 16ms) da más margen para completar el DMA, a pesar de que el mismo framebuffer PAL antes sufría un 17% de ralentización en otros juegos que asumían timing NTSC fijo.
- Es el port de esta serie donde el autor concluye con más rotundidad que la limitación no fue la CPU (68000 "potente" para la época) sino el ancho de banda de memoria/VRAM, un punto de comparación directo para cualquier motor de Mega Drive que planee renderizado por software con blit final vía DMA.

## Referencias cruzadas

- MOTORES/arquitectura/another-world-pc-dos.md
- MOTORES/arquitectura/another-world-snes.md
- MOTORES/arquitectura/another-world-gba.md
- MOTORES/arquitectura/another-world-jaguar.md
- (pendiente de enlazar con MegaDrive/graficos/dma-transfer.md y MegaDrive/graficos/vdp-registers.md — mismo hardware, mismas limitaciones de DMA)

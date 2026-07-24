---
titulo: Cómo funciona el sistema gráfico de la SNES (PPU1/PPU2)
plataforma: [General]
categoria: [graficos-hardware]
tags: [snes, ppu, vram, sram, oam, cgram, ingenieria-inversa]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/snes_ppus_how/index.html]
---

# Cómo funciona el sistema gráfico de la SNES (PPU1/PPU2)

## Resumen

Segunda entrada de la serie de Fabien Sanglard sobre el sistema gráfico de SNES. Mientras la primera entrada ("por qué") explica la restricción de latencia de VRAM que motiva todo el diseño, esta se centra en el **cómo**: la arquitectura física real de los dos chips PPU, basada en análisis de esquemáticos y en el decapado (decapping) del die por parte de un investigador externo. Es la entrada más orientada a "ingeniería inversa de hardware" de las tres.

## Detalle técnico

### Primer contacto: los cuatro chips del sistema gráfico

El sistema gráfico de la SNES está formado por cuatro chips: **PPU1** (etiquetado 5C77), **PPU2** (etiquetado 5C78), y dos chips SRAM de 8 bits × 32 KiB cada uno.

El artículo señala que en revisiones posteriores de hardware, Nintendo fusionó la CPU, la PPU1 y la PPU2 en un único ASIC masivo (las versiones conocidas como "1-CHIP"). Estas versiones eran menos propensas a fallos, pero venían con sus propios problemas.

### Arquitectura: análisis de esquemáticos

A partir del análisis de los esquemáticos de la placa, el artículo identifica varias líneas de señal clave:

- **Generación de señal de vídeo**: las líneas R, G, A y CSYNC salen de la PPU2, lo que indica que la señal de vídeo se genera ahí. Es probable que las paletas también se almacenen en ese chip.
- **Direccionamiento de VRAM**: la PPU1 tiene acceso a las líneas de dirección de la VRAM, mientras que la PPU2 solo tiene acceso a las líneas de datos. Esto indica que la PPU1 actúa como el "cerebro" (decide qué leer) mientras que la PPU2 hace el trabajo pesado de composición.
- **Streaming de datos**: además de líneas de control entre PPU1 y PPU2, hay líneas de color que sugieren que la PPU1 va transmitiendo ("streaming") píxeles y sincronizándose con la PPU2.
- **Acceso desde el bus de sistema**: el resto del sistema no tiene acceso directo a la VRAM. Todas las operaciones de escritura desde el bus de sistema hacia la VRAM deben pasar obligatoriamente por la PPU1.

El artículo señala un paralelismo de diseño explícito: este esquema — un chip que decide direcciones (vía líneas de dirección) y un chip compositor que simplemente muestra lo que llega por las líneas de datos — es similar a cómo Capcom diseñó su placa arcade CPS-1.

### Los tres sistemas de RAM del subsistema gráfico

- **VRAM**: donde se almacenan los assets de sprites, los assets de tiles y los tilemaps.
- **OAM (Object Attribute Memory)**: define las propiedades de los 128 sprites del sistema.
- **CGRAM**: donde se almacena la paleta de 256 colores de 15 bits.

### SRAM 101: el pinout de los chips de memoria

Los chips SRAM usados son del tipo LH5P832, con el pinout estándar:

- VCC (alimentación), GND (tierra)
- A0–A14: líneas de dirección (2^15 = 32.768 = 32 KiB direccionables)
- D0–D7: bus de datos de 8 bits
- CS: Chip Select
- WE: Write Enable
- OE: Output Enable

El artículo plantea las configuraciones posibles para combinar dos de estos chips:

1. **Modo serie**: dos chips con la línea A15 actuando como selector de chip → bus de datos efectivo de 8 bits.
2. **Modo entrelazado (interleaved)**: bus de datos efectivo de 16 bits.
3. **El diseño real de Nintendo**: un esquema donde los chips de RAM pueden direccionarse *individualmente*, pero **comparten el pin A14**. El artículo lo plantea como una pregunta abierta de diseño: "¿qué está pasando aquí?" — es decir, por qué compartir A14 en concreto tiene sentido dado el resto de restricciones del sistema.

### Cómo se accede a la VRAM según el modo gráfico

El motivo de este direccionamiento inusual (chips individualmente direccionables pero con A14 compartido) se explica por la diferencia entre los modos gráficos:

- **Modos 0 a 6**: la PPU1 siempre accede a dos valores de 8 bits consecutivos. Esto significa que ambos buses de dirección mantienen el mismo valor en cada chip — de ahí que compartir A14 no suponga ninguna pérdida de flexibilidad en estos modos.
- **Modo 7**: dado que en este modo el fondo puede rotarse y escalarse, el acceso a los datos ya **no es lineal**. Como la PPU1 ya no puede "predecir" dónde estarán los datos a acceder y recuperarlos en bloques de 16 bits, necesita la flexibilidad de tener **dos buses de dirección independientes**.
- **Restricción del Modo 7**: en este modo solo se usan 16 KiB de cada chip de VRAM, lo que significa que la línea A14 se mantiene siempre a 0.

### Dentro de la PPU1 y la PPU2: análisis del die decapado

El artículo cita un análisis del die decapado (decapped) realizado por John McMaster en octubre de 2020, que identificó cinco áreas funcionales dentro de los chips:

- 512 bytes de OAM (áreas 1 y 2): describe las propiedades de los sprites.
- 256 × 9 bits de line buffer (áreas 5 y 6): el componente de line-buffering usado para componer líneas de sprites/fondo.
- 480 bytes de área de paleta de color (área 15).

El propio Sanglard señala una discrepancia sin resolver: "algunas de estas estimaciones parecen no cuadrar (¿no debería la CGRAM tener 512 bytes?)" y comenta que, al investigar el tema, le sorprendió lo poco que se conoce públicamente de los internos de las PPUs de la SNES — es decir, esta parte del hardware sigue siendo, a día de la publicación del artículo, territorio de ingeniería inversa incompleta.

## Notas / gotchas

- Esta entrada es más "arqueología de hardware" que "manual de programación": no cubre registros de programación ($21xx), profundidades de bits por modo, HDMA/DMA, ni reglas de prioridad — ese contenido corresponde más bien a documentación de referencia tipo wiki (nesdev/SNESdev), no al enfoque de este artículo.
- El dato más reutilizable para el diseño de un motor es el patrón arquitectónico: un chip "cerebro" con acceso a direcciones (PPU1) y un chip "compositor" ciego a direcciones que solo consume datos (PPU2) — el mismo patrón que Capcom usó en CPS-1. Es un patrón general de separación de responsabilidades en hardware gráfico de la época, no exclusivo de SNES.
- La necesidad de dos buses de dirección independientes solo en Modo 7 (por la falta de linealidad del acceso al rotar/escalar) es un buen ejemplo de cómo una característica de software (rotación/escalado) impone una restricción de hardware (direccionamiento no predictivo) que no existe en los modos "normales".
- El propio autor deja constancia de que parte de la información sobre los internos físicos de las PPUs no está confirmada ni es de dominio público completo — para tomar estos datos (especialmente los tamaños de área del die) como aproximaciones, no como hoja de datos oficial.

## Referencias cruzadas

- (pendiente de enlazar)

---
titulo: Los polígonos de Another World — el port a Super Nintendo (SNES)
plataforma: [General]
categoria: [arquitectura]
tags: [another-world, snes, ppu, polígonos-2d, dma, modo-1]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/another_world_polygons_SNES/index.html]
---

# Los polígonos de Another World — el port a Super Nintendo (SNES)

## Resumen

Analiza el port de Rebecca Heineman (Interplay, ~1992) del motor de polígonos 2D de Another World a la SNES, una máquina con una CPU (Ricoh 5A22, "un 6502 con esteroides") mucho menos potente que el 68000 del Genesis pero con un PPU muy superior en color (15 bits por píxel con blending RGB). Es la comparación más directa y mejor documentada contra el port de Genesis: mismo motor, mismo problema de fondo (rasterizar polígonos sin blitter dedicado para ello), decisiones de compromiso distintas — y una serie de intentos fallidos con chips de expansión (Super-FX, RAM estática) rechazados por coste, resueltos finalmente con un truco de registros hardware no documentado.

## Detalle técnico

### Contexto histórico de la consola

La Super Famicom se lanzó en Japón el 21 de noviembre de 1990, con 300.000 unidades agotadas en horas (se requirió intervención gubernamental para prevenir disturbios en lanzamientos de consolas futuros). La SNES llegó a Norteamérica el 9 de septiembre de 1991 y a Europa en abril de 1992. Norteamérica vio 721 juegos publicados a lo largo de la vida de la consola. Con casi 50 millones de unidades vendidas en nueve años (1991-1999), es una de las consolas más exitosas de la historia.

### Especificaciones de la CPU

La SNES se apartó del Motorola 68000 o el x86 de Intel usados por otras plataformas de la serie. La CPU era el Ricoh 5A22, esencialmente "un 6502 con esteroides".

**Mejoras incluidas:**
- Registros de 16 bits
- Unidad DMA
- Bus-A de direcciones de 24 bits
- Bus-B de direcciones de 8 bits, único (ambos buses se usan para declarar origen/destino de DMA)
- Capacidad de generar interrupciones VBLANK y HBLANK

**Características de rendimiento:**
- 128 KiB de RAM disponible
- Ricoh funcionando a 3.58MHz
- Bus de datos de 8 bits
- Significativamente menos potente que el Genesis

El autor señala que estas no fueron malas decisiones de diseño, ya que los chips de vídeo (S-PPU) y audio (S-DSP) aportaban la verdadera potencia del sistema.

### Sistema de vídeo: el S-PPU

El S-PPU es el componente de vídeo principal — "una potencia dedicada a sprites" (a sprite powerhouse) con especificaciones notables:
- 15 bits de profundidad de color por píxel
- Blending RGB entre capas

El PPU se compone de dos chips:
- **PPU-1:** determina la fuente de muestreo desde VRAM
- **PPU-2:** convierte los bitplanes de VRAM a píxeles

**Resoluciones de vídeo estándar:**

NTSC: normal 256×224; alta resolución (entrelazado) 512×448
PAL: estándar 256×239; alta resolución (entrelazado) 512×478

Los desarrolladores rara vez implementaban optimizaciones específicas para PAL, usando a menudo la resolución NTSC en monitores PAL.

### Capas de fondo y modos gráficos

El sistema gira en torno a "capas, aquí llamadas 'backgrounds'". Distintos modos ofrecen combinaciones diferentes de color y número de fondos:

| Modo | Colores BG1 | Colores BG2 | Colores BG3 | Colores BG4 | Res. NTSC | Res. PAL |
|------|------------|------------|------------|------------|-----------|----------|
| 0    | 4          | 4          | 4          | 4          | 256x224   | 256x239  |
| 1    | 16         | 16         | 4          | —          | 256x224   | 256x239  |
| 2    | 16         | 16         | —          | —          | 256x224   | 256x239  |
| 3    | 256        | 16         | —          | —          | 256x224   | 256x239  |
| 4    | 256        | 4          | —          | —          | 256x224   | 256x239  |
| 5    | 16         | 4          | —          | —          | 512x448   | 512x478  |
| 6    | 16         | —          | —          | —          | 512x448   | 512x478  |
| 7    | 256        | —          | —          | —          | 256x224   | 256x239  |
| 7EXT | 256        | 128        | —          | —          | 256x224   | 256x239  |

### Estructura y disposición de tiles

Los backgrounds usan tiles de 8x8 (ocasionalmente 16x16). Los datos de tile siguen una organización planar por bitplanes:

**DISPOSICIÓN DE TILE 4BPP DE SNES:**
```
      BYTE                BYTE
Línea 0 bitplano0    Línea 0 bitplano1   
Línea 1 bitplano0    Línea 1 bitplano1   
       .                    .
       .                    .
Línea 7 bitplano0    Línea 7 bitplano1 
Línea 0 bitplano2    Línea 0 bitplano3   
Línea 1 bitplano2    Línea 1 bitplano3   
       .                    .
       .                    .
Línea 7 bitplano2    Línea 7 bitplano3
```

El artículo señala que los tiles de 16x16 tienen "una disposición todavía más rara" pero no profundiza. Los sprites no se usan durante el gameplay de Another World.

### NTSC vs PAL: consideraciones técnicas

Las resoluciones NTSC (256x224) y PAL (256x239) se eligieron específicamente para respetar las garantías de "área segura de acción" de los televisores de principios de los 90.

Cita de wiki.nesdev.com sobre overscan: "Los televisores reales muestran alrededor de 224 líneas de la señal, de ahí la resolución comúnmente reportada de 256x224. Pero la posición vertical puede estar ligeramente descentrada. Los emuladores y las HDTV LCD tienden a usar las líneas 8 a 231, pero se ha visto que algunos televisores muestran las líneas 12 a 235."

**Super Mario World como implementación de referencia:** Nintendo invirtió en optimización para PAL en Super Mario World, a diferencia de la mayoría de desarrolladores que simplemente usaban 256x224 (resultando en bordes negros verticales en televisores PAL). El enfoque de Nintendo: usar resolución 256x239, reorganizar los tilemaps, y ajustar por las diferencias de refresco 60Hz/50Hz (16ms vs 20ms), asegurando una pantalla completa y un gameplay consistente en todo el mundo.

El truco de Nintendo en PAL fue "rellenar las dos líneas extra de tiles de 8x8 en la parte inferior con mayor visibilidad 'subterránea'. Los bordes y posiciones de texto se ajustaron en consecuencia." Nota sobre aspecto: "los televisores PAL y NTSC tenían una relación de aspecto 4:3. Una vez transferidos a pantalla, estos tilemaps aparecían estirados horizontalmente."

### Contexto de desarrollo del port

Rebecca Heineman portó Another World a SNES hacia 1992 para Interplay. No pudo responder preguntas antes de la publicación del artículo; la información se obtuvo de "Burgertime: Out of This World" y de análisis con el emulador bsnes-plus.

**Evaluación inicial del reto**, cita de Heineman: "Queríamos hacer juegos de Super Nintendo y Out of This World realmente era un juego que nos gustaría hacer. El pensamiento inmediato fue que el Amiga era un ordenador súper potente y aunque la Super Nintendo era potente, no era un Amiga. No había forma de que pudiéramos hacer OotW en SNES, simplemente no se podía hacer." Aceptó el reto: "bueno, cualquiera que haya seguido mi carrera sabría que 'no se puede hacer' generalmente resultaba en un 'reto aceptado'."

**Entorno de desarrollo: Apple IIgs y devkit propio.** La conversión ocurrió en un Apple IIgs acelerado (comparte el procesador 65C816 con la SNES) conectado mediante un emulador de ROM Sluggo 3 que Heineman "construyó ella misma".

**Metodología de traducción**, cita: "Cogí el código fuente y me fui a mi despacho a mirar el código y me di cuenta [...] de que todo lo que realmente necesitaba [...] era crear un intérprete que ejecutara el lenguaje de script y optimizar al máximo el intérprete para que el juego funcionara." Cronología: "me llevó unas dos o tres semanas sentada sin hacer otra cosa que traducir línea a línea [...]."

Enfoque: "miraba la función en 68k, averiguaba qué se suponía que hacía realmente, y escribía el equivalente en 65C816. Hice esto para cada función del juego hasta que finalmente tenía todas las funciones. ¡Y efectivamente, tenía Out of This World corriendo en un Apple IIGS! Iba solo a unos 10 fotogramas por segundo, más bien 5/10fps en un IIGS muy acelerado, porque no había hecho ninguna optimización real."

**Devkit y barrera de idioma:** Heineman usó un devkit casero consistente en "un cable paralelo desde el Apple IIGS a un Sluggo 3, este a su vez conectado a una Super Nintendo." Decisión motivada por coste: "usé principalmente este devkit casero porque Interplay era demasiado tacaña para comprarme un devkit oficial de Super Nintendo." Barrera de idioma: "de hecho, cuando estábamos desarrollando cosas para la Super Nintendo al principio fuimos víctimas de xenofobia por parte de Japón. Cuando nos dieron permiso para empezar a hacer software de Super Nintendo, Nintendo nos dio sus manuales. Estaban todos en japonés y nos dijeron que tardarían entre 2 y 6 meses en tener las traducciones." Heineman hablaba japonés: "日本語を話しますか? ¡Sí que hablo!"

### Diseño general: arquitectura de renderizado

**Estrategia de framebuffer**, pipeline de tres etapas:
1. La CPU renderiza en tres framebuffers locales en RAM
2. Transferencia DMA: cuando el frame se completa, DMA copia el framebuffer a VRAM
3. Doble buffer en VRAM: evita el tearing de pantalla

**Selección de modo gráfico:** "la consola se configura en modo 1, que tiene tres fondos, pero todo ocurre en el Background 1."

**Proceso de actualización del tilemap:** "cada vez que hay un nuevo frame disponible, se actualiza el tilemap del doble buffer correspondiente. Los framebuffers se simulan mediante tiles únicos de 8x8."

**Rendimiento del renderizado:** "gracias a la disposición planar de sus tiles, esta versión podía renderizar de forma similar al Atari ST y avanzar con segmentos de línea de 8 píxeles escritos en 4 operaciones."

### Cuellos de botella de rendimiento

Se identifican dos ralentizaciones críticas:

1. **Intérprete de bytecode:** "una ralentización significativa es el bytecode, que fue diseñado pensando en registros de 16 bits. El Ricoh tiene registros de 16 bits pero su bus de datos de 8 bits hacía que las operaciones de load/store fueran el doble de lentas."

2. **Ancho de banda de memoria:** "incluso con DMA, empujar todos esos bytes de RAM a VRAM era un problema. Parece que fue un problema todavía más difícil implementar el opcode COPY."

**Compromiso de resolución:** para abordar tanto las restricciones de CPU como de ancho de banda: "la solución a ambos problemas de CPU y ancho de banda fue usar solo 28x20 de los 32x28 tiles disponibles. Esto bajaba efectivamente la resolución de 256x224 a 224x160."

### Chips de expansión: intentos iterativos de optimización

**Arquitectura de extensibilidad hardware:** el puerto de cartucho de la SNES se diseñó específicamente para chips de expansión: espacio de memoria reservado en $6000–$7FFF del espacio de direcciones del Bus-A, y una línea de señal /CART que la CPU activa al acceder a ese espacio de direcciones de cartucho. Esto permitía cartuchos con chips especiales que monitorizan el uso del Bus-A y usan el espacio reservado para mapear sus registros y recibir órdenes de la CPU.

Heineman exploró "una serie de cuatro revisiones distintas".

**Intento 1 — chip Super-FX:** Heineman intentó inicialmente lograr 60fps usando el chip de expansión Super-FX: "pensé que podría conseguir que este juego corriera a 60 fotogramas por segundo metiendo un chip Super FX. Conseguí una copia de Starfox, le saqué el chip, averigüé cómo funcionaba y empecé a usarlo. ¡Tenía OotW corriendo a 60fps! Le dije 'oye Brian, el juego está a punto de salir, aquí tienes el cartucho que voy a necesitar'. Mira la lista de precios y dice '¿puedes quitar el chip Super FX? ¡Es demasiado caro!'." Resultado: rechazado por coste.

**Intento 2 — RAM estática (WRAM):** obligada a abandonar el Super-FX, Heineman persiguió RAM estática en el cartucho: "quité todo el código del Super FX y lo cambié a renderizado por software puro. Pero había ciertas partes del renderizado por software que iban realmente muy lentas. Una de ellas era que tenía que coger una pantalla de fondo completa y copiarla a otra. Descubrí este truco: si tenía memoria estática [WRAM] en el cartucho, podía copiar esa memoria estática a velocidades enormes usando DMA, y dije 'dadme un cartucho que use memoria estática y puedo llegar a 30 fotogramas por segundo'. Y él dice algo como 'eso es demasiado caro'." Resultado: también rechazado por coste.

**Intento 3 — cartucho rápido (3.58MHz):** sin RAM con pila de respaldo, Heineman optimizó para ROM de alta velocidad: "así que sin RAM con pila de respaldo, íbamos a tener que guardar la partida con un pequeño código de entrada [...]. Cuando llegas a cierto nivel dice aquí está tu código [...] lo apuntas en un papel y así es como guardas tus partidas." Opciones de velocidad de ROM disponibles: SlowROM a 2.68MHz; FastROM a 3.58MHz. "Vale, sin chip Super FX, sin RAM con pila de respaldo, bien, conseguí que el software volviera a correr a unos 30 fotogramas por segundo si, claro, uso un cartucho rápido, porque los cartuchos de la época se podían comprar a 2.68Mhz [SlowROM] o 3.58Mhz [FastROM]." Ventaja de desarrollo: "para que te hagas una idea, la SNES corre en realidad a 3.6Mhz. Es un chip bastante rápido para la época, pero las ROMs que corren a esa velocidad cuestan un poco más que las que corren más lento. Usando una ROM de 3.6Mhz, mi devkit podía correr e ir hasta 10Mhz, así que no era problema para mí simplemente poner el interruptor para que el devkit corriera muy rápido, con buena pinta, listo." Presión de costes: "entonces llega Fargo y dice 'puedes usar la ROM más lenta, ahorrará 50 centavos por cartucho'." Resultado: obligados a usar SlowROM.

**Intento 4 — cartucho lento (2.68MHz) con truco de registro hardware:** enfrentada a las limitaciones de SlowROM, Heineman descubrió una optimización no documentada: "forzada a usar una slowROM, descubrí que el juego iría como mucho a 20 fotogramas por segundo. Probé pequeños trucos como poner el juego en formato letterbox, hacer que pareciera una película, pero en realidad estoy renderizando menos, lo cual sube el framerate, pero no llegaba del todo. ¿Cómo demonios voy a conseguir que esto corra tan rápido como quiero?"

Descubrimiento clave: "entonces caí en la cuenta: los registros DMA de la SNES que no estoy usando están ahí sentados. Después de leer las especificaciones técnicas muy cuidadosamente en japonés, descubrí que esos registros de memoria en realidad corren a la velocidad completa de la CPU de 3.6Mhz."

Técnica de implementación: "los registros hardware son esencialmente valores en los que puedes simplemente escribir y leer. Había unos 32 bytes de memoria que se usan como registros, pero mientras no los uses de verdad, simplemente pones valores y los vuelves a leer. Metí ahí una función de 32 bytes que dibujaba una scanline de datos de polígono. Tuve una mejora de velocidad del 10% y conseguí mis fotogramas por segundo."

Complicaciones de emulación: "por supuesto, los autores de emuladores más adelante me odiarían. '¿Por qué este cartucho no corre en mi emulador?'."

### Adaptación PAL/NTSC y relación de aspecto

Con una ventana de desarrollo limitada, "Another World en sistema PAL no se benefició de un ajuste horizontal especial. La versión PAL del juego se lanzó con la misma resolución de tiles que la versión NTSC."

**Cálculos de relación de aspecto:**
- Versión PAL: relación 1.74
- Versión NTSC: relación 1.6299
- Relación prevista: 1.666

Conclusión del artículo: "ambas están muy cerca de la relación prevista de 1.666."

### Comparación de colores y resolución

| Plataforma | Resolución |
|------------|------------|
| SNES       | 224 x 160  |
| Genesis    | 224 x 176  |
| Amiga      | 320 x 200  |

**Conversión de profundidad de color:** "gracias a la profundidad de color de 15 bits, los colores en SNES son una conversión exacta de los colores de 12 bits del Amiga. Las expresiones faciales de Lester no se alteraron, lo que puede indicar que la conversión de Genesis se hizo después que la de SNES, con arte especial solicitado a Éric Chahi."

### Veredicto: reconocimiento del creador

El artículo cierra con el reconocimiento de Éric Chahi a Heineman: "tengo una lámina de la portada de 'Out Of This World'. Está firmada por Éric Chahi y dice en francés 'Felicidades por la finalización y el envío de la versión para Super Nintendo de Out of This World'. Me dio esa pintura ya enmarcada [...] y yo estaba como 'oh dios mío, muchísimas gracias'. Ha estado colgada en mi pared desde entonces."

## Notas / gotchas

- El cuello de botella principal en SNES fue doble: (1) el intérprete de bytecode, diseñado pensando en registros de 16 bits, se ralentizaba porque el bus de datos del Ricoh 5A22 es de solo 8 bits (load/store el doble de lentos); (2) el ancho de banda RAM→VRAM vía DMA, igual que en Genesis, resultó insuficiente — el opcode COPY fue "un problema todavía más difícil" de resolver que en otras plataformas.
- La solución de resolución (224×160, usando 28x20 de los 32x28 tiles disponibles en modo 1) es el mismo patrón que en Genesis (recortar el área activa para bajar volumen de datos transferidos por DMA), pero partiendo de una CPU bastante más débil.
- Los chips de expansión (Super-FX, RAM estática con pila) habrían resuelto el problema de rendimiento de raíz, pero se descartaron sistemáticamente por coste por cartucho, no por limitación técnica — un recordatorio de que las restricciones de negocio (coste unitario) pueden pesar tanto como las de hardware en decisiones de arquitectura de un motor.
- El truco final que salvó el proyecto (usar los ~32 bytes de registros DMA no utilizados como si fueran RAM de trabajo a velocidad de CPU completa, para alojar una rutina crítica de 32 bytes que dibuja una scanline de polígono) es un hack de hardware no documentado, descubierto leyendo las especificaciones técnicas en japonés — y rompió la compatibilidad con emuladores que no modelan ese comportamiento no oficial. Ilustra un riesgo genérico de los trucos "no documentados pero medidos empíricamente": funcionan en hardware real pero pueden fallar en herramientas de terceros.
- Igual que en Genesis, la solución de renderizado es CPU-side (framebuffer en RAM simulado con tiles 8x8 únicos) + blit final por DMA a VRAM con doble buffer — el PPU de la SNES, pese a ser "una potencia de sprites" con blending RGB, tampoco ofrece un camino nativo de rasterizado de polígonos arbitrarios, igual que el VDP de Genesis.
- La barrera de idioma (manuales solo en japonés, 2-6 meses de espera de traducción) y el uso de un devkit casero conectado por cable paralelo son detalles de contexto de producción, no de arquitectura técnica, pero explican por qué ciertas decisiones (como el truco de registros DMA) se descubrieron tarde y de forma no oficial.

## Referencias cruzadas

- MOTORES/arquitectura/another-world-pc-dos.md
- MOTORES/arquitectura/another-world-genesis.md
- MOTORES/arquitectura/another-world-gba.md
- MOTORES/arquitectura/another-world-jaguar.md

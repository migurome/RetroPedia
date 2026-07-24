---
titulo: Juegos de SNES difíciles de emular con precisión
plataforma: [SNES]
categoria: [herramientas]
tags: [emulacion, mesen-s, bugs, ppu, dma, hdma, spc]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Tricky-to-emulate_games]
---

# Juegos de SNES difíciles de emular con precisión

## Resumen

Catálogo, recopilado durante el desarrollo del emulador Mesen-S, de juegos concretos de SNES que exponen bugs de emulación por depender de comportamientos exactos y poco documentados del hardware real (open bus, timing de BRK/COP, HDMA, Mode 7, SPC700, etc.). Organizado por el tipo de problema de hardware implicado, no alfabéticamente.

## Detalle técnico

### Problemas de implementación de open bus

- **Captain America and the Avengers**: gráficos rotos durante el juego si el open bus falta o es incorrecto.
- **The Combatribes**: no arranca si el open bus está mal implementado.
- **Home Alone**: se cuelga después del menú principal por deficiencias en el open bus.
- **Rock n' Roll Racing**: gráficos rotos en la pantalla de selección de personaje.
- **Super 3D Noah's Ark**: los gráficos de Modo 7 fallan sin un manejo correcto del open bus.

### Manejo de las instrucciones BRK/COP

- **Actraiser**: se cuelga justo después del menú por una implementación incorrecta de BRK/COP.
- **Bishoujo Senshi Sailor Moon - Another Story**: cuelgue tras el menú relacionado con la ejecución de BRK/COP.
- **Cybernator**: la intro se reinicia al empezar la partida, señal de problemas de timing con estas instrucciones.
- **Dekitate High School**: no arranca sin soporte correcto de BRK/COP.
- **Illusion of Gaia**: fallo de arranque ligado a la implementación de estas instrucciones.
- **Kamaitachi no Yoru**: requiere un manejo preciso de BRK/COP para arrancar.
- **Soul Blazer**: fallo de arranque sin la emulación correcta de estas instrucciones.

### Problemas específicos de instrucciones

- **Super Mario World**: un error en la implementación de la instrucción ORA [d] provoca un "mapa del mundo corrupto (mala descompresión 3bpp)"; además, un segundo problema hace que el Mario del indicador de vidas quede oculto en la esquina superior izquierda, por falta de rotación de prioridad de sprites en OAMADD.

### Acceso a VRAM y renderizado

- **Hook**: aparecen glitches de texto durante la intro cuando las escrituras a VRAM no se ignoran durante el renderizado.
- **Kick Off**: necesita el comportamiento correcto de incremento de dirección de VRAM al ignorar escrituras durante el renderizado.
- **Breath of Fire**: necesita una implementación correcta de lectura de VRAM.
- **Super Famista 5**: presenta problemas en la parte superior de la pantalla durante animaciones del menú principal, por una lógica de wraparound incorrecta del offset-per-tile.
- **Axelay**: en la fase 2 muestra "fondo de nivel roto en algunas zonas" por bugs de offset-per-tile.
- **Chrono Trigger**: el menú de juego muestra un "layout roto" por el mismo error de implementación de offset-per-tile.

### Renderizado de gráficos en Modo 7

- **The Atlas - Renaissance Voyager**: la intro muestra "gráficos de Modo 7 rotos" porque el Modo 7 no implementa la lógica de ventanas (window logic).
- **MechWarrior**: gráficos de Modo 7 rotos durante el juego; además, tras el menú aparecen "gráficos de Modo 7 rotos durante el juego" por no ignorarse el flag de "decrement" del HDMA.
- **NHL '94**: aparece un "glitch de scanline en medio de la pantalla durante la animación de la intro" porque los offsets de scroll de Modo 7 no se fijan (latch) al inicio de la scanline.
- **Aerobiz**: "colores incorrectos durante la secuencia de intro" por una implementación incorrecta del color directo (direct color) de Modo 7.

### Ventanas de color y gráficos

- **Krusty's Super Fun House**: dos problemas distintos — "gráficos rotos en los bordes de la pantalla" porque la ventana de color no se aplica a todos los píxeles, y "colores incorrectos durante el juego" porque el controlador de DMA permite leer registros del B-bus usando direcciones del A-bus.
- **Jurassic Park**: "gráficos rotos durante el juego" causados por un cálculo incorrecto del color math de la subpantalla en modos de alta resolución.

### Renderizado de sprites

- **Uniracers**: en modo 2 jugadores, "el unicycle del segundo jugador no se muestra correctamente" por una implementación inexacta de escritura en OAM durante el renderizado.
- **Mega lo Mania - Jikuu Daisenryaku**: "aparece una línea negra en medio de la pantalla durante la intro" por un timing inexacto de lectura/renderizado de OAM.

### Problemas de controlador DMA y HDMA

- **Heian Fuuunden**: "pantalla de título corrupta al pulsar start para saltar la animación de apertura", por un estado de encendido inválido del controlador de DMA.
- **Mighty Morphin Power Rangers - The Fighting Edition**: la CPU debe ejecutar un ciclo extra antes de iniciar el DMA tras escribir en $420B.
- **Circuit USA**: el menú muestra "gráficos rotos" por timings inexactos de DMA/HDMA.
- **Jumbo Ozaki no Hole in One**: el menú muestra "gráficos rotos" por problemas de timing.
- **Dekitate High School**: requiere que el DMA se suspenda mientras se ejecuta el HDMA.
- **Batman Forever**: los gráficos de Modo 7 de la intro fallan cuando el HDMA no ignora el flag de "fixed transfer".
- **The Lost Vikings**: "pantalla negra al empezar la partida" por el mismo motivo del flag de "fixed transfer" en HDMA.
- **The Adventures of Kid Kleets**: "gráficos rotos al encender" porque el HDMA no ignora el flag de "decrement".
- **Pocky & Rocky**: dos problemas — "pantallas de licencia/logo rotas al encender" porque el HDMA ignora el flag de dirección, y "parpadeos de pantalla durante el juego" porque el NMI no se dispara cuando se activa a mitad del vertical blank.
- **Aladdin**: "gráficos rotos en el fondo del primer nivel" por una gestión incorrecta del flag "do transfer" del HDMA.
- **Super Ghouls'n Ghosts**: "pantalla negra al empezar la partida" por el mismo problema del flag "do transfer" del HDMA.

### Problemas de interrupciones y timing

- **RoboCop versus The Terminator**: el V-IRQ no se dispara si se activa para la scanline actual.
- **Alien vs Predator**: "parpadeos de pantalla durante el juego" porque el NMI no se dispara cuando se activa a mitad de vertical blank.
- **Jaki Crush**: no arranca; la CPU debería retrasar una instrucción antes de saltar al vector de interrupción cuando el NMI se dispara a mitad de vertical blank.

### Timing del procesador de audio

- **ActRaiser 2**: se congela si el timing del SPC requiere ejecución ciclo a ciclo en vez de instrucción a instrucción.
- **Hiouden - Mamono-tachi to no Chikai**: "sonido roto" por timing inexacto del SPC.
- **Illusion of Gaia**: fallo de arranque adicional por timing del SPC (también listado en BRK/COP).
- **Tales of Phantasia**: "faltan efectos de sonido, se congela" por el mismo requisito de timing del SPC.
- **Rendering Ranger R2**: "se congela por un bucle infinito con el SPC" cuando los efectos de lectura de la CPU no ocurren lo bastante pronto.
- **Chester Cheetah - Too Cool to Fool**: "faltan efectos de sonido" porque el registro KOF del DSP no se inicializa a $00.
- **King of Dragons**: "faltan efectos de sonido" por el mismo motivo de inicialización de KOF en el DSP.

### Mapeo de memoria e inicialización

- **Fire Emblem - Thracia 776**: mapeos de SRAM incorrectos.
- **Ys III - Wanderers from Ys**: errores de mapeo de SRAM.
- **Bishoujo Senshi Sailor Moon - Another Story**: "estática aleatoria en la música durante la intro (cuando la RAM del SPC se aleatoriza)" por un estado de encendido de RAM incorrecto.
- **Death Brade**: "el duelo termina instantáneamente cuando la RAM se inicializa con ceros".
- **Power Drive**: "gráficos rotos cuando la RAM se inicializa con ceros".
- **Super Keiba 2**: "pantalla negra tras unos menús cuando la SRAM se inicializa con ceros".

### Problemas de chips especializados

- **Yoshi's Island**: "efectos rotos en los túneles" por una implementación incorrecta de RPIX del Super FX.
- **Speedy Gonzales: Los Gatos Bandidos**: "el juego se bloquea en el nivel 6-1" porque el HDMA no puede afectar al open bus.

## Notas / gotchas

- Esta lista demuestra que la emulación precisa de SNES exige implementar con exactitud numerosos quirks de hardware, comportamientos de timing y casos límite en CPU, PPU, DMA, HDMA, audio y memoria — todos ellos coinciden con quirks ya catalogados de forma más abstracta en la entrada de erratas de hardware.
- Varios títulos aparecen en más de una categoría (p. ej. Illusion of Gaia en BRK/COP y en timing del SPC; Dekitate High School en BRK/COP y en DMA/HDMA), lo que indica que combinan varios bugs de hardware distintos.

## Referencias cruzadas

- [Erratas y quirks conocidos del hardware SNES](errata.md)
- [Juegos con modos gráficos inusuales](juegos-modos-graficos-inusuales.md)
- [ROMs de test para verificar precisión de emuladores](tests-emuladores.md)
- (pendiente de enlazar)

# Índice

Mapa general de RetroPedia. Se actualiza cada vez que se añade una entrada nueva.

Contenido inicial extraído de [plutiedev.com](https://plutiedev.com) (2026-07-21) — todas las entradas están marcadas `estado: borrador` en su frontmatter hasta ser revisadas/contrastadas durante el desarrollo de un proyecto real.

## General

- [GLOSARIO.md](GLOSARIO.md) — conceptos transversales a varias plataformas

### GLOSARIO/ — fundamentos de programación de juegos

- [Números en punto fijo](GLOSARIO/fixed-point.md)
- [Matemáticas de ángulos](GLOSARIO/angle-math.md)
- [Velocidad y gravedad en plataformas](GLOSARIO/speed-and-gravity.md)
- [Detección de colisiones básica](GLOSARIO/basic-collision.md)
- [Ordenación de sprites](GLOSARIO/sorting-sprites.md)
- [Sistemas numéricos — decimal, binario y hexadecimal](GLOSARIO/dec-bin-hex.md)

## Plataformas

- [MegaDrive/](MegaDrive/README.md)
- [SNES/](SNES/README.md)

### Mega Drive / Genesis

**cpu/**
- [Motorola 68000 — instrucciones y modos de direccionamiento (base)](MegaDrive/cpu/68000-instrucciones-modos-direccionamiento.md)
- [Multiplicación y división en el 68000](MegaDrive/cpu/multiplication-division.md)
- [Matemáticas BCD en el 68000](MegaDrive/cpu/bcd-math.md)
- [Matemáticas de 64 bits en el 68000](MegaDrive/cpu/64bit-math.md)
- [La instrucción MOVEP](MegaDrive/cpu/movep.md)
- [Usando el Z80 en la Mega Drive](MegaDrive/cpu/using-the-z80.md)
- [Tablas de salto en Z80](MegaDrive/cpu/z80-jump-tables.md)
- [Extender un valor de 8 bits a 16 bits en Z80](MegaDrive/cpu/z80-extend-8bit-to-16bit.md)
- [Sumar y restar un valor de 8 bits a uno de 16 bits en Z80](MegaDrive/cpu/z80-add-8bit-to-16bit.md)
- [Hilos de SpritesMind sobre timing de bus, DMA y Z80](MegaDrive/cpu/spritesmind-bus-timing-hilos.md) *(índice curado, foro)*
- [Big Evil Corporation — bases del ensamblador 68000](MegaDrive/cpu/bec-asm-basics.md) *(blog de TANGLEWOOD)*
- [Hello World en código máquina puro para Mega Drive (anotado)](MegaDrive/cpu/bumbershoot-hello-world-anotado.md) *(blog de Bumbershoot Software)*
- [Primer diseño e implementación del Autómata Celular Cíclico (CCA) en Genesis](MegaDrive/cpu/bumbershoot-cca-primera-implementacion.md) *(blog de Bumbershoot Software)*
- [Optimización del bucle principal del CCA en 68000 (2018)](MegaDrive/cpu/bumbershoot-cca-optimizacion-inicial.md) *(blog de Bumbershoot Software)*
- [Re-optimización del CCA en Genesis en 2025 — contar ciclos en serio](MegaDrive/cpu/bumbershoot-cca-reoptimizacion-2025.md) *(blog de Bumbershoot Software)*
- [Variaciones sobre el ABI/convención de llamada del 68000](MegaDrive/cpu/bumbershoot-variaciones-abi-68000.md) *(blog de Bumbershoot Software)*

**graficos/**
- [Configuración inicial del VDP](MegaDrive/graficos/vdp-setup.md)
- [Escritura en memoria de vídeo (VRAM/CRAM/VSRAM)](MegaDrive/graficos/writing-video.md)
- [Tiles y paletas](MegaDrive/graficos/tiles-and-palettes.md)
- [Sprites en el VDP](MegaDrive/graficos/sprites.md)
- [Formato del Tile ID (nametable / sprites)](MegaDrive/graficos/tile-id.md)
- [Prioridad de capas (layer priority)](MegaDrive/graficos/layer-priority.md)
- [Resoluciones de pantalla del VDP](MegaDrive/graficos/screen-resolution.md)
- [Transferencias DMA](MegaDrive/graficos/dma-transfer.md)
- [Modo Shadow/Highlight](MegaDrive/graficos/shadow-highlight.md)
- [Referencia de comandos del VDP](MegaDrive/graficos/vdp-commands.md)
- [Referencia de registros del VDP](MegaDrive/graficos/vdp-registers.md)
- [Referencia de la tabla de sprites](MegaDrive/graficos/sprite-table.md)
- [Puerto de depuración del VDP](MegaDrive/graficos/vdp-debug.md)
- [Rampa de color del VDP](MegaDrive/graficos/vdp-color-ramp.md)
- [Hilos de SpritesMind sobre internals del VDP](MegaDrive/graficos/spritesmind-vdp-hilos.md) *(índice curado, foro)*
- [Interrupciones del VDP y externas (VInt, HInt, external)](MegaDrive/graficos/segaretro-interrupts.md) *(índice curado, Sega Retro)*
- [Escalado de sprites (técnicas para simularlo)](MegaDrive/graficos/scaling-sprites.md)
- [Rotación de sprites (técnicas para simularla)](MegaDrive/graficos/rotating-sprites.md)
- [Efecto de agua translúcida de Sonic](MegaDrive/graficos/sonic-water.md)
- [Tarjetas de título de Sonic (fade-in parcial)](MegaDrive/graficos/sonic-title-cards.md)
- [Efecto de fuego de Doom Troopers](MegaDrive/graficos/doomtroopers-fire.md)
- [Efecto de espejo 3D de Project MD](MegaDrive/graficos/project-md-3d-mirror.md) *(mirror de terceros)*
- [Big Evil Corporation — inicialización de hardware al arranque](MegaDrive/graficos/bec-vdp-init.md) *(blog de TANGLEWOOD)*
- [Big Evil Corporation — primer "Hello World" en pantalla](MegaDrive/graficos/bec-hello-world.md) *(blog de TANGLEWOOD)*
- [Big Evil Corporation — fuentes y renderizado de texto](MegaDrive/graficos/bec-fonts-text.md) *(blog de TANGLEWOOD)*
- [Big Evil Corporation — sprites ("Scary Monsters and Nice Sprites")](MegaDrive/graficos/bec-sprites.md) *(blog de TANGLEWOOD)*
- [Animación de sprites — enfoque Big Evil Corporation](MegaDrive/graficos/bec-animated-sprites.md) *(blog de TANGLEWOOD)*
- [Mapas y planos de scroll — enfoque Big Evil Corporation](MegaDrive/graficos/bec-maps-scrolling.md) *(blog de TANGLEWOOD)*
- [Bumbershoot Software — VDP desde cero: "Hello World" y bitmap con ASM puro](MegaDrive/graficos/bumbershoot-hello-world-y-bitmap-vdp.md) *(blog de Bumbershoot Software)*
- [Bumbershoot Software — pipeline de cuantización y compresión para un logo en Genesis](MegaDrive/graficos/bumbershoot-refinando-graficos-logo.md) *(blog de Bumbershoot Software)*
- [Bumbershoot Software — scroll de pantalla completa dirigido por interrupción VBLANK](MegaDrive/graficos/bumbershoot-scroll-pantalla-completa-vblank.md) *(blog de Bumbershoot Software)*
- [Fundido de paleta y efecto "pixel bleed" por VSRAM (Bumbershoot / CCA)](MegaDrive/graficos/bumbershoot-cca-fade-pixel-bleed.md) *(blog de Bumbershoot Software)*
- [Presupuesto de ancho de banda a VRAM y la "venganza" de las badlines del C64](MegaDrive/graficos/bumbershoot-badlines-revenge.md) *(blog de Bumbershoot Software)*
- [Modo pseudo-bitmap mediante dos planos de scroll superpuestos](MegaDrive/graficos/bumbershoot-pseudo-bitmap-dos-planos.md) *(blog de Bumbershoot Software)*

**sonido/**
- [Operaciones comunes del YM2612](MegaDrive/sonido/ym2612-operations.md)
- [PSG (Programmable Sound Generator)](MegaDrive/sonido/psg.md)
- [Mapa de registros del YM2612](MegaDrive/sonido/ym2612-registers.md)
- [Caja (snare drum) FM con el YM2612](MegaDrive/sonido/fm-snare-drum.md)
- [Sonidos chiptune con el YM2612](MegaDrive/sonido/chiptune-sounds.md)
- [Silbido (whistle) con el PSG](MegaDrive/sonido/psg-whistle.md)
- [Guitarra con el PSG](MegaDrive/sonido/psg-guitar.md)
- [Formato EIF (Echo Instrument Format)](MegaDrive/sonido/format-eif.md)
- [Formato EEF (Echo Envelope Format)](MegaDrive/sonido/format-eef.md)
- [Formatos TFI y VGI](MegaDrive/sonido/format-tfi.md)
- [Hilos de SpritesMind sobre el YM2612](MegaDrive/sonido/spritesmind-ym2612-hilos.md) *(índice curado, foro)*
- [SMPS — driver de sonido de Sega](MegaDrive/sonido/smps-sound-driver.md)
- [Sonido, parte I — el chip PSG — enfoque Big Evil Corporation](MegaDrive/sonido/bec-sound-psg.md) *(blog de TANGLEWOOD)*
- [Driver de música PSG en Z80 con decaimiento de subframe (Bumbershoot)](MegaDrive/sonido/bumbershoot-z80-driver-psg.md) *(blog de Bumbershoot Software)*
- [Reproducción de PCM por el YM2612 con temporización por ciclos (Bumbershoot)](MegaDrive/sonido/bumbershoot-ym2612-pcm-playback.md) *(blog de Bumbershoot Software)*
- [Un bug de offset de canal en los registros $B0/$B4 del YM2612, diagnosticado con Gens KGen](MegaDrive/sonido/bumbershoot-bug-offset-canal-ym2612.md) *(blog de Bumbershoot Software)*

**perifericos/**
- [Puertos de I/O de la Mega Drive](MegaDrive/perifericos/io-ports.md)
- [Mandos de Mega Drive (3 y 6 botones)](MegaDrive/perifericos/controllers.md)
- [Sega Mouse (ratón) en Mega Drive](MegaDrive/perifericos/mouse.md)
- [Sega Multitap (Team Player / Sega Tap)](MegaDrive/perifericos/sega-multitap.md)
- [EA Multitap (4 Way Play)](MegaDrive/perifericos/ea-multitap.md)
- [Teclado de Saturn en Mega Drive](MegaDrive/perifericos/saturn-keyboard.md)
- [Ten Key Pad (Mega Anser)](MegaDrive/perifericos/ten-key-pad.md)
- [Interfaz de impresora en Mega Drive](MegaDrive/perifericos/printer.md)
- [Identificación de periféricos](MegaDrive/perifericos/peripheral-id.md)
- [Pinout de los puertos de I/O (Mega Drive y Saturn)](MegaDrive/perifericos/io-pinout.md)
- [Lectura de mando y bucle principal — enfoque Big Evil Corporation](MegaDrive/perifericos/bec-input-game-loop.md) *(blog de TANGLEWOOD)*
- [Bumbershoot Software — lectura de mando de 3 botones en ASM puro](MegaDrive/perifericos/bumbershoot-lectura-mando-3-botones.md) *(blog de Bumbershoot Software)*

**memoria/**
- [Padding de la ROM](MegaDrive/memoria/rom-padding.md)
- [Guardado en SRAM](MegaDrive/memoria/saving-sram.md)
- [Más allá de 4MB (bank switching / mappers)](MegaDrive/memoria/beyond-4mb.md)
- [Cabecera de la ROM](MegaDrive/memoria/rom-header.md)
- [Cartucho Lock-On (Sonic & Knuckles)](MegaDrive/memoria/lockon.md)
- [Uso del rango $A130xx](MegaDrive/memoria/a130xx-usage.md)
- [Conector de cartucho (pinout)](MegaDrive/memoria/cartridge-slot.md)
- [Problemas de hardware conocidos](MegaDrive/memoria/hardware-issues.md)
- [Hilos de SpritesMind sobre diferencias entre revisiones de hardware](MegaDrive/memoria/spritesmind-hardware-revisiones-hilos.md) *(índice curado, foro)*
- [Quirks de hardware documentados por Sega Retro](MegaDrive/memoria/segaretro-quirks.md) *(índice curado, Sega Retro)*
- [Notas de hardware de Kabuto (VDP en profundidad)](MegaDrive/memoria/kabuto-hardware-notes.md) *(mirror de terceros)*
- [Depurando el AtGames Firecore — bug de tono/pitch de audio (TANGLEWOOD)](MegaDrive/memoria/bec-atgames-firecore.md) *(blog de TANGLEWOOD)*
- [Depurando el AtGames Flashback — bug del emulador de Z80 en IYH (TANGLEWOOD)](MegaDrive/memoria/bec-atgames-flashback.md) *(blog de TANGLEWOOD)*

**avanzado/**
- [Multitarea en Mega Drive](MegaDrive/avanzado/multitasking.md)
- [Usar el Sub-CPU de Mega CD en modo 1](MegaDrive/avanzado/subcpu-in-mode1.md)
- [Control de LEDs en Mega CD](MegaDrive/avanzado/megacd-leds.md)
- [Análisis del startup code oficial de Sega (arqueología del boot code de ROMs comerciales)](MegaDrive/avanzado/bumbershoot-analisis-startup-code-sega.md) *(blog de Bumbershoot Software)*

**herramientas/**
- [Formato de compresión SLZ](MegaDrive/herramientas/format-slz.md)
- [Formato de compresión UFTC](MegaDrive/herramientas/format-uftc.md)

**herramientas/sgdk/** — [SGDK](https://github.com/Stephane-D/SGDK), SDK en C para Mega Drive
- [SGDK — overview](MegaDrive/herramientas/sgdk/overview.md)
- [Instalación de SGDK](MegaDrive/herramientas/sgdk/installation.md)
- [Uso de SGDK (estructura de proyecto y build)](MegaDrive/herramientas/sgdk/usage.md)
- [Tutorial: Introducción](MegaDrive/herramientas/sgdk/tuto-introduction.md)
- [Tutorial: Hello World](MegaDrive/herramientas/sgdk/tuto-hello-world.md)
- [Tutorial: Backgrounds](MegaDrive/herramientas/sgdk/tuto-background.md)
- [Tutorial: Sprites](MegaDrive/herramientas/sgdk/tuto-sprites.md)
- [Tutorial: Input](MegaDrive/herramientas/sgdk/tuto-input.md)
- [Tutorial: Matemáticas (fix16/fix32)](MegaDrive/herramientas/sgdk/tuto-maths.md)

**herramientas/** — [SpritesMind.Net](https://www.spritesmind.net/), foro de referencia (índice curado, no extracción completa)
- [SpritesMind — overview y limitaciones de acceso](MegaDrive/herramientas/spritesmind-overview.md)

**herramientas/** — [Sonic Retro](https://info.sonicretro.org/Disassemblies), disassemblies de juegos comerciales
- [Disassemblies de Sonic Retro (código fuente real de juegos comerciales)](MegaDrive/herramientas/sonic-retro-disassemblies.md)
- [Formatos de compresión clásicos de Sega — Nemesis, Kosinski, Enigma](MegaDrive/herramientas/compresion-nemesis-kosinski-enigma.md)

**herramientas/** — caso práctico
- [Caso práctico — hacer un juego de Mega Drive en 2019 (Doctor Ludos / SGDK)](MegaDrive/herramientas/caso-practico-sgdk-2019.md)

**herramientas/** — [Sega Retro](https://segaretro.org/Category:Mega_Drive_technical_information), wiki de documentación técnica (índice curado)
- [Sega Retro — overview y limitaciones de acceso](MegaDrive/herramientas/segaretro-overview.md)

**herramientas/** — flujo de trabajo sin SGDK
- [Build y debug de ASM puro para Mega Drive (sin SGDK)](MegaDrive/herramientas/build-debug-asm-puro.md)

**herramientas/** — [blog de Big Evil Corporation](https://blog.bigevilcorporation.co.uk/), serie de TANGLEWOOD (juego comercial real)
- [Big Evil Corporation — puesta en marcha del toolchain](MegaDrive/herramientas/bec-getting-started.md)
- [Taller de ensamblador Mega Drive y ejemplo "hello world" (2018)](MegaDrive/herramientas/bec-workshop-2018.md) *(mayormente redundante con "Getting Started" — ver notas de la propia entrada)*

**herramientas/** — [blog de Bumbershoot Software](https://bumbershootsoft.wordpress.com/platform-guide-sega-genesis-mega-drive/), guía de plataforma + serie de desarrollo 2018/2025/2026
- [Bumbershoot Software — panorama de hardware y elección de herramientas para Genesis/Mega Drive](MegaDrive/herramientas/bumbershoot-preparando-entorno-genesis.md)
- [Comparativa de emuladores para desarrollo en Mega Drive/Genesis](MegaDrive/herramientas/bumbershoot-emuladores-desarrollo.md)
- [Comparativa de toolchains de ensamblador para Mega Drive (SGDK, ASMX, Vasm, WLA-DX, ASMotor)](MegaDrive/herramientas/bumbershoot-comparativa-toolchains.md)
- [El toolchain improvisado de 2018 para Genesis (ASMX + conversión manual)](MegaDrive/herramientas/bumbershoot-toolchain-improvisado-2018.md)
- [Modernizando el toolchain de ASMX en 2025 (INCBIN, RORG/REND, smdfix)](MegaDrive/herramientas/bumbershoot-modernizando-toolchain-asmx-2025.md)
- [Probando ASMotor para Mega Drive (2026)](MegaDrive/herramientas/bumbershoot-asmotor-mega-drive.md)
- [Bugs de compatibilidad entre emuladores detectados al lanzar CCA (Genesis)](MegaDrive/herramientas/bumbershoot-cca-bugs-emuladores-lanzamiento.md)
- [Descompresor LZ4 en ensamblador 68000](MegaDrive/herramientas/bumbershoot-lz4-decompression-68000.md)
- [Bumbershoot Software — el toolchain improvisado de 2018 para Genesis (ASMX + conversión manual)](MegaDrive/herramientas/bumbershoot-toolchain-improvisado-2018.md)
- [Bumbershoot Software — modernizando el toolchain de ASMX en 2025 (INCBIN, RORG/REND, smdfix)](MegaDrive/herramientas/bumbershoot-modernizando-toolchain-asmx-2025.md)
- [Bumbershoot Software — probando ASMotor para Mega Drive (2026)](MegaDrive/herramientas/bumbershoot-asmotor-mega-drive.md)

### SNES

Contenido extraído de la [SNESdev Wiki](https://snes.nesdev.org/wiki/SNESdev_Wiki) (2026-07-21), `estado: borrador`. No se documentaron los pinouts eléctricos de chips internos (ver [`SNES/README.md`](SNES/README.md)) ni varias páginas que resultaron ser stubs sin contenido técnico real (65C816 general, Variable_width_fonts, DSP_Expansion, Super_FX, CX4).

**cpu/**
- [Vectores de interrupción y reset del 65816 en SNES](SNES/cpu/cpu-vectors.md)
- [Multiplicación en SNES (hardware 5A22 y Modo 7 de la PPU)](SNES/cpu/multiplicacion.md)
- [División en SNES (hardware de división del 5A22)](SNES/cpu/division.md)
- [El 65C816 para desarrolladores que ya conocen 6502](SNES/cpu/65c816-para-desarrolladores-6502.md)
- [Usar X (o Y) como puntero de 16 bits en 65816](SNES/cpu/usar-x-como-puntero.md)
- [MVN y MVP — copia de bloques de memoria en 65816](SNES/cpu/mvn-mvp-copia-bloques.md)
- [Gestión de tamaños de registro (8/16 bits) en ca65](SNES/cpu/tamanos-registro-ca65.md)
- [Signature byte en BRK, COP y WDM](SNES/cpu/signature-byte.md)
- [Structs vs. registros — dónde guardar el "this" en 65816](SNES/cpu/structs-vs-registros.md)

**graficos/**
- [Registros del PPU ($2100-$213F)](SNES/graficos/ppu-registers.md)
- [Fondos y modos de BG](SNES/graficos/backgrounds.md)
- [Tilemaps](SNES/graficos/tilemaps.md)
- [Tiles y formatos de bitplane](SNES/graficos/tiles.md)
- [Sprites (OAM)](SNES/graficos/sprites.md)
- [Paletas y CGRAM](SNES/graficos/palettes.md)
- [Ventanas de recorte (windows)](SNES/graficos/windows.md)
- [Offset-per-tile](SNES/graficos/offset-per-tile.md)
- [Color math](SNES/graficos/color-math.md)
- [Interrupciones VBlank (NMI)](SNES/graficos/vblank-interrupts.md)
- [Rutina de VBlank (patrones prácticos)](SNES/graficos/vblank-routine.md)
- [PPU de SNES para desarrolladores de NES](SNES/graficos/ppu-para-desarrolladores-nes.md)
- [Scroll de un mapa grande (streaming de tilemap)](SNES/graficos/scroll-mapa-grande.md)
- [Dibujar formas con las ventanas (windows) de la PPU](SNES/graficos/formas-de-ventana.md)
- [HDMA — ejemplos prácticos](SNES/graficos/hdma-ejemplos.md)
- [Leer y escribir memoria de la PPU (VRAM, CGRAM, OAM)](SNES/graficos/leer-escribir-memoria-ppu.md)
- [Modo 7 — transformación afín y efectos de perspectiva](SNES/graficos/modo-7-efectos-perspectiva.md) ⭐

**sonido/**
- [S-SMP — el procesador de sonido del SNES](SNES/sonido/s-smp.md)
- [Set de instrucciones del SPC-700](SNES/sonido/spc700-instrucciones.md)
- [Registros del S-DSP](SNES/sonido/s-dsp-registers.md)
- [Envolventes del S-DSP (ADSR y GAIN)](SNES/sonido/dsp-envelopes.md)
- [Formato de muestras BRR](SNES/sonido/brr-samples.md)
- [Arranque del SPC700 (protocolo IPL)](SNES/sonido/arranque-spc700.md)
- [Técnica de subida "Blargg SPC upload"](SNES/sonido/blargg-spc-upload.md)

**memoria/**
- [Mapa de memoria (LoROM / HiROM / ExHiROM)](SNES/memoria/memory-map.md)
- [Cabecera de la ROM (SNES)](SNES/memoria/rom-header.md)
- [Registros de DMA y HDMA](SNES/memoria/dma-registers.md)
- [DMA/HDMA — ejemplos de uso](SNES/memoria/dma-ejemplos.md)
- [Formatos de archivo de ROM](SNES/memoria/formatos-archivo-rom.md)
- [Formatos de archivo de guardado (SRM y savestates)](SNES/memoria/formatos-archivo-save.md)
- [Registros MMIO del CPU (fuera del PPU)](SNES/memoria/mmio-registers.md)

**perifericos/**
- [Mando estándar de SNES](SNES/perifericos/mando-estandar.md)
- [Ratón de SNES (Super NES Mouse)](SNES/perifericos/raton.md)
- [Multitap de SNES](SNES/perifericos/multitap.md)
- [Super Scope (pistola de luz de SNES)](SNES/perifericos/super-scope.md)
- [NTT Data Keypad (NDK10) de Super Famicom](SNES/perifericos/ntt-data-keypad.md)
- [Copiers de cartuchos SNES](SNES/perifericos/copier.md)
- [Turbo File Twin (ASCII Corporation)](SNES/perifericos/turbo-file-twin.md)
- [Lectura de mando en SNES (protocolo serie)](SNES/perifericos/lectura-de-mando.md)
- [Conector de cartucho de SNES (62 pines)](SNES/perifericos/conector-cartucho.md)
- [Conector de mando de SNES (puerto de controlador)](SNES/perifericos/conector-mando.md)

**herramientas/**
- [Herramientas recomendadas para desarrollo en SNES](SNES/herramientas/tools.md)
- [Timing general del sistema SNES](SNES/herramientas/timing.md)
- [Erratas y quirks conocidos del hardware SNES](SNES/herramientas/errata.md)
- [Glosario de términos de desarrollo SNES](SNES/herramientas/glosario.md)
- [Manual de desarrollo oficial de Nintendo para SNES](SNES/herramientas/manual-desarrollo-nintendo.md)
- [Tutoriales recomendados para desarrollo en SNES](SNES/herramientas/tutoriales.md)
- [ROMs de test para verificar precisión de emuladores de SNES](SNES/herramientas/tests-emuladores.md)
- [Juegos de SNES difíciles de emular con precisión](SNES/herramientas/juegos-dificiles-de-emular.md)
- [Juegos de SNES con modos gráficos inusuales](SNES/herramientas/juegos-modos-graficos-inusuales.md)

## Motores — arquitectura y diseño (multiplataforma)

- [MOTORES/](MOTORES/README.md) — conocimiento de arquitectura de motores NO ligado a una consola concreta; referencia para diseñar una segunda versión de motor-cenital y futuros motores. Fuente inicial: [fabiensanglard.net](https://fabiensanglard.net/).

**arquitectura/** — la serie "Another World" documenta el MISMO motor de polígonos 2D portado a 7 plataformas
- [Another World — arquitectura general del motor (VM y bytecode)](MOTORES/arquitectura/another-world-overview.md)
- [Another World en Amiga 500 — Blitter, bitplanes y renderizado de polígonos](MOTORES/arquitectura/another-world-amiga.md)
- [Another World en Atari ST — rasterizado de polígonos por software sin Blitter](MOTORES/arquitectura/another-world-atari-st.md)
- [Another World — el port a IBM PC/DOS](MOTORES/arquitectura/another-world-pc-dos.md)
- [Another World — el port a Sega Genesis / Mega Drive](MOTORES/arquitectura/another-world-genesis.md) ⭐
- [Another World — el port a Super Nintendo (SNES)](MOTORES/arquitectura/another-world-snes.md)
- [Another World — el port a Game Boy Advance](MOTORES/arquitectura/another-world-gba.md)
- [Another World — el port a Atari Jaguar](MOTORES/arquitectura/another-world-jaguar.md)
- [Timers de juego — problemas y soluciones (bucle principal y framerate)](MOTORES/arquitectura/game-timers-framerate.md)
- [Coma flotante explicada visualmente](MOTORES/arquitectura/floating-point-visually-explained.md)

**efectos/**
- [Cómo se hizo el efecto de fuego de DOOM (PSX)](MOTORES/efectos/doom-fire-psx.md)
- [FizzleFade — la transición de pantalla de Wolfenstein 3D](MOTORES/efectos/fizzlefade.md)

**graficos-hardware/**
- [Por qué el sistema PPU de la SNES se diseñó así](MOTORES/graficos-hardware/snes-ppu-por-que.md)
- [Cómo funciona el sistema gráfico de la SNES (PPU1/PPU2)](MOTORES/graficos-hardware/snes-ppu-como.md)
- [Diseñando el sistema de vídeo de la SNES](MOTORES/graficos-hardware/snes-video-diseno.md)
- [Estudio del sistema gráfico de la Capcom CPS-1](MOTORES/graficos-hardware/cps1-gfx-system.md)
- [CPS-1 Sound System](MOTORES/graficos-hardware/cps1-sound-system.md)
- [Street Fighter II: Rotating and Scaling](MOTORES/graficos-hardware/sf2-rotacion-escalado.md)
- [Street Fighter II: Health Bar](MOTORES/graficos-hardware/sf2-health-bar.md)

*(No se creó entrada para los "Game Engine Black Books" de Wolfenstein 3D/DOOM: las páginas de fabiensanglard.net son solo anuncios de venta del libro, sin contenido técnico gratuito.)*

## Cómo añadir una entrada al índice

Al crear una entrada nueva a partir de `_plantilla.md`, añade aquí una línea bajo la plataforma/categoría correspondiente:

```
- [Título de la entrada](MegaDrive/graficos/nombre-archivo.md) — descripción corta
```

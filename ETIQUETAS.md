# Etiquetas de RetroPedia

Lista cerrada de 50 etiquetas para el campo `tags:`. El validador (`Tools\retropedia-validar`) rechaza cualquier otra. Si de verdad falta
una, se añade aquí en el mismo cambio.

Las etiquetas agrupan temas; los nombres concretos (`hitpause`, `SPR_addSprite`) se buscan con Grep en el texto.
La plataforma va en `plataforma:` y el autor en `fuentes:`, no aquí.

## Gráficos

| Etiqueta | Para | Incluye |
|---|---|---|
| `sprites` | Sprites de hardware y metasprites | OAM, SCB, tabla de sprites |
| `tiles` | Tiles, tilemaps, planos y fuentes | nametable, baldosas, FNT |
| `paletas` | Color y paletas | CRAM, CGRAM, cuantización |
| `scroll` | Scroll, parallax y cámara por hardware | mapas, VSRAM, offset-per-tile |
| `vram` | Memoria de vídeo y su presupuesto | caché, mapa de VRAM |
| `dma` | DMA y HDMA | transferencias, colas |
| `efectos` | Efectos visuales | transparencia, sombra, modo 7, rotación, fundidos |
| `animacion` | Animación y fotogramas | AIR, sprites animados |

## Hardware

| Etiqueta | Para | Incluye |
|---|---|---|
| `vdp` | VDP de Mega Drive | registros y comandos |
| `ppu` | PPU de SNES | registros, ventanas |
| `powervr` | GPU de Dreamcast y sus texturas | TA, PVR, twiddled, VQ |
| `68000` | CPU 68000 | instrucciones |
| `z80` | CPU Z80 | comunicación con la principal |
| `65816` | CPU de SNES | 6502, S-CPU |
| `sh4` | CPU de Dreamcast | FPU, store queues |
| `registros` | Mapas de registros y E/S | MMIO, puertos |
| `memoria` | Mapa de memoria, RAM y guardado | bancos, mappers, SRAM, VMU |
| `interrupciones` | Interrupciones y temporización | vblank, hblank, timing |
| `rom` | Cartucho, cabecera y formato de ROM | checksum, padding, pines |
| `mando` | Mandos | joypad, Maple, multitap |
| `perifericos` | Otros periféricos | ratón, teclado, pistola, impresora |
| `arranque` | Arranque y disco | TMSS, IP.BIN, BIOS, CDI |

## Sonido

| Etiqueta | Para | Incluye |
|---|---|---|
| `audio` | Sonido en general | drivers, muestras, PCM, ADPCM, AICA |
| `fm` | Síntesis FM y sus chips | YM2612, instrumentos |
| `psg` | PSG y ondas simples | SN76489, chiptune |
| `spc700` | Sonido de SNES | S-SMP, S-DSP, BRR |

## Programación

| Etiqueta | Para | Incluye |
|---|---|---|
| `asm` | Ensamblador | ensambladores, opcodes |
| `optimizacion` | Rendimiento y ciclos | benchmark, fillrate |
| `matematicas` | Matemáticas | punto fijo, trigonometría, tablas, azar |
| `colisiones` | Colisiones y física | cajas, máscaras, pendientes, gravedad |
| `formatos` | Formatos de fichero y compresión | SFF, VGM, LZ, Nemesis |
| `arquitectura` | Diseño de motores | game loop, VM, polígonos, IA del juego |

## Herramientas

| Etiqueta | Para | Incluye |
|---|---|---|
| `sgdk` | SGDK | rescomp |
| `md-engine` | MD Engine | — |
| `kos` | KallistiOS y DreamSDK | — |
| `ngdevkit` | ngdevkit | GnGeo |
| `toolchain` | Compiladores y compilación | GCC, makefile, MSYS2 |
| `emuladores` | Emuladores | BlastEm, Flycast, Mesen |
| `depuracion` | Depurar y probar | log, savestate, QR, regresión, síntomas |
| `herramientas` | Editores y otras herramientas | Aseprite, Furnace, scripts |

## Temas y tipo de entrada

| Etiqueta | Para | Incluye |
|---|---|---|
| `mugen` | MUGEN | CNS, Ikemen, Dolmexica |
| `port` | Portar o hacer un demake | migración, casos reales |
| `diseno` | Diseño de niveles y de mundo | metroidvania, métricas |
| `extraccion` | Sacar assets de un vídeo | OpenCV, segmentación con IA |
| `proceso` | Cómo se organiza un proyecto | ROADMAP, diario, publicación |
| `limites` | Límites y presupuestos de una máquina | — |
| `tutorial` | Paso a paso | hello world, receta |
| `caso-practico` | Experiencia de un proyecto real | — |
| `referencia` | Índices y material de consulta | overview, wiki, foros |
| `historia` | Cómo lo hizo un juego clásico | ingeniería inversa |

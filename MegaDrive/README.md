---
titulo: Mega Drive / Genesis — overview
plataforma: [MegaDrive]
categoria: [general]
tags: [hardware, overview]
estado: borrador
fecha: 2026-07-21
---

# Mega Drive / Genesis

## Resumen de hardware (a validar/ampliar durante el proyecto)

- CPU principal: Motorola 68000 (~7.6 MHz)
- CPU secundaria: Zilog Z80 (control de sonido, compatibilidad Master System)
- Vídeo: VDP (Video Display Processor)
- Sonido: YM2612 (FM) + PSG SN76489
- Resoluciones habituales: 320x224 / 256x224

Esta ficha es un punto de partida — se ampliará y corregirá con detalle real a medida que trabajemos en proyectos concretos de Mega Drive.

## Categorías

- `graficos/` — VDP, tiles, sprites, paletas, scroll, efectos visuales
- `sonido/` — YM2612, PSG, drivers de sonido, formatos de instrumento
- `cpu/` — 68000 ASM, Z80 (uso y comunicación con el 68000)
- `memoria/` — mapa de memoria, bank switching, ROM, cabecera, SRAM
- `perifericos/` — mandos, ratón, multitaps, teclado, impresora, I/O
- `avanzado/` — multitarea, Mega CD (Sub-CPU, LEDs)
- `herramientas/` — formatos de compresión, toolchains, debuggers

## Entradas

Contenido inicial extraído de [plutiedev.com](https://plutiedev.com) (2026-07-21), `estado: borrador` — ver [INDEX.md](../INDEX.md) para el listado completo enlazado por categoría.

| Categoría | Nº entradas |
|---|---|
| cpu/ | 13 |
| graficos/ | 32 |
| sonido/ | 15 |
| perifericos/ | 12 |
| memoria/ | 13 |
| avanzado/ | 4 |
| herramientas/ | 16 + 9 (subcarpeta `sgdk/`) |

`herramientas/sgdk/` documenta [SGDK](https://github.com/Stephane-D/SGDK), el SDK en C más usado para Mega Drive hoy en día (alternativa de alto nivel al ASM puro de las demás entradas): instalación, estructura de proyecto/build, y los tutoriales de la wiki oficial (introducción, hello world, backgrounds, sprites, input, matemáticas). La referencia función-por-función completa vive en la documentación Doxygen oficial (enlazada desde `overview.md`) y no se ha volcado aquí.

`herramientas/spritesmind-overview.md` documenta [SpritesMind.Net](https://www.spritesmind.net/), el foro histórico de la comunidad de desarrollo Mega Drive — sus hilos técnicos (timing de bus/DMA, internals del VDP, YM2612, diferencias de hardware entre revisiones) no se pudieron extraer directamente (bucle de redirecciones), así que hay 4 entradas "índice curado" repartidas por categoría (`graficos/spritesmind-vdp-hilos.md`, `sonido/spritesmind-ym2612-hilos.md`, `cpu/spritesmind-bus-timing-hilos.md`, `memoria/spritesmind-hardware-revisiones-hilos.md`) con enlaces directos a los hilos y resúmenes basados en fragmentos de búsqueda — no son transcripciones completas, hay que abrir el hilo para el detalle fino.

`herramientas/sonic-retro-disassemblies.md` documenta los disassemblies de juegos comerciales de Sonic Retro (Sonic 1/2/CD/&Knuckles, Kid Chameleon, Chaotix, Ristar...) — la wiki índice (`info.sonicretro.org`) y la wiki hermana `segaretro.org` dan 403, así que el listado se reconstruyó desde la organización de GitHub `sonicretro`. De paso se documentaron dos temas transversales muy usados en juegos comerciales reales (no solo homebrew): [`sonido/smps-sound-driver.md`](sonido/smps-sound-driver.md) (el driver de sonido SMPS de Sega) y [`herramientas/compresion-nemesis-kosinski-enigma.md`](herramientas/compresion-nemesis-kosinski-enigma.md) (los formatos de compresión Nemesis/Kosinski/Enigma, más extendidos en juegos reales que los SLZ/UFTC de plutiedev).

`herramientas/caso-practico-sgdk-2019.md` es un artículo de Gamedeveloper.com (Doctor Ludos, 2019) con la experiencia real de publicar juegos homebrew en cartucho con SGDK — complementa los tutoriales oficiales con límites de hardware "de verdad" encontrados en producción (presupuesto de ~7KB/frame para transferencias a VRAM, 80 sprites máx., 64 colores simultáneos de 512).

`herramientas/segaretro-overview.md` documenta [Sega Retro](https://segaretro.org/Category:Mega_Drive_technical_information) (wiki, 403 igual que spritesmind/Sonic Retro) — de sus 14 páginas de "Mega Drive technical information" solo se pudo extraer contenido real vía WebSearch para dos temas nuevos: [`graficos/segaretro-interrupts.md`](graficos/segaretro-interrupts.md) (niveles de interrupción VInt/HInt/external y semántica del contador HInt) y [`memoria/segaretro-quirks.md`](memoria/segaretro-quirks.md) (quirk de `clr` sobre el VDP y de la instrucción `TAS`). Las otras 12 páginas se solapan con contenido ya existente de plutiedev y quedan como referencia pendiente (enlaces en `segaretro-overview.md`).

Dos huecos identificados el 2026-07-21 ya rellenados: [`cpu/68000-instrucciones-modos-direccionamiento.md`](cpu/68000-instrucciones-modos-direccionamiento.md) (base de instrucciones/modos de direccionamiento del 68000, que las demás entradas de `cpu/` daban por sabida) y [`herramientas/build-debug-asm-puro.md`](herramientas/build-debug-asm-puro.md) (ensambladores como vasm/ASM68K/WLA-DX, proceso de build sin SGDK, y comandos de depuración de BlastEm/Gens KMod/DGen/Exodus).

**13 entradas nuevas del [blog de Big Evil Corporation](https://blog.bigevilcorporation.co.uk/)** (desarrollador de TANGLEWOOD, juego comercial real para Mega Drive), con prefijo `bec-`: la serie completa de 10 tutoriales de 2012 (toolchain, bases de ASM 68000, arranque de hardware, hello world, fuentes, sprites y su animación, mapas/scroll, PSG, mando/game loop) — un segundo enfoque autoral de raíz distinto de plutiedev.com para gran parte del mismo terreno, útil para contrastar explicaciones — más dos relatos reales de depuración en hardware clon AtGames (bug de pitch de audio en el Firecore, bug del emulador de Z80 en el Flashback) que amplían con detalle de primera mano lo que `memoria/hardware-issues.md` ya mencionaba de pasada. La entrada del taller de 2018 (`bec-workshop-2018.md`) quedó marcada como mayormente redundante con "Getting Started" — sin código propio, solo un anuncio con enlaces.

**12 entradas nuevas del [blog de Bumbershoot Software](https://bumbershootsoft.wordpress.com/platform-guide-sega-genesis-mega-drive/)** (prefijo `bumbershoot-`), un tercer enfoque autoral independiente de plutiedev.com y Big Evil Corporation sobre gran parte del mismo terreno, más algunas piezas nuevas: `herramientas/` (panorama de hardware/toolchains, comparativa de emuladores centrada en debugging —BizHawk, JGenesis—, comparativa de ensambladores), `cpu/` (Hello World anotado en ASM puro con expansión de fuente 1bpp→4bpp), `avanzado/` (disección del startup code real usado en ROMs comerciales de Sega, con las "anomalías" que el propio autor documenta: stack Z80 mal inicializado, chip de sonido no-legacy sin resetear), `graficos/` (VDP desde cero, pipeline completo de cuantización/dithering/compresión LZ4 para arte de un logo, scroll de pantalla completa por VBLANK, fundido de paleta + efecto "pixel bleed" por VSRAM), `perifericos/` (lectura de mando de 3 botones con el truco `asl`/`asr` para extraer el bit de Start) y `sonido/` (driver de música PSG en Z80 con decaimiento de subframe, reproducción de PCM por el YM2612 con temporización por ciclos). Varios bloques de la fuente se descartaron explícitamente por solapar 1:1 con contenido ya existente sin aportar nada nuevo: la lista de "recursos online" de la guía principal (ya cubiertos por `spritesmind-overview.md`/`segaretro-overview.md`/`kabuto-hardware-notes.md`), la sección de sprites del artículo de mando/sprites (sin código, ya cubierta con más detalle en `sprites.md`/`sprite-table.md`/`bec-sprites.md`), y el artículo sobre el chip YM2612 orientado a música (relato de diseño personal sin registros ni código, sin nada no cubierto ya en `ym2612-operations.md`/`ym2612-registers.md`).

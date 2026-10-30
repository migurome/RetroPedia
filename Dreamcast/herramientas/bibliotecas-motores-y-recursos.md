---
titulo: Bibliotecas, motores y recursos — kos-ports, GLdc, Simulant, SH4ZAM y comunidad
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [kos-ports, gldc, aldc, simulant, sh4zam, raylib, sdl, dolmexica, openbor, recursos, comunidad]
estado: borrador
fecha: 2026-10-30
fuentes: [https://www.dreamcast.wiki/Engine_%26_Library, https://www.dreamcast.wiki/Graphics_APIs, https://www.dreamcast.wiki/GLdc, https://github.com/Kazade/GLdc, https://gitlab.com/simulant/simulant, https://github.com/KallistiOS/kos-ports, https://github.com/dreamcastdevs/awesome-dreamcast, https://github.com/dreamcastdevs/dreamcast_tutorial, https://dreamsdk.org/, https://mc.pp.se/dc/, https://dreamcastdev.blogspot.com/]
---

# Bibliotecas, motores y recursos

## Resumen

Casi todo el homebrew moderno de Dreamcast se apoya en **KallistiOS** y en las bibliotecas de **kos-ports**. Por encima hay tres formas de dibujar (por software, GLdc y la API PVR de KOS) y varios motores. Esta entrada es el mapa del ecosistema: qué existe, para qué sirve y dónde está la documentación. También dice qué fuentes de la comunidad valen la pena y cuáles no.

## Detalle técnico

### Cómo dibujar: tres opciones

| | Software (CPU) | GLdc (OpenGL) | API PVR de KOS |
|---|---|---|---|
| Qué es | escribir en el framebuffer (`vram_s`) | OpenGL 1.2 parcial, *fixed pipeline*, sobre el PVR | acceso directo al PowerVR |
| Velocidad | lenta | rápida (capa fina sobre el PVR) | la más rápida en casos concretos |
| Portabilidad | — | multiplataforma, muchos tutoriales | solo Dreamcast |
| Pega | hay que escribir todo el render | sin shaders. Algunas cosas encajan mal: el PVR no tiene z-buffer clásico, el *scissor* es incómodo | hay que hacer a mano lo básico (recorte de polígonos...) y probar mucho para que vaya rápido |

Para un juego 2D, la API PVR con sprites en listas PT/TR es lo habitual (ver [PowerVR](../graficos/powervr-pipeline.md) y la [caché de sprites](../graficos/cache-sprites-vram.md)). GLdc compensa al portar código OpenGL existente.

### kos-ports

Gestor de paquetes de bibliotecas portadas. Cada port se instala con `make install clean` en su carpeta (o todos con `utils/build-all.sh`). También tiene los objetivos `uninstall` y `portinfo`. Las dependencias se descargan solas y el resultado queda en `kos-ports/include` y `kos-ports/lib`. Lista actual:

- **Gráficos/3D:** `libGL` (= GLdc), `libKGL` (el OpenGL antiguo de KOS), `raylib4dc`, `SDL`, `SDL_ttf`, `cglm`, `sh4zam`, `libparallax`, `libtsunami`
- **Imagen:** `libpng`, `libjpeg`, `libkmg`, `libpcx`, `libtga`, `stb_image`, `libimageload`, `freetype`
- **Audio/vídeo:** `libAL` (ALdc, OpenAL), `libADX`, `libmp3`, `libogg`, `libvorbis`, `libtremor`, `liboggvorbisplay`, `opus`, `opusfile`, `libopusplay`, `libwav`, `libmodplug`, `miniaudio`, `libmpeg`
- **Compresión/datos:** `zlib`, `libbz2`, `libzip`, `expat`, `libyaml`, `libcmark`
- **Scripting:** `lua`, `micropython`, `mruby`, `libjimtcl`
- **Red:** `curl`, `mbedtls`, `polarssl`, `libbearssl`, `libsmb2`, `lftpd`
- **Otros:** `libchipmunk` (física 2D), `libconio`, `libkosh`, `libdcplib`, `libfastmem`, `libvldmail`

Para añadir un port, la recomendación es copiar la estructura de uno existente (`libpng`).

### Bibliotecas destacadas

- **GLdc** (Kazade, BSD-2-Clause; desarrollo en `gitlab.com/simulant/GLdc`): implementación parcial de OpenGL 1.2, nacida como fork del libGL de Josh Pearson y reescrita casi entera. Tiene un *backend* `kospvr` (hardware) y otro `software` para pruebas en PC. Se compila con CMake: `cmake -DCMAKE_TOOLCHAIN_FILE=../toolchains/Dreamcast.cmake ..`. Pendiente: spotlights, framebuffer extension, texture matrix. Lo usan Simulant, nuQuake y Summoning Signals.
- **ALdc**: OpenAL 1.2 del mismo autor.
- **SH4ZAM** (Falco Girgis): matemáticas y matrices aceleradas para el SH-4 (FIPR, FTRV, FSRRA; ver [optimización SH-4](../cpu/sh4-optimizacion.md)). Está en kos-ports.
- **DreamHAL**: capa de abstracción del SH-4 con matemáticas rápidas.
- **libGimbal**: biblioteca estándar extendida, tests unitarios y utilidades.
- **DreamcastAicaSoundDriver** (Aurelien34): driver de sonido para el ARM del AICA con S3M y efectos.
- **DreamROQ / libdreamroq**: reproductor de vídeo ROQ (la versión librería tiene sonido).
- **LibFatFS**: FAT16/FAT32 para el adaptador SD.
- **Crayon-Utilities**: iconos de VMU y conversores de texturas.

### Motores

| Motor | Qué es |
|---|---|
| **Simulant** | motor 2D/3D de propósito general para Linux, Android y Dreamcast (LGPLv2.1, `gitlab.com/simulant/simulant`) |
| **DolmexicaInfinite** | motor tipo MUGEN para juegos de lucha (CaptainDreamcast). **Relevante para [MOTORES/mugen](../../MOTORES/mugen/cns-compilacion.md)**: es otra forma de llevar contenido MUGEN a la consola |
| **DreamBOR** | port no oficial de OpenBOR (beat'em ups), mejorado por Ian Micheal |
| **Antiruins** | motor 2D mínimo con scripting en Lua |
| **nuQuake**, **RADquake** | motores Quake |
| **raylib4dc** | raylib sobre GLdc (ejemplo `raytris` en KOS) |
| **BennuGD** | motor multiplataforma |
| **Prism** | utilidades de CaptainDreamcast: física, carga de ficheros... (la wiki lo marca "sin probar") |

Proyectos grandes que sirven de referencia de código: ClassiCube, el port de Super Mario 64 (MrNeo), FNF-DC (raylib), DreamShell (sistema alternativo para cargar desde SD/IDE) y 240pTestSuite (pruebas de vídeo en hardware).

### Documentación y comunidad

| Recurso | Para qué |
|---|---|
| [dreamcast.wiki — Development](https://www.dreamcast.wiki/Development) | **punto de partida**: índice de guías (toolchain, gráficos, VMU, optimización). Varias páginas del índice están aún sin escribir |
| [kos-docs.dreamcast.wiki](https://kos-docs.dreamcast.wiki/) | referencia Doxygen de la API de KOS. Para el detalle exacto, mejor las cabeceras locales en `kos/kernel/arch/dreamcast/include/dc/` |
| [mc.pp.se/dc](https://mc.pp.se/dc/) | documentación de hardware de Marcus Comstedt (mapa de memoria, PVR, Maple, IP.BIN, syscalls). Es la base de varias entradas de esta sección |
| [awesome-dreamcast](https://github.com/dreamcastdevs/awesome-dreamcast) | lista curada y bastante completa de bibliotecas, motores, juegos y herramientas |
| [dreamcast_tutorial](https://github.com/dreamcastdevs/dreamcast_tutorial) | tutorial por capítulos: toolchain, GLdc, mando, sonido, VMU, gestión de recursos, CDI |
| Foro de DCEmulation (`dcemulation.org/phpBB`), Discord de Simulant, IRC `#dreamcastdev` en Libera | soporte de la comunidad |
| [dreamsdk.org](https://dreamsdk.org/) | instalador para Windows |

## Notas / gotchas

- **dcemulation.org devuelve 403** a la herramienta de fetch (el mismo bloqueo anti-bot que segaretro.org o spritesmind). Su antigua wiki de desarrollo se trasladó en buena parte a dreamcast.wiki, así que no se pierde mucho.
- **dreamsdk.org anuncia "R3"**, pero el instalador probado en este proyecto es R4 (build 4.0.11.2508, ver [DreamSDK en Windows](dreamsdk-windows.md)). La portada está desactualizada. Además pide Python 2.7 de 32 bits para el `sh-elf-gdb`.
- **dreamcastdev.blogspot.com** (HighwayTofu, agosto de 2018) se revisó y **no se ha volcado**: son seis entradas básicas (triángulo con PVR y con OpenGL, mando, pirámide 3D con errores sin corregir) que la wiki y los ejemplos de KOS cubren mejor y más al día.
- **r/dreamcast** y **dreamcast-talk.com** son comunidades generales de la consola (juegos, hardware, modding), no fuentes técnicas de desarrollo: no se han extraído.
- En el repositorio de GitHub `Kazade/simulant` da 404: el proyecto vive en GitLab.

## Referencias cruzadas

- [Toolchain de KallistiOS](toolchain-kos.md)
- [Dreamcast — overview](../README.md)
- [MUGEN: compilar la lógica CNS](../../MOTORES/mugen/cns-compilacion.md)

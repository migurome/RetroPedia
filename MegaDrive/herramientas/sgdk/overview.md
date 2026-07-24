---
titulo: SGDK — Sega Genesis Development Kit (overview)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, c, sdk, toolchain, gcc, rescomp]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK, https://github.com/Stephane-D/SGDK/wiki]
---

# SGDK — Sega Genesis Development Kit

## Resumen

SGDK es un kit de desarrollo gratuito y de código abierto (licencia MIT; los componentes GCC usan GPL3 con excepción de runtime) que permite programar para Mega Drive/Genesis en lenguaje C. Incluye una librería (con su propio código fuente), herramientas de compilación de recursos propias, y usa el compilador GCC (target m68k-elf) para generar la ROM final. Es, junto con el ensamblador puro (ver entradas de `plutiedev.com` en esta misma carpeta), una de las dos vías principales para desarrollar homebrew de Mega Drive hoy en día — mucho más accesible que ASM puro.

## Detalle técnico

### Requisitos

- Conocimientos de C (imprescindible antes de empezar; también se puede mezclar con ensamblador, pero no es necesario).
- Conocimiento básico de la arquitectura de Mega Drive, en particular el sistema de vídeo (se recomienda complementar con fuentes como Raster Scroll y el "Genesis Software Manual" — ver entradas de plutiedev en esta base).
- Java instalado (las herramientas de compilación de recursos, como `rescomp`, están escritas en Java).
- GCC (se distribuyen binarios para Windows; en otros sistemas operativos hay que instalarlo manualmente).

### Estructura del repositorio

- `src/` — código fuente de la librería SGDK.
- `inc/` — cabeceras (headers) de la librería.
- `lib/` — librerías compiladas (`libmd.a`).
- `sample/` — proyectos de ejemplo, incluyendo una demo de Sonic bastante completa.
- `res/` — recursos.
- `tools/` — herramientas propias de compilación (incluye `rescomp`).
- `doc/` — documentación (incluye `rescomp.txt`, que explica los tipos de recursos y cómo declararlos).
- `project/template/` — plantilla de proyecto para VSCode.

### Filosofía: desarrollo orientado a recursos

SGDK depende fuertemente del concepto de **recursos** (`resources`), compilados mediante la herramienta `rescomp` a partir de archivos `.res`. Esto cubre desde tilesets/mapas hasta sprites y música — ver [Uso de SGDK](usage.md) y las entradas de tutorial de esta misma carpeta para la sintaxis concreta.

### Documentación

- Wiki oficial: https://github.com/Stephane-D/SGDK/wiki — instalación, integración con IDEs (Eclipse, Code::Blocks, QtCreator) y una serie de tutoriales ("Start Coding").
- Documentación Doxygen online: http://stephane-d.github.io/SGDK/ — referencia completa y generada automáticamente de todas las estructuras y funciones de la librería. **Es la fuente de verdad para cualquier función/struct concreta** — las entradas de esta carpeta cubren solo los tutoriales conceptuales de la wiki, no la referencia función por función. Cuando se necesite el detalle exacto de una función durante el desarrollo de un proyecto, consultar directamente esa documentación (o las cabeceras en `inc/`) en vez de fiarse solo de estas notas.
- `rescomp.txt` (dentro del repo) — referencia de tipos de recursos y su sintaxis de declaración.

### Comunidad y soporte

- Servidor de Discord de SGDK.
- Foro Spritesmind, dedicado al desarrollo para Mega Drive.
- Extensión "Genesis-Code" para VSCode.

## Notas / gotchas

- Esta base solo documenta de momento los tutoriales de la wiki ("Start Coding": introducción, hello world, backgrounds, sprites, input, matemáticas) y las páginas de instalación/uso. Las guías de integración con IDEs concretos (Eclipse, Code::Blocks, QtCreator) no se han extraído por ser específicas de entorno/versión y quedar desactualizadas rápido — consultar la wiki directamente si se necesita.
- El debugging con GDB tiene limitaciones en emuladores; SGDK recomienda el sistema de logging de mensajes de Gens KMod (`KLog`) y validar el comportamiento en emuladores precisos (BlastEm) o hardware real.

## Referencias cruzadas

- [Instalación de SGDK](installation.md)
- [Uso de SGDK (estructura de proyecto y build)](usage.md)
- [Tutorial: Introducción](tuto-introduction.md)
- [Tutorial: Hello World](tuto-hello-world.md)
- [Tutorial: Backgrounds](tuto-background.md)
- [Tutorial: Sprites](tuto-sprites.md)
- [Tutorial: Input](tuto-input.md)
- [Tutorial: Matemáticas](tuto-maths.md)
- (pendiente de enlazar con las entradas de hardware crudo de `plutiedev.com` cuando se identifiquen equivalencias, p.ej. `graficos/tiles-and-palettes.md` vs. el sistema de recursos MAP/TILESET de SGDK)

---
titulo: Inventario de herramientas compartidas — Desktop\Tools y SDK instalados
plataforma: [General]
categoria: [herramientas]
tags: [herramientas, toolchain, emuladores, proceso, referencia]
estado: revisado
comprobado: pc
comprobado_en: "parcial: versiones y rutas leídas en este PC el 2026-11-06; los comandos marcados con ✔ se ejecutaron ese día"
fecha: 2026-11-06
fuentes: [C:\Users\migur\Desktop\Tools, C:\sgdk\changelog.txt, Tools/ngdevkit-master/ngdevkit-master/configure.ac, Dreamcast/herramientas/flycast-automatizado.md, NeoGeo/herramientas/ngdevkit-windows.md]
---

# Herramientas compartidas

## Resumen

Qué hay instalado, para qué sirve y cómo se llama. La norma de uso (por ruta, sin copiarlas a cada proyecto) está en las
[reglas comunes](reglas-comunes-de-proyecto.md). ✔ = comando ejecutado aquí.

## Detalle técnico

### Emuladores

| Herramienta | Ruta | Para qué | Comando típico | Limitaciones |
|---|---|---|---|---|
| BlastEm 1.0.0 | `Tools\blastem-win64-1.0.0` | Mega Drive preciso, con depurador | `blastem.exe rom.bin` ✔; opciones con `-h` | Sin modo sin ventana comprobado |
| Gens 2.14 | `Tools\Gens_v2.14` | Contrastar con un emulador antiguo | `gens.exe`, abrir la ROM | Menos preciso: que funcione aquí no demuestra nada |
| Flycast 2.7 | `Tools\flycast-win64-2.7` | Dreamcast; `flycast.lua` automatiza | `flycast.exe C:\ruta\absoluta\juego.cdi` ✔ | No emula la velocidad del GD-ROM. Carpeta compartida: ver abajo |
| Flycast (fuentes) | `Tools\flycast-master` | Consultar cómo emula algo | No se compila | Versión de las fuentes sin anotar |
| Mesen 2 | Paquete de WinGet `SourMesen.Mesen2` | SNES, con depurador | Accesos `Mesen.url` del escritorio | Fuera de `Tools` |
| GnGeo (ngdevkit) | `C:\msys64\ucrt64\bin\ngdevkit-gngeo.exe` | Neo Geo | Ver [ngdevkit](../NeoGeo/herramientas/ngdevkit-windows.md) | Fuera de `Tools` |

### SDK y compiladores

| Herramienta | Ruta | Para qué | Comando típico | Limitaciones |
|---|---|---|---|---|
| SGDK 2.11 | `C:\sgdk` (y otra copia en `C:\Users\migur\sgdk`) | Mega Drive en C, GCC 13.2 | `GDK=C:/sgdk make -f C:/sgdk/makefile.gen` ✔ | `make` no admite espacios en la ruta del kit. `rescomp` necesita Java |
| DreamSDK R4 | `C:\DreamSDK`; instalador en `Tools\DreamSDK-R4-Setup` | KOS, GCC 13.2 | Ver el [snippet](../snippets/README.md) ✔ | Sin espacios en la ruta; `makeip.exe` no arranca. [Trampas](../Dreamcast/herramientas/dreamsdk-windows.md) |
| ngdevkit 0.5 | MSYS2 en `C:\msys64`, GCC 15.3; fuentes en `Tools\ngdevkit-master` | Neo Geo | `m68k-neogeo-elf-gcc --version` ✔ | El SDK es el de MSYS2, no el de `Tools` |

No hay toolchain de SNES instalado.

### Editores y sonido

| Herramienta | Ruta | Para qué | Comando típico | Limitaciones |
|---|---|---|---|---|
| Aseprite 1.3.17.2 | `Tools\Aseprite-v1.3.17.2-x64` | Sprites, paletas y animaciones | `Aseprite.exe --version` ✔; `-b` para exportar por lotes | — |
| Furnace 0.6.8.3 | `Tools\furnace-0.6.8.3-win32` | Música y efectos | Ver [Furnace a VGM](../MegaDrive/sonido/furnace-fur-generar-y-exportar-vgm.md) | Binario de 32 bits |

### Propias

| Herramienta | Ruta | Para qué | Comando típico |
|---|---|---|---|
| Validador de RetroPedia | `Tools\retropedia-validar` | Frontmatter, enlaces, índice, recuentos y etiquetas | `python validar.py` ✔; pruebas: `python test_validar.py` ✔ |
| PXDraw, PXExtractor | `Desktop\PXDraw`, `Desktop\PXExtractor` | Repositorios propios, usados en varios proyectos | Ver cada repositorio |

### Otros ficheros de `Tools`

`Resources.txt` tiene enlaces a The Spriters Resource y VGMRips. `Sprite-0001.svg` y
`Gemini_Generated_Image_*.jpg` son sueltos, sin uso conocido.

## Notas / gotchas

- **Flycast comparte carpeta entre proyectos:** `flycast.lua` y `emu.cfg` están junto al ejecutable. Antes de lanzarlo,
  mira si hay otro abierto: `Get-Process flycast`.
- **Cada proyecto, su número de producto en `IP.BIN`:** Flycast guarda la VMU por ese número (`data/T-00000_vmu_save_A1.bin`), así que dos juegos con el mismo comparten fichero. Hyper DBZ usa `T-00000`, y el snippet, `T-99901`.
- **Capturar por título engaña:** con dos Flycast abiertos, buscar la ventana `Flycast - ` puede devolver la del otro
  proyecto (2026-11-06: se capturó `hdbz.2.053` en vez del snippet). Busca por el PID que has lanzado.
- **Dos copias de SGDK:** `SSB Port` usa `C:\sgdk`. Las dos son 2.11.
- **SSB Port lleva su propio BlastEm:** es lo que las reglas evitan en proyectos nuevos.

## Referencias cruzadas

- [Reglas comunes de proyecto](reglas-comunes-de-proyecto.md)
- [Flycast automatizado](../Dreamcast/herramientas/flycast-automatizado.md)
- [Emuladores para desarrollar en Mega Drive](../MegaDrive/herramientas/bumbershoot-emuladores-desarrollo.md)

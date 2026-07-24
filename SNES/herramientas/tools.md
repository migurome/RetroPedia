---
titulo: Herramientas recomendadas para desarrollo en SNES
plataforma: [SNES]
categoria: [herramientas]
tags: [ensambladores, compiladores, emuladores, debugging, toolchain, graficos]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Tools]
---

# Herramientas recomendadas para desarrollo en SNES

## Resumen

Página índice de la SNESdev Wiki que recopila el software recomendado por la comunidad para programar en SNES: ensambladores, compiladores/toolchains, frameworks, herramientas de debugging, utilidades de sonido y gráficos, herramientas web específicas, gestión de ROMs y emuladores. Sirve como punto de partida para montar un entorno de desarrollo.

## Detalle técnico

### Ensambladores

- **64tass** — ensamblador 6502/65816. https://tass64.sourceforge.net/
- **asar** — ensamblador para SNES orientado a parches (patch-oriented). https://github.com/RPGHacker/asar
- **bass** — ensamblador de Near basado en tablas, con soporte para 65816 y SPC-700 de SNES. https://github.com/ARM9/bass
- **cc65** — incluye ensamblador 6502/65816 y compilador de C para 6502. https://cc65.github.io/
- **Free SNES Assembler** — ensamblador ligero para SNES hecho por Bisqwit. https://bisqwit.iki.fi/source/snescom.html
- **TASM** (Telemark Assembler) — ensamblador freeware basado en tablas para DOS, soporta varios procesadores; existe build para WIN64 y tabla para SPC700. https://www.ticalc.org/archives/files/fileinfo/250/25051.html
- **WLA-DX** (Wzonka-Lad Assembler Deluxe) — ensamblador multiplataforma, incluye soporte para 65816 y SPC-700 de SNES. https://github.com/vhelin/wla-dx
- **xkas-plus** — ensamblador multi-arquitectura, incluye 65816 y SPC-700 de SNES. https://github.com/devinacker/xkas-plus

Nota de la wiki: tanto ca65 como 64tass tienen muchas funciones avanzadas, pero de momento solo ca65 cuenta con soporte de debugging completo en Mesen.

### Compiladores y toolchains

- **Calypsi** — compilador de C con soporte para 65c816; requiere licencia comercial. https://www.calypsi.cc/
- **PVSnesLib** (Programmer Valuable Snes Library) — framework/toolchain de SNES con compilador de C incluido. https://github.com/alekmaul/pvsneslib
- **vbcc** — compilador de C con soporte para 65c816; requiere licencia comercial. http://www.compilers.de/vbcc.html
- **WDCTools** — paquete de desarrollo para 65c02/65c816 que incluye compilador de C y ensamblador. https://wdc65xx.com/WDCTools

### Frameworks de ensamblador

- **libSFX** — framework construido sobre ca65. https://github.com/Optiroc/libSFX

### Debugging

- **Mesen** — https://www.mesen.ca/
- **bsnes-plus** — https://github.com/devinacker/bsnes-plus
- **no$sns** — https://problemkaputt.de/sns.htm

La propia wiki recomienda Mesen explícitamente, ya que sigue en desarrollo activo, tiene más funciones y es probablemente el más preciso de los tres.

### Herramientas de sonido

- **SNES GSS** — tracker y driver de sonido/música. https://github.com/nathancassano/snesgss
- **Furnace** — tracker de chiptune. https://github.com/tildearrow/furnace
- **Terrific Audio Driver** — driver de música y herramienta de composición MML. https://github.com/undisbeliever/terrific-audio-driver

### Herramientas de gráficos

- **SuperFamiconv** — conversor de gráficos de tiles por línea de comandos. https://github.com/Optiroc/SuperFamiconv
- **tiledpalettequant** — utilidad web en JavaScript para reducir imágenes a combinaciones de tiles y paletas. https://rilden.github.io/tiledpalettequant/
- **M1TE2** — editor de tilemap para modo 1, permite configurar y previsualizar tres capas juntas. https://github.com/nesdoug/M1TE2
- **M8TE** — editor de tilemap para 8bpp (modos 3 y 7). https://github.com/nesdoug/M8TE
- **SPEZ** — editor de metasprites. https://github.com/nesdoug/SPEZ

### Herramientas web

- **SnesInstructionCycleTool** — calcula ciclos de CPU bajo distintas condiciones. https://novasquirrel.github.io/SnesInstructionCycleTool/
- **Mode7Preview** — permite especificar y visualizar la fórmula de los registros del Modo 7. https://novasquirrel.github.io/Mode7Preview/
- **Telinc1 Mode 7 Simulator** — ajuste y visualización de parámetros del Modo 7. https://telinc1.github.io/mode7/
- **65816 Chiplab** — ejecuta ensamblador contra un 65816 real desde el navegador y muestra el estado de los buses en cada ciclo. https://chiplab.emulationonline.com/65816/
- **SnesVRAMPlanner** — constructor visual de distribución de la VRAM. https://novasquirrel.github.io/SnesVRAMPlanner/

### Gestión de ROMs

- **uCON64** — herramienta gestora/copiadora de ROMs. https://ucon64.sourceforge.io

### Emuladores

- **bsnes** — https://github.com/bsnes-emu/bsnes/releases
- **Mesen** — https://www.mesen.ca/
- **Snes9x** — https://github.com/snes9xgit/snes9x/releases
- **ZSNES** — https://www.zsnes.com/

La wiki aclara que la mayoría de estos emuladores no tienen capacidades de debugging (de ahí la sección separada de "Debugging" más arriba).

## Notas / gotchas

- Mesen aparece listado tanto en la sección de debugging como en la de emuladores: es la opción recomendada cuando se necesita depurar durante el desarrollo.
- Calypsi y vbcc requieren licencia comercial, a diferencia del resto de compiladores/ensambladores listados que son gratuitos u open source.
- Si se necesita debugging avanzado, priorizar ca65 (dentro de cc65) por su soporte completo en Mesen frente a otros ensambladores.

## Referencias cruzadas

- [Timing del sistema SNES](timing.md)
- (pendiente de enlazar)

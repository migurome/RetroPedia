---
titulo: SMPS — driver de sonido de Sega para Mega Drive
plataforma: [MegaDrive]
categoria: [sonido]
tags: [smps, driver-sonido, sega, ym2612, psg, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/sonicretro/smps-rips]
---

# SMPS — driver de sonido de Sega para Mega Drive

## Resumen

SMPS ("Sega Music Programming System") es el driver de sonido propio de Sega usado en la gran mayoría de sus juegos de Mega Drive (incluida toda la saga clásica de Sonic), corriendo en el Z80 y controlando tanto el YM2612 (FM) como el PSG. Es distinto de — y mucho más extendido que — los formatos de instrumento EIF/EEF/TFI/VGI ya documentados en esta base (ver [`ym2612-operations.md`](ym2612-operations.md), fuente plutiedev.com), que pertenecen a herramientas de tracker más modernas (Echo, TFM Maker, VGM Maker) no al driver original de Sega.

## Detalle técnico

### Variantes del driver

- **68k** — implementación de SMPS para Mega Drive corriendo sobre el 68000 (variante menos común; la mayoría corre en el Z80).
- **Z80** — implementación basada en Z80, la más habitual en juegos de Mega Drive.
- **preSMPS** — versiones anteriores del driver (tanto en 68k como en Z80), usadas en los primeros juegos antes de estandarizarse el formato "SMPS" propiamente dicho.
- **Pico** — variante para Sega Pico (hardware educativo infantil basado en Mega Drive).

### Extensiones de archivo y sub-formatos

Los "rips" (extracciones) de SMPS desde ROMs reales usan extensiones que identifican la variante exacta del driver:
- `.smy` — SMPS Z80 Type 1
- `.mmw` — SMPS 68k Type 1
- `.tra` / `.trs` — SMPS 68k Type 2, con distintos algoritmos de tempo

### Contenido típico de un rip

Cada carpeta de juego en el repositorio `smps-rips` contiene canciones, muestras PCM y datos de instrumento extraídos como binario crudo directamente de la ROM (algunas incluyen versiones prototipo o contenido extra, como versiones corregidas de canciones o efectos de sonido). Los archivos de configuración asociados documentan parámetros del driver, datos de instrumento FM/PSG, especificaciones de muestras PCM, y flags de coordinación que definen comandos de secuencia.

## Notas / gotchas

- Esta entrada resume lo que se pudo extraer del README de `smps-rips`; no cubre la especificación completa del formato binario de SMPS (comandos de secuencia, tabla de instrumentos byte a byte) — para eso, sería necesario estudiar directamente un disassembly como [s1disasm](https://github.com/sonicretro/s1disasm) (ver [`../herramientas/sonic-retro-disassemblies.md`](../herramientas/sonic-retro-disassemblies.md)), que incluye el driver SMPS completo comentado.
- No confundir con los formatos EIF/EEF/TFI/VGI ya documentados (fuente plutiedev.com) — son de herramientas de terceros para trackear música FM/PSG "a mano" en homebrew moderno, no el driver que usaba Sega internamente.

## Referencias cruzadas

- [Disassemblies de Sonic Retro](../herramientas/sonic-retro-disassemblies.md)
- [`ym2612-operations.md`](ym2612-operations.md), [`psg.md`](psg.md), [`format-eif.md`](format-eif.md), [`format-eef.md`](format-eef.md), [`format-tfi.md`](format-tfi.md) (fuente plutiedev.com)

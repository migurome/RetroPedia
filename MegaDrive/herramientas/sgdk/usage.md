---
titulo: Uso de SGDK — estructura de proyecto y build
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, makefile, rescomp, build, proyecto]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/SGDK-Usage]
---

# Uso de SGDK — estructura de proyecto y build

## Resumen

Cómo debe organizarse un proyecto SGDK en disco, qué tipos de archivo reconoce el sistema de build, y los comandos para compilarlo en sus distintos modos (release/debug/asm).

## Detalle técnico

### Estructura de carpetas del proyecto

- `src/` — código fuente (soporta hasta 2 niveles de subcarpetas, p. ej. `src/engine/enemy/`)
- `inc/` — cabeceras (headers)
- `res/` — archivos de definición de recursos
- `out/` — salida compilada (se crea automáticamente)

### Tipos de archivo soportados

**Código fuente:**
- `.c` — código C
- `.s` — ensamblador 68000
- `.s80` — ensamblador Z80

**Cabeceras:**
- `.h` — cabeceras C
- `.inc` — cabeceras de ensamblador

**Recursos:**
- `.res` — definiciones de recursos (compiladas por `rescomp`)
- `.bmp`, `.png` — imágenes (solo color indexado)
- `.vgm`, `.xgm` — archivos de música
- `.wav` — efectos de sonido
- `.bin` — datos binarios

### Modos de compilación

El makefile soporta tres modos:

1. **release** — build optimizado (por defecto)
2. **debug** — incluye símbolos de depuración y logging de Gens KMod
3. **asm** — genera listados de ensamblador

### Comandos de build

Desde la raíz del proyecto:

```
<SGDK_PATH>\bin\make -f <SGDK_PATH>\makefile.gen
```

Para build de debug:

```
<SGDK_PATH>\bin\make -f <SGDK_PATH>\makefile.gen debug
```

(sustituyendo `<SGDK_PATH>` por la ruta completa de instalación, incluida la letra de unidad, p. ej. `D:\SGDK`).

### Salida

Al compilar con éxito aparece un archivo `rom.bin` en la carpeta `out/`, listo para emular o grabar en un flash cart.

## Notas / gotchas

- El sistema de recursos (`.res` + `rescomp`) es central en SGDK — ver `overview.md` y `rescomp.txt` (en el propio repo) para la sintaxis completa de declaración de recursos.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Instalación de SGDK](installation.md)
- [Tutorial: Introducción](tuto-introduction.md)

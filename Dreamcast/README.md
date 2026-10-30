---
titulo: Dreamcast — overview
plataforma: [Dreamcast]
categoria: [general]
tags: [hardware, overview, kallistios, dreamsdk, sh4, powervr, aica]
estado: borrador
fecha: 2026-10-30
fuentes: [cabeceras de KallistiOS (master del 27-10-2026), proyecto hyperdbz-dreamcast-port (Fase 0), dreamcast.wiki (Development), mc.pp.se/dc (Marcus Comstedt), repos KallistiOS/kos-ports/GLdc, awesome-dreamcast]
---

# Dreamcast

## Resumen de hardware

Lo marcado con ✔ se comprobó en un proyecto real (`hyperdbz-dreamcast-port`, KallistiOS con DreamSDK R4, probado en Flycast 2.7). Lo marcado con ⏳ está pendiente de medir en la consola.

- **CPU:** Hitachi SH-4 a 200 MHz, con FPU de precisión simple rápida. Little endian. **No admite accesos desalineados** de 16/32 bits: las estructuras binarias que se leen desde disco deben tener sus campos alineados. ✔
- **RAM principal:** 16 MB. Con KallistiOS recién iniciado quedan unos **15,4 MB** libres para el programa. ✔
- **Vídeo:** PowerVR2 (CLX2), renderizado diferido por tiles, 8 MB de VRAM. Con `pvr_init` a 640×480 quedan unos **5,4 MB** de VRAM libres para texturas. ✔
  - Texturas con lados potencia de dos, de 8 a 1024. Formatos ARGB1555, RGB565, ARGB4444, paletizadas de 4/8 bits (1024 colores de paleta compartidos) y VQ.
  - Listas de polígonos: opacos (OP), transparentes (TR, ordenados por profundidad) y *punch-through* (PT, alfa de 1 bit, ideal para sprites). ✔
- **Sonido:** Yamaha AICA con un ARM7 y **2 MB de RAM propia**; tras `snd_init` quedan unos **1,8 MB** libres. 64 canales PCM/ADPCM. ✔
- **Disco:** GD-ROM de ~1 GB. En homebrew se usan CD-R (700 MB, imagen CDI) o emuladores de lectora como **GDEMU** (imágenes GDI o CDI desde una SD). ⏳ velocidad real
- **Salida:** 640×480 por VGA (progresivo) o por cable compuesto/RGB (entrelazado).

## Kit de desarrollo

- **KallistiOS (KOS)**: SDK libre y la base de casi todo el homebrew. En Windows se instala cómodamente con **DreamSDK** (ver [DreamSDK en Windows](herramientas/dreamsdk-windows.md)).
- No usar los SDK oficiales (Katana de Sega, Windows CE): son propietarios y no se pueden distribuir.

## Dos tipos de entradas

- **`estado: revisado`**: salen del port de Hyper DBZ, comprobadas en un proyecto real.
- **`estado: borrador`** (2026-10-30): extraídas de la documentación de la comunidad: [dreamcast.wiki](https://www.dreamcast.wiki/Development), la [referencia de hardware de Marcus Comstedt](https://mc.pp.se/dc/), los repositorios de KallistiOS, kos-ports y GLdc, y [awesome-dreamcast](https://github.com/dreamcastdevs/awesome-dreamcast). Los nombres de la API de KOS se contrastaron con las cabeceras de KOS instaladas por DreamSDK R4.

## Categorías

**herramientas/**
- [DreamSDK R4 en Windows — instalación y problemas resueltos](herramientas/dreamsdk-windows.md)
- [Imagen de disco autoarrancable (CDI) — 1ST_READ.BIN, IP.BIN, mkisofs y cdi4dc](herramientas/imagen-cdi.md)
- [Flycast automatizado — buzón en RAM con Lua y capturas](herramientas/flycast-automatizado.md)
- [Toolchain de KallistiOS en Linux, macOS y WSL — dc-chain, environ.sh, Makefile y romdisk](herramientas/toolchain-kos.md)
- [Carga en consola y depuración — dcload-ip/serial, dc-tool, GDB y emuladores](herramientas/carga-y-depuracion.md)
- [Bibliotecas, motores y recursos — kos-ports, GLdc, Simulant, SH4ZAM y comunidad](herramientas/bibliotecas-motores-y-recursos.md)

**cpu/**
- [Optimización del SH-4 — caché, store queues, alineación, ABI, FIPR/FTRV y GCC](cpu/sh4-optimizacion.md)

**memoria/**
- [Mapa de memoria — regiones P0-P4, áreas físicas, VRAM de 32/64 bits y SDRAM](memoria/mapa-de-memoria.md)

**graficos/**
- [Caché de sprites en VRAM — texturas PVR, subida y asignador buddy](graficos/cache-sprites-vram.md)
- [PowerVR2 — render por tiles, listas OP/TR/PT, bucle de escena y fillrate](graficos/powervr-pipeline.md)
- [Texturas del PVR — formatos, twiddling, VQ, paletas de 4/8 bpp y modo sprite](graficos/texturas-paletas-y-sprites.md)

**sonido/**
- [ADPCM del AICA — formato, codificador y límites](sonido/aica-adpcm.md)

**perifericos/**
- [Bus Maple, mando y VMU — protocolo, lectura del mando con KOS y partidas guardadas](perifericos/maple-mando-y-vmu.md)

**avanzado/**
- [Arranque — estructura de IP.BIN, carga de 1ST_READ.BIN y syscalls de la BIOS](avanzado/arranque-ip-bin-y-syscalls.md)

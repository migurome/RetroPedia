---
titulo: Big Evil Corporation — puesta en marcha del toolchain (serie Tanglewood)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [bigevilcorporation, tanglewood, asm68k, mess, toolchain, ensamblador]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2012/02/28/sega-megadrive-1-getting-started/]
---

# Big Evil Corporation — puesta en marcha del toolchain (serie Tanglewood)

## Resumen

Primera entrada de la serie de desarrollo de Mega Drive del blog de Matt (Big Evil Corporation, autor de Tanglewood, juego comercial real). Cubre la elección de herramientas para arrancar un proyecto de ASM puro sin hardware de desarrollo oficial: ensamblador, emulador con debugger, y editor de texto, más los requisitos mínimos que debe cumplir una ROM para arrancar.

## Detalle técnico

### Herramientas elegidas y por qué

El autor no tenía acceso a hardware de desarrollo oficial de la época, así que montó el toolchain con herramientas de la escena homebrew/romhacking:

- **Ensamblador: SN Systems ASM68K v2.53.** Herramienta de línea de comandos para MS-DOS, históricamente usada por estudios comerciales de Mega Drive. El autor lo obtuvo del paquete de disassemblies de Sonic the Hedgehog de Sonic Retro, que incluye una versión modificada compatible con Windows 7.
- **Emulador con debugger: MESS (Multi Emulator Super System).** Elegido explícitamente porque sus atajos de teclado de debugging coinciden con los de Visual Studio, reduciendo la fricción de cambiar de contexto. Se mencionan como alternativas Gens con el plugin KMod y Regen (este último recomendado por la comunidad de ASSEMblergames.com).
- **Editor: Microsoft Visual Studio.** Elegido por familiaridad con sus atajos, no porque existiera integración oficial con Mega Drive — en la época no había un IDE dedicado.
- **Editor hexadecimal**, necesario porque ASM68K sin la opción adecuada añade metadatos de debugger SN al binario resultante que hay que eliminar manualmente (o evitar con el flag `/p`, ver abajo).

### Compilación con ASM68K

```
ASM68K.EXE source.asm,destination.bin
```

Con la opción `/p` (recomendada) para eliminar los metadatos del debugger SN original del binario generado:

```
ASM68K.EXE /p source.asm,destination.bin
```

Ejemplo de ensamblado exitoso citado en el artículo:

```
Loop:
   move.l #0xF, d0
   move.l d0, d1
   jmp Loop
```

Salida del ensamblador: `SN 68k version 2.53 / Assembly completed / 0 error(s) from 4 lines in 0.1 seconds`.

### Ejecución y debugging en MESS

Tras la primera ejecución, MESS genera un `mess.ini` que hay que editar para activar el flag de debug (ponerlo a 1).

```
mess64.exe genesis -cart test.bin
mess64.exe genesis -cart test.bin -debug
```

Atajos de debugging (coinciden deliberadamente con Visual Studio):
- `F9` — poner/quitar breakpoint
- `F10` — step over
- `F11` — step into
- `SHIFT+F11` — step out

### Requisitos mínimos de la ROM

**Hardware de referencia:** CPU Motorola 68000 a 7.61 MHz, coprocesador Zilog Z80, 64KB de RAM de trabajo, 64KB de VRAM, chips de audio Yamaha YM2612 (FM) y Texas Instruments SN76489 (PSG).

**Estructura:**
- Cabecera de 512 bytes.
- Stack pointer inicial: `0x00FFE000`.
- Rango de RAM: `0x00FF0000` a `0x00FFFFFF`.
- Labels/rutinas obligatorias que debe exponer la ROM: `EntryPoint` (punto de entrada principal), `Exception` (manejador de excepciones — debe terminar en `rte`, no en `rts`), `HBlankInterrupt`, `VBlankInterrupt`, y un marcador de fin `__end`.

**Convención de formato del código fuente:** el código y los datos requieren tabulación a la derecha; las etiquetas (labels) no.

### Recursos citados por el autor

- Disassemblies de Sonic the Hedgehog de Sonic Retro (fuente del ensamblador y punto de partida habitual).
- Tutoriales de Marc's Realm sobre Mega Drive.
- Foro ASSEMblergames.com (discusión sobre emuladores adecuados para debugging).

## Notas / gotchas

- El autor deja claro que este toolchain (ASM68K + Visual Studio + MESS) fue una elección pragmática de 2012 condicionada por no tener hardware de desarrollo oficial ni un IDE dedicado a Mega Drive; no lo presenta como "la" forma correcta, sino como la que le funcionó a él para arrancar Tanglewood.
- Sin la opción `/p` de ASM68K, el binario de salida incluye metadatos del debugger SN que hay que limpiar a mano con un editor hexadecimal antes de poder cargarlo en el emulador.
- La instrucción de retorno desde el manejador de excepciones es `rte`, no `rts` — un error fácil de cometer si se copia el patrón de una subrutina normal.

## Referencias cruzadas

- [Build y debug de ASM puro para Mega Drive (sin SGDK)](build-debug-asm-puro.md) — cubre el mismo problema (toolchain de ASM puro) con herramientas más modernas (vasm, BlastEm); esta entrada documenta específicamente la elección histórica de 2012 de Big Evil Corporation.
- [Cabecera de la ROM](../memoria/rom-header.md)
- [Disassemblies de Sonic Retro](sonic-retro-disassemblies.md)

---
titulo: Build y debug de ASM puro para Mega Drive (sin SGDK)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [asm, vasm, asm68k, wla-dx, blastem, gens-kmod, debug, build, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://bumbershootsoft.wordpress.com/platform-guide-sega-genesis-mega-drive/, https://github.com/sroccaserra/learning-megadrive, https://github.com/libretro/blastem, https://documentation.help/Gens-KMod/]
---

# Build y debug de ASM puro para Mega Drive (sin SGDK)

## Resumen

Ya tenemos documentado el flujo de build de [SGDK](sgdk/usage.md) (C + `make` + `rescomp`), pero faltaba el equivalente para quien escribe ensamblador 68000 directamente (como en las entradas de plutiedev.com o los disassemblies de Sonic Retro): qué ensamblador usar, cómo se pasa de `.asm` a una ROM válida, y cómo depurarla.

## Detalle técnico

### Ensambladores disponibles

| Ensamblador | Notas |
|---|---|
| **vasm** (`vasmm68k_mot`) | El más recomendado en guías modernas; sintaxis Motorola. Se compila desde el tarball con `make CPU=m68k SYNTAX=mot`. |
| **ASMX** | Descrito como "primitivo en algunos aspectos" pero funcional; soporta mezclar código m68k y Z80 en el mismo archivo; hay una continuación activa en GitHub. |
| **ASMotor** | Ensamblador multi-target con salidas específicas para Genesis; soporta mezclar Motorola y Zilog (Z80) en el mismo proyecto; incluye linker propio. |
| **WLA-DX** | Incluye ensambladores Motorola y Z80 con directivas específicas de Genesis; sintaxis con particularidades que no gustan a todo el mundo. |
| **ASM68K** | Herramienta histórica de SN Systems (originalmente para MS-DOS) — es el ensamblador que usan tradicionalmente muchos disassemblies clásicos (incluido el linaje de s1disasm, ver [`sonic-retro-disassemblies.md`](sonic-retro-disassemblies.md)). |
| **AS** (Macroassembler AS, de Alfred Arnold) | Otra opción histórica usada en algunas ramas de disassemblies (p. ej. la rama "AS" de s1disasm). |

Para proyectos basados en **vasm**, el linker recomendado es **vlink** (portable, soporta múltiples formatos de salida).

### Ejemplo de build mínimo (vasm)

Ensamblar directamente a binario plano:
```
vasmm68k_mot -Fbin -o hello.bin hello.asm
```
El binario resultante ya corre directamente en un emulador para un programa simple, sin pasos de linkado adicionales.

En proyectos más grandes con múltiples fuentes/assets, es habitual envolver esto en un `Makefile` (p. ej. el proyecto `learning-megadrive` reduce todo el proceso a un simple `make`, incluyendo la conversión de PNGs indexados a `.gfx`/`.pal` mediante scripts de Python antes de inyectarlos en la ROM).

### Requisitos estructurales de la ROM

- Tabla de vectores en `$000000` (incluye el vector de reset apuntando al código de inicialización).
- Cabecera oficial de Sega — copia exacta de los campos esperados (ver [`../memoria/rom-header.md`](../memoria/rom-header.md), fuente plutiedev.com, para el formato completo).
- Checksum del cartucho, declarado como `dc.w` en la cabecera (ver también [`../memoria/rom-header.md`](../memoria/rom-header.md)).
- Expansión mínima a `$003fff` (16KB) — coincide con lo ya documentado en [`../memoria/rom-padding.md`](../memoria/rom-padding.md) (fuente plutiedev.com).

### Herramientas de depuración

| Herramienta | Punto fuerte |
|---|---|
| **BlastEm** | Depurador de línea de comandos integrado (estilo GDB), más soporte para actuar como **stub remoto de GDB real** (`-D` + `target remote :1234`). Ver comandos abajo. |
| **Gens KMod** | Los mejores visualizadores de gráficos y sonido (paletas, tiles, sprites, registros del VDP/YM2612/PSG); soporta logging de mensajes vía `KLog_xx(..)` (usado también desde SGDK, ver [`../herramientas/sgdk/overview.md`](sgdk/overview.md)). |
| **DGen** | Depuradores interactivos considerados los más usables, con soporte para el coprocesador Z80. |
| **Exodus** | Emulador centrado en precisión a nivel de bus ("accuracy at any cost") — el más adecuado para cazar bugs de borde (edge cases) de timing real. |

### Comandos del depurador de BlastEm

Invocación:
- `-d` — arranca BlastEm directamente en modo depurador.
- Tecla asignada a `ui.enter_debugger` (por defecto `u`) — entra al depurador durante la ejecución.
- `-dm` — depura específicamente la ROM de menú.

Depurador 68K:
- `b DIRECCION` — pone un breakpoint; `d BREAKPOINT` lo borra.
- `c` — continúa; `n` avanza una instrucción; `s` sigue las ramas hacia subrutinas.
- `p[/(x|X|d|c)] VALOR` — imprime un registro o posición de memoria; `di[/(x|X|d|c)] VALOR` lo imprime cada vez que se alcanza un breakpoint.
- `a DIRECCION` — avanza hasta una dirección concreta; `bt` imprime el backtrace.
- `vs` — imprime la lista de sprites del VDP; `vr` — imprime la información de registros del VDP.

Depurador Z80 (funcionalidad similar):
- `b DIRECCION` / `de BREAKPOINT` — breakpoints.
- `p[/(x|X|d|c)] VALOR` — inspección de valores.
- `n` / `c` — control de ejecución.
- `q` — sale del depurador desde cualquiera de las dos CPUs.

Depuración remota vía GDB real: en Unix, conectar BlastEm por pipe con `-D`; en Windows, ejecutar `blastem.exe ROM.bin -D` y conectar con `target remote :1234` desde GDB.

## Notas / gotchas

- El README de BlastEm no documenta nada sobre ensamblado/build de ROMs — es puramente un emulador/depurador, no un toolchain de compilación (a diferencia de SGDK, que integra todo).
- Elegir ensamblador según el proyecto: si se va a estudiar/modificar un disassembly existente de Sonic Retro, usar el mismo ensamblador con el que se mantiene ese repo (histórico: ASM68K o AS); para un proyecto nuevo desde cero, vasm es la recomendación más moderna y con mejor soporte activo.
- Contenido reconstruido combinando varias fuentes independientes (blog de Bumbershoot Software, repo de ejemplo `learning-megadrive`, README de BlastEm) — contrastar con la documentación oficial de cada herramienta antes de fijar el toolchain de un proyecto real.

## Referencias cruzadas

- [Motorola 68000 — instrucciones y modos de direccionamiento](../cpu/68000-instrucciones-modos-direccionamiento.md)
- [`../memoria/rom-header.md`](../memoria/rom-header.md), [`../memoria/rom-padding.md`](../memoria/rom-padding.md) (fuente plutiedev.com)
- [SGDK — uso (build en C, para comparar)](sgdk/usage.md)
- [Disassemblies de Sonic Retro](sonic-retro-disassemblies.md)
- [Hilos de SpritesMind sobre internals del VDP](../graficos/spritesmind-vdp-hilos.md)

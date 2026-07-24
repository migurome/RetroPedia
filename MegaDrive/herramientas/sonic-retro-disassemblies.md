---
titulo: Disassemblies de Sonic Retro (código fuente real de juegos comerciales)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sonicretro, disassembly, sonic, github, codigo-real, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://info.sonicretro.org/Disassemblies, https://github.com/orgs/sonicretro/repositories]
---

# Disassemblies de Sonic Retro (código fuente real de juegos comerciales)

## Resumen

Sonic Retro mantiene un conjunto de **disassemblies** (desensamblados anotados) de juegos comerciales de Mega Drive/Genesis, principalmente de la saga Sonic, junto con herramientas asociadas (editores de niveles, reproductores de música, librerías de compresión). Un disassembly es el código ensamblador original reconstruido a partir de la ROM, con etiquetas y comentarios añadidos por la comunidad — es la forma más directa de ver cómo se estructuró realmente un juego comercial completo, más allá de los tutoriales de plutiedev.com o SGDK.

**Aviso legal:** estos disassemblies se presentan explícitamente "con fines informativos y educativos únicamente" por sus propios mantenedores; el uso comercial está prohibido y Sonic Retro no reclama propiedad del código original de Sega.

## Detalle técnico

### Limitación de acceso

La página índice de la wiki (`https://info.sonicretro.org/Disassemblies`) devuelve **403 Forbidden** con las herramientas de fetch disponibles (igual que ocurrió con el foro de SpritesMind — ver [`spritesmind-overview.md`](spritesmind-overview.md)); tampoco es accesible `segaretro.org` (wiki hermana con documentación de hardware/formatos Sega). El listado de abajo se reconstruyó consultando directamente la organización de GitHub `sonicretro`, que sí es accesible.

### Disassemblies de juegos (Assembly)

| Repositorio | Juego |
|---|---|
| [s1disasm](https://github.com/sonicretro/s1disasm) | Sonic the Hedgehog (1) |
| [s2disasm](https://github.com/sonicretro/s2disasm) | Sonic the Hedgehog 2 |
| [skdisasm](https://github.com/sonicretro/skdisasm) | Sonic & Knuckles |
| [skc_disasm](https://github.com/sonicretro/skc_disasm) | Sonic & Knuckles Collection (PC) |
| [scddisasm](https://github.com/sonicretro/scddisasm) | Sonic CD |
| [spindisasm](https://github.com/sonicretro/spindisasm) | Sonic Spinball |
| [kid-chameleon-disasm](https://github.com/sonicretro/kid-chameleon-disasm) | Kid Chameleon (split disassembly) |
| [chaotix](https://github.com/sonicretro/chaotix) | Knuckles' Chaotix (32X) — disassembly + toolkit |
| [ristar](https://github.com/sonicretro/ristar) | Ristar |

Los README de estos repos son deliberadamente breves y remiten a la wiki (bloqueada) para la explicación de la estructura de carpetas — solo confirman que la organización típica incluye carpetas como `_anim` (animación), `_inc` (includes), `mappings`, `palettes`, `sound`, `levels` y variantes de arte comprimido (`artkos`, `artnem`, `artunc` — Kosinski, Nemesis, sin comprimir respectivamente, ver [`compresion-nemesis-kosinski-enigma.md`](compresion-nemesis-kosinski-enigma.md)).

### Editores de nivel y herramientas relacionadas

| Repositorio | Qué es |
|---|---|
| [SonLVL](https://github.com/sonicretro/SonLVL) | Editor de niveles genérico para juegos de Sonic (C#) |
| [SonLVL-SPA](https://github.com/sonicretro/SonLVL-SPA) | Editor de niveles para Sonic Pocket Adventure |
| [ManiaBSEdit](https://github.com/sonicretro/ManiaBSEdit) | Editor de la fase de Blue Spheres de Sonic Mania |
| [Generations-Archive-Editor](https://github.com/sonicretro/Generations-Archive-Editor) | Editor de archivos `.ar.00`/`.pfd` de Sonic Generations |

### Audio y compresión

| Repositorio | Qué es |
|---|---|
| [smps-rips](https://github.com/sonicretro/smps-rips) | Datos crudos del driver de sonido SMPS extraídos de juegos de MD — ver [`../sonido/smps-sound-driver.md`](../sonido/smps-sound-driver.md) |
| [SMPSPlay-DLL](https://github.com/sonicretro/SMPSPlay-DLL) | Librería para reproducir música SMPS de juegos de Sonic en Mega Drive |
| [KENSSharp](https://github.com/sonicretro/KENSSharp) | Librería de compresión/descompresión (Nemesis, Kosinski, Enigma, Saxman...) — ver [`compresion-nemesis-kosinski-enigma.md`](compresion-nemesis-kosinski-enigma.md) |

### Mod loaders y utilidades

`mod-loader-common`, `sonicr-mod-loader`, `mania-mod-loader`, `skc-mod-loader`, `SCDSteamModLoader` — infraestructura para cargar mods en juegos de Sonic ya publicados; poco relevante para desarrollo homebrew desde cero, se mencionan solo por completitud.

## Notas / gotchas

- Estos son proyectos con miles de líneas de ASM 68000 real de un juego comercial completo — no son tutoriales paso a paso. Son más útiles como referencia ("¿cómo resolvió Sega/el equipo original este problema concreto?") que como punto de partida para aprender desde cero (para eso, mejor las entradas de plutiedev.com o SGDK ya presentes en esta base).
- Al no poder leer la wiki, no se ha documentado aquí el detalle exacto de la estructura de carpetas ni las convenciones de macros de cada disassembly — para eso, clonar el repo y leer directamente `README.md`/`_inc/` o buscar una copia de la wiki por otra vía.

## Referencias cruzadas

- [SpritesMind — overview y limitaciones de acceso](spritesmind-overview.md)
- [SMPS — driver de sonido](../sonido/smps-sound-driver.md)
- [Formatos de compresión Nemesis/Kosinski/Enigma](compresion-nemesis-kosinski-enigma.md)
- [Formatos de compresión SLZ/UFTC](format-slz.md) (fuente plutiedev.com — formatos alternativos, menos extendidos que los de Sega)

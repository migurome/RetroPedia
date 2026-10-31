# RetroPedia

Base de conocimiento sobre desarrollo de videoconsolas retro (Mega Drive, SNES, Neo Geo, Dreamcast, y las que se vayan añadiendo) y arquitectura de motores. Pensada para que una IA (o cualquier persona) pueda consultarla como referencia técnica al trabajar en proyectos concretos.

## Estructura

```
RetroPedia/
├── CLAUDE.md             # Instrucciones para la IA que mantiene la base
├── INDEX.md              # Mapa general, enlaces a todo el contenido
├── GLOSARIO.md            # Conceptos transversales a varias plataformas
├── GLOSARIO/              # Fundamentos (fixed-point, colisiones...) y reglas de proceso (reglas comunes, depuración)
├── _plantilla.md          # Plantilla para nuevas entradas
├── Notas.txt              # Fuentes pendientes de extraer
├── MegaDrive/
│   ├── README.md          # Overview de la plataforma
│   ├── graficos/, sonido/, cpu/, memoria/, perifericos/, avanzado/
│   └── herramientas/      # incluye herramientas/sgdk/
├── SNES/
│   ├── README.md
│   └── graficos/, sonido/, cpu/, memoria/, perifericos/, herramientas/
├── NeoGeo/
│   ├── README.md          # Overview (qué está comprobado y qué no)
│   └── graficos/, herramientas/   # ngdevkit en Windows, sprites SCB, ROM C/S, caso práctico
├── Dreamcast/
│   ├── README.md          # Overview (qué está comprobado y qué no)
│   └── graficos/, sonido/, cpu/, memoria/, perifericos/, avanzado/, herramientas/
├── MOTORES/
│   ├── README.md          # Arquitectura de motores, multiplataforma (no ligado a una consola)
│   └── arquitectura/, efectos/, graficos-hardware/, mugen/
├── DISENO/                # Diseño de niveles y de mundo (multiplataforma)
└── EXTRACCION/            # Sacar fondos, sprites y animaciones de un vídeo (sin ROM ni código)
```

Los subdirectorios de categoría (`graficos/`, `sonido/`, etc.) se crean bajo demanda, según vaya haciendo falta contenido — no hace falta tenerlos todos vacíos de antemano.

## Por qué Markdown + carpetas

- Es el formato que mejor puedo leer, buscar (grep/glob) y razonar directamente, sin parsers intermedios.
- Legible y editable a mano, versionable con git si en algún momento se quiere.
- Cada consola vive en su propia carpeta; los conceptos que cruzan plataformas (DMA, tile-based rendering, interrupciones...) van en `GLOSARIO.md` para no duplicar explicaciones.

## Convenciones para nuevas entradas

1. Copia `_plantilla.md` dentro de la subcarpeta de categoría que corresponda.
2. Rellena el frontmatter (ver plantilla) — sirve para filtrar por plataforma/categoría/estado más adelante.
3. Añade un enlace a la entrada en `INDEX.md`.
4. Si el concepto aplica a más de una plataforma, va en `GLOSARIO.md` en vez de duplicarse.

## Estado

273 entradas en total (2026-10-31), más de 200 de ellas ya extraídas para Mega Drive (plutiedev.com, blogs de Big Evil Corporation/TANGLEWOOD y Bumbershoot Software, SGDK, SpritesMind/Sega Retro/Sonic Retro como índices curados), SNES (SNESdev Wiki), y una sección `MOTORES/` de arquitectura de motores multiplataforma (fabiensanglard.net) pensada explícitamente como referencia de diseño para una futura v2 de motor-cenital. Las entradas están en `estado: borrador` hasta contrastarse en un proyecto real (14 ya están en `revisado`) — ver [INDEX.md](INDEX.md) para el mapa completo y la nota de origen de cada tanda de contenido. Próximas fuentes pendientes de extraer (ver `Notas.txt`): más SNES (nesdev.org, pvsneslib, foros), y más Mega Drive (retroreversing.com). La plataforma NeoGeo (ngdevkit) ya tiene sección propia (2026-10-23), con 5 entradas sacadas de un proyecto real probado en GnGeo: instalación en Windows, sprites SCB, formatos de ROM y un caso práctico. La plataforma Dreamcast (KallistiOS con DreamSDK) tiene sección propia desde 2026-10-30, con entradas del port de Hyper DBZ (MUGEN) probado en Flycast y una segunda tanda extraída de dreamcast.wiki, mc.pp.se y los repositorios de KOS (toolchain, SH-4, PowerVR, texturas, Maple/VMU, arranque), y `MOTORES/mugen/` recoge los formatos y la semántica de MUGEN.

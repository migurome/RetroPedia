# RetroPedia

Base de conocimiento sobre desarrollo de videoconsolas retro (Mega Drive, SNES, y las que se vayan añadiendo). Pensada para que una IA (o cualquier persona) pueda consultarla como referencia técnica al trabajar en proyectos concretos.

## Estructura

```
RetroPedia/
├── INDEX.md              # Mapa general, enlaces a todo el contenido
├── GLOSARIO.md            # Conceptos transversales a varias plataformas
├── GLOSARIO/              # Fundamentos de programación (fixed-point, colisiones...)
├── _plantilla.md          # Plantilla para nuevas entradas
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
├── MOTORES/
│   ├── README.md          # Arquitectura de motores, multiplataforma (no ligado a una consola)
│   └── arquitectura/, efectos/, graficos-hardware/
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

Más de 200 entradas técnicas ya extraídas para Mega Drive (plutiedev.com, blogs de Big Evil Corporation/TANGLEWOOD y Bumbershoot Software, SGDK, SpritesMind/Sega Retro/Sonic Retro como índices curados), SNES (SNESdev Wiki), y una sección `MOTORES/` de arquitectura de motores multiplataforma (fabiensanglard.net) pensada explícitamente como referencia de diseño para una futura v2 de motor-cenital. Todas las entradas están marcadas `estado: borrador` en su frontmatter hasta ser revisadas/contrastadas trabajando en un proyecto real — ver [INDEX.md](INDEX.md) para el mapa completo y la nota de origen de cada tanda de contenido. Próximas fuentes pendientes de extraer (ver `Notas.txt`): más SNES (nesdev.org, pvsneslib, foros), y más Mega Drive (retroreversing.com). La plataforma NeoGeo (ngdevkit) ya tiene sección propia (2026-10-23), con 5 entradas sacadas de un proyecto real probado en GnGeo: instalación en Windows, sprites SCB, formatos de ROM y un caso práctico.

# RetroPedia

Base de conocimiento sobre desarrollo de videoconsolas retro (Mega Drive, SNES, y las que se vayan añadiendo). Pensada para que una IA (o cualquier persona) pueda consultarla como referencia técnica al trabajar en proyectos concretos.

## Estructura

```
RetroPedia/
├── INDEX.md            # Mapa general, enlaces a todo el contenido
├── GLOSARIO.md          # Conceptos transversales a varias plataformas
├── _plantilla.md        # Plantilla para nuevas entradas
├── MegaDrive/
│   ├── README.md        # Overview de la plataforma
│   ├── graficos/
│   ├── sonido/
│   ├── cpu/
│   ├── memoria/
│   └── herramientas/
└── SNES/
    ├── README.md
    ├── graficos/
    ├── sonido/
    ├── cpu/
    ├── memoria/
    └── herramientas/
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

Estructura recién creada — sin contenido técnico todavía. Se irá alimentando a medida que abramos proyectos concretos (Mega Drive, SNES, ...).

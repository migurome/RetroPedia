# RetroPedia

Base de conocimiento en Markdown sobre desarrollo para consolas retro (Mega Drive, SNES, NeoGeo, Dreamcast) y arquitectura de motores. No hay código ni build: el producto son las entradas `.md`.

Otros proyectos la usan como referencia: `hyperdbz DreamCast Port` (→ `Dreamcast/`, `MOTORES/mugen/`), `SSB Port` y los demakes de `Future` (→ `MegaDrive/`, `NeoGeo/`).

**Todo proyecto nuevo** que se apoye en RetroPedia aplica desde el principio
[GLOSARIO/reglas-comunes-de-proyecto.md](GLOSARIO/reglas-comunes-de-proyecto.md) (skill de usuario `nuevo-proyecto-retro`):
RetroPedia como consulta y destino de los avances reales, herramientas compartidas en
`C:\Users\migur\Desktop\Tools`, `ROADMAP.md` con fases, `DIARIO.md` de builds, builds
`Nombre.[Fase].[NúmeroBuild].ext` con el identificador visible en el juego, el savestate como log y QR en pantalla
para el hardware real.

## Estructura

- `INDEX.md`: mapa de todo el contenido. **Toda entrada nueva se enlaza aquí.**
- `GLOSARIO.md` + `GLOSARIO/`: conceptos transversales. Si algo aplica a varias plataformas, va aquí y no se duplica.
- `<Plataforma>/README.md`: visión general y qué está comprobado y qué no. Las entradas van en subcarpetas por categoría (`graficos/`, `sonido/`, `cpu/`, `memoria/`, `perifericos/`, `avanzado/`, `herramientas/`), que se crean bajo demanda.
- `MOTORES/` (arquitectura, efectos, mugen), `DISENO/` (diseño de niveles) y `EXTRACCION/` (sacar assets de un vídeo).
- `_plantilla.md`: plantilla obligatoria para cualquier entrada nueva.
- `Notas.txt`: fuentes pendientes de extraer.

## Convenciones

- Cada entrada nueva parte de `_plantilla.md`. El frontmatter (`titulo`, `plataforma`, `categoria`, `tags`, `estado`, `fecha`, `fuentes`) es obligatorio.
- `estado: borrador` hasta que se contraste en un proyecto real; entonces pasa a `revisado`. Lo comprobado en hardware o emulador se dice explícitamente.
- Cita siempre las fuentes (URL o proyecto) en `fuentes:`.
- Al añadir entradas, actualiza el recuento de la tabla del `README.md` de la plataforma.
- Commits en castellano, con el formato `Área: qué cambia` (p. ej. `Dreamcast: escenarios 2D en el PVR (...)`).

## Ahorro de tokens

- `INDEX.md` ocupa 34 KB: no lo leas entero. Busca la sección con Grep (`^## `, `^### `) y lee solo ese tramo con offset/limit.
- Para localizar entradas, usa Grep sobre el frontmatter (`plataforma:`, `tags:`) o Glob por carpeta. No hace falta abrir ficheros uno a uno.

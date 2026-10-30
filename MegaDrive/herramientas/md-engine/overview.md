---
titulo: MD Engine — overview (motor visual para Mega Drive)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [md-engine, tbce, sgdk, electron, motor-visual, eventos, steam]
estado: borrador
fecha: 2026-10-15
fuentes: [análisis directo de MD Engine 1.0.9 instalado vía Steam (app.asar, plantillas, código C del motor)]
---

# MD Engine — overview

## Resumen

MD Engine (Two Black Cats, en Steam) es un editor visual de juegos de Mega Drive. Funciona al estilo de GB Studio: escenas, actores y scripts de eventos que se compilan a C y luego a una ROM. Por dentro usa un motor propio en C llamado **TBCE** sobre una toolchain de SGDK (gcc m68k, rescomp, xgmtool), y trae un emulador para probar. No tiene línea de comandos: compilar se hace siempre desde la GUI, pero el formato de proyecto es JSON legible y se puede **generar por código**.

## Detalle técnico

**Instalación (Steam, Windows):** `C:\Program Files (x86)\Steam\steamapps\common\MD Engine\MD Engine-win32-x64\`

- `md-engine.exe`: app Electron (versión de Electron 22.3.27 en el archivo `version`).
- `resources/app.asar` (~339 MB) contiene:
  - `/appData/mde/templates/`: 38 proyectos plantilla (action-platformer, top-down, shump, breakout, endless-runner, collisions, camera-follow, parallax, squarebrothers, visual-novel, etc.).
  - `/src/lib/events/*.js`: ~356 definiciones de eventos (campos, valores por defecto, `id`).
  - `/appData/engine/` y `/appData/mde/engine/`: código C del motor TBCE (`core/TBCE_Actor.*`, `modules/TBCE_Physics.c`, `TBCE_Collisions_*.c`, `TBCE_Camera.c`…) y `-schema/engine.json` (campos de física editables).
  - `/buildTools/win32-x64/mde/bin/`: gcc, as, ld, rescomp.jar, xgmtool, xgm2tool, sjasm, make.
  - `/subApps/`: herramientas auxiliares (imgconv, bmfont, minipainter, minitracker, bfxr, chipsfx).
- La lógica de compilación (JS → C) está en bytecode V8 (`.webpack/main/*.jsc`), así que no se puede leer directamente. Para ver qué genera, se mira el C de la última compilación (ver [depuracion-y-generacion.md](depuracion-y-generacion.md)).

**Conceptos del motor:**

| Concepto | Qué es |
|---|---|
| Proyecto | Carpeta con `<nombre>.mdep`, `project/` (recursos `.mderes`) y `assets/` (PNG, TMX, audio) |
| Escena | Una pantalla o nivel: fondo, mapa de colisiones, actores, triggers y scripts `onStart`/`onUpdate`/`onEnd` |
| Actor | Objeto con sprite, grupo de colisión y scripts `onStart`, `onUpdate`, `onEnd` y `hit1..hit8` |
| Prefab | Plantilla de actor reutilizable (`project/prefabs/actors/`) |
| Script personalizado | Subrutina con parámetros de actor y variable (`project/scripts/`, `EVENT_CALL_CUSTOM_EVENT`) |
| Variables | Globales numeradas, locales por actor `L0..` y temporales `T0..T2` |
| Campos de motor | Física global: velocidad máxima, aceleración, deceleración, gravedad, salto (`engine_field_values.mderes`) |

**Hardware/ajustes por defecto relevantes** (`settings.mderes`): modo de escena H40 (320×224) o H32 (256×224); `vdpBGFG: 1004` tiles para fondos, `vdpSprites: 420`, `vdpFont: 96`; driver de música XGM2; cartucho de 4 MB con guardado `flash_only`.

## Notas / gotchas

- Si MD Engine tiene abierto un proyecto y se modifican sus archivos desde fuera, la app puede sobrescribirlos al guardar. Ciérrala, o cierra el proyecto sin guardar, antes de regenerar.
- No hay CLI de compilación: cualquier flujo automatizado acaba en "abrir en la GUI → Build".
- El nombre del archivo de evento no coincide siempre con su `id` (`eventTextDraw.js` → `EVENT_DRAW_TEXT`). Ver [eventos-y-scripts.md](eventos-y-scripts.md).

## Referencias cruzadas

- [formato-proyecto.md](formato-proyecto.md): estructura JSON de cada recurso.
- [eventos-y-scripts.md](eventos-y-scripts.md): eventos, valores y variables.
- [fisica-y-colisiones.md](fisica-y-colisiones.md): física, grupos de colisión y gotchas.
- [camara-y-scroll.md](camara-y-scroll.md): cámara, niveles largos y parallax.
- [depuracion-y-generacion.md](depuracion-y-generacion.md): generar proyectos por código y depurar.
- [../sgdk/overview.md](../sgdk/overview.md): SGDK, la base de la toolchain.
- [../../graficos/imitar-paletas-otras-consolas.md](../../graficos/imitar-paletas-otras-consolas.md)

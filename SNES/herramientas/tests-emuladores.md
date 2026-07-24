---
titulo: ROMs de test para verificar precisión de emuladores de SNES
plataforma: [SNES]
categoria: [herramientas]
tags: [test-roms, emuladores, precision, cpu, ppu, mando]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Emulator_tests]
---

# ROMs de test para verificar precisión de emuladores de SNES

## Resumen

Listado de la SNESdev Wiki con ROMs de test y suites de pruebas usadas para validar cuán fiel es un emulador de SNES al hardware real. Cubre suites generales, tests de CPU, tests de PPU (gráficos) y tests de entrada (mando/ratón).

## Detalle técnico

### Suites generales de test

1. **undisbeliever SNES Test ROMS** — colección amplia de tests para desarrollo de emuladores. https://github.com/undisbeliever/snes-test-roms
2. **blargg hardware tests** — suite de pruebas de precisión de hardware. https://snescentral.com/article.php?id=1115
3. **TASVideos SNES Accuracy Tests** — recurso de validación de precisión. https://tasvideos.org/Emulatorresources/SNESaccuracytests

### Tests de CPU

**Multiplier Behavior Test** — discusión en el foro de nesdev: https://forums.nesdev.org/viewtopic.php?t=24087. Comprueba que escribir en $4203 dos veces demasiado rápido produce un resultado erróneo (comportamiento oscuro del multiplicador). Marcado como trabajo en curso (work in progress).

### Tests de PPU (gráficos)

| ROM | Autor | Propósito |
|-----|-------|-----------|
| gradient-test | NovaSquirrel | Prueba el registro CGWSEL; pone en evidencia comportamientos inexactos en emuladores |
| Two Ship | rainwarrior | Compara Modo 5 + entrelazado frente a gráficos en Modo 1 |
| PPU bus activity | lidnariq | Muestra todos los modos 0-6 simultáneamente en una sola pantalla |
| Elasticity | rainwarrior | Técnicas de Modo 3 para lograr más profundidad de color de la que ofrece 8bpp |

### Tests de entrada

| ROM | Autor | Propósito |
|-----|-------|-----------|
| ctrltest | rainwarrior | Test genérico de lectura de mando |
| mset | rainwarrior | Test del periférico de ratón |

## Notas / gotchas

- El test del multiplicador ($4203 escrito dos veces demasiado rápido) apunta a un comportamiento oscuro del hardware que coincide con la colisión de multiplicación/división documentada como quirk del S-CPU — ver [Erratas y quirks conocidos del hardware SNES](errata.md).
- gradient-test (CGWSEL) es útil específicamente para detectar emuladores que implementan mal el color math/windowing, un área con varios bugs de emulación documentados por juego.

## Referencias cruzadas

- [Erratas y quirks conocidos del hardware SNES](errata.md)
- [Juegos difíciles de emular](juegos-dificiles-de-emular.md)
- (pendiente de enlazar)

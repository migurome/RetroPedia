---
titulo: Receta — de un personaje de MUGEN al disco de Dreamcast
plataforma: [General, Dreamcast]
categoria: [motores]
tags: [mugen, port, sprites, formatos, audio, kos, toolchain]
estado: revisado
comprobado: emulador
comprobado_en: "parcial: Flycast 2.7, hyperdbz-dreamcast-port; Goku y Krillin en partida completa, otros personajes en pruebas sueltas; la cadena se documenta sin haberla lanzado de nuevo"
fecha: 2026-11-06
fuentes: [proyecto hyperdbz-dreamcast-port (tools/build_bank.py, build_logic.py, build_stage.py, build_hud.py, make_disc.sh, CLAUDE.md, ROADMAP.md)]
---

# De un personaje de MUGEN al disco de Dreamcast

## Resumen

Qué herramienta convierte cada fichero de MUGEN y cómo se acaba en un CDI, según la cadena de `hyperdbz-dreamcast-port`.
Las herramientas están en ese proyecto, no aquí: esta receta dice en qué orden se usan. No hay ruta equivalente para
cartuchos: Mega Drive y Neo Geo no se han intentado.

## Detalle técnico

| De | A | Herramienta | Qué hace |
|---|---|---|---|
| `.sff`, `.air`, `.act` | `<nombre>.hdb` | `tools/build_bank.py` | Recorta y comprime sprites; paletas ARGB1555; animaciones |
| `.snd` | `<nombre>.hds` | `tools/build_bank.py` | Sonidos en ADPCM de Yamaha mono |
| `.cns`, `.cmd`, `common1.cns` | `<nombre>.hdl` | `tools/build_logic.py` | CNS a bytecode para la VM de `dc/proto/src/logic.c` |
| Escenario `.def` y `.sff` | `<escenario>.hde` | `tools/build_stage.py` | Sprites a 4 bpp, 8 bpp o ARGB1555, en baldosas |
| `fight.def`, `fight.sff`, `.fnt` | `hud.hdb` | `tools/build_hud.py` | Barra de vida; fuentes convertidas en sprites |
| `fightfx` | `fightfx.hdb` | `tools/build_bank.py --fightfx` | Chispas de golpe |
| `build/cd/` y el ejecutable | `build/hdbz.<fase>.<build>.cdi` | `tools/make_disc.sh` | Compila y monta el disco; sube la build |

### Orden

```sh
# Desde la raíz del proyecto, en el bash de Git. Python siempre con -I.
py -I tools/build_bank.py  "<carpeta del personaje>" build/cd
py -I tools/build_logic.py "<carpeta del personaje>" "<carpeta del juego>" build/cd
sh tools/qa/run.sh          # antes de gastar una build
sh tools/make_disc.sh       # recompila también los .hdl, el escenario, fightfx y el HUD
sh tools/run_flycast.sh     # lanza la última imagen y recoge el log y la VMU
```

`make_disc.sh` fija el escenario (Kame House). Que un personaje convertido salga a combatir depende del prototipo: revisa
`dc/proto/src/main.c`.

### Qué mirar con un personaje nuevo

1. [Sprites obligatorios](sprites-obligatorios-y-grupos.md): si faltan, los lanzamientos se ven mal.
2. `mugenversion` y `localcoord`: [migrar de 2002 a 1.0](migrar-contenido-2002-a-1.0.md).
3. `py -I tools/inventario.py`: lo que la VM aún no entiende.
4. `py -I tools/rarezas.py`: rarezas en ejecución de los emparejamientos.
5. Verlo jugar: en Hyper DBZ, probar otros personajes destapó fallos que Goku y Krillin no enseñaban.

## Notas / gotchas

- **Derechos:** la [licencia de MUGEN](README.md) y la de los personajes son aparte. El juego original no se versiona.
- **Formatos ligados a la VM:** un `.hdl` antiguo no vale; por eso `make_disc.sh` los recompila siempre.
- **Ruta con espacios:** el proyecto compila a través del junction `C:\DreamSDK\home\hdbz`.
- **Lo escrito aquí puede quedarse viejo:** manda el `CLAUDE.md` del proyecto, que sigue en desarrollo.

## Referencias cruzadas

- [Formatos SFF, AIR y SND](formatos-sff-air-snd.md)
- [Compilar el CNS](cns-compilacion.md)
- [Imagen de disco CDI](../../Dreamcast/herramientas/imagen-cdi.md)
- [Decisiones de proyectos](../../GLOSARIO/decisiones-de-proyectos.md)
- [Dolmexica Infinite](dolmexica-infinite.md): la alternativa sin motor propio

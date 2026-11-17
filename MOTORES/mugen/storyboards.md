---
titulo: Storyboards de MUGEN — logo, intros, finales, créditos y game over
plataforma: [General]
categoria: [motores]
tags: [mugen, formatos, animacion, audio, referencia]
estado: revisado
comprobado: emulador
comprobado_en: "parcial: Flycast 2.7, hyperdbz-dreamcast-port 5.108 (logo, intro, 20 finales y créditos de Hyper DBZ; sin window ni volumescale/pan)"
fecha: 2026-11-06
fuentes: [https://network.mugenguild.com/justnopoint/1.0docs/storyboard.html, proyecto hyperdbz-dreamcast-port (tools/build_story.py, dc/proto/src/stage.c stage_load_mem/stage_stream, main.c story_play; paso 5.4, build 5.108)]
---

# Storyboards de MUGEN

## Resumen

Un storyboard es un `.def` que define una cinemática: escenas con capas de animación o texto, sonido, música y fundidos.
Un port que quiera logo, intro o finales debe interpretar este formato o convertirlo. El port de Hyper DBZ a Dreamcast convierte
cada escena en un escenario (sus capas pasan a capas `anim` y su `starttime`/`endtime` a BGCtrl) y lo reproduce con streaming de
baldosas: ver [Llevarlo a una consola](#llevarlo-a-una-consola-hyper-dbz-en-dreamcast).

## Detalle técnico

### Cuándo se reproducen

| Evento | Se declara en | Cuándo | Ejemplo (KFM) |
|---|---|---|---|
| Logo | `system.def` | Al iniciar, una vez | `data/kfm/logo.def` |
| Intro | `system.def` | Tras el logo | `data/kfm/intro.def` |
| Final por defecto | `system.def` | Al ganar con un personaje sin final propio | Sin archivo |
| Créditos | `system.def` | Tras el final | `data/kfm/credits.def` |
| Game over | `system.def` | Al elegir «No» en continuar | `data/kfm/gameover.def` |
| Intro de personaje | `.def` del personaje | Tras elegirlo en Arcade | `chars/kfm/intro.def` |
| Final de personaje | `.def` del personaje | Tras ganar Arcade; sustituye al final por defecto | `chars/kfm/ending.def` |

Se puede probar uno suelto con `mugen -storyboard <archivo>`, que busca desde `data/`.

### Secciones

| Sección | Parámetro | Significado | Por defecto |
|---|---|---|---|
| `[Info]` | `localcoord = w, h` | Lienzo; se escala si no coincide con la ventana | 320, 240 |
| `[SceneDef]` | `spr` | `.sff`, en la carpeta del storyboard. **Obligatorio** | — |
| | `snd` | `.snd` | — |
| | `fontX` | Fuente, X de 0 a 9; busca en la carpeta y luego en `font/` | — |
| | `startscene` | Se salta las primeras n escenas, para probar | 0 |
| `[Scene X]` | `end.time` | Duración en ticks (60 = 1 s). **Obligatorio** | — |
| | `fadein.time`, `fadeout.time` | Duración del fundido; no alarga `end.time` | 0 |
| | `fadein.col`, `fadeout.col` | Solo negro o blanco | 0, 0, 0 |
| | `clearcolor = r, g, b` | Color de borrado; `r = -1` no borra | Primera: -1; luego, hereda |
| | `layerall.pos = x, y` | Base de las capas; no afecta a los fondos | Primera: 0, 0; luego, hereda |
| | `window = x1, y1, x2, y2` | Área visible; no afecta a los fondos | Primera: pantalla; luego, hereda |
| | `bgm`, `bgm.loop` | Música; sin valor sigue la anterior, y vacío la para | Sin bucle |
| | `bg.name = prefijo` | Activa fondos `<prefijo>Def`, debajo de las capas salvo `layer = 1` | — |
| `[Begin Action N]` | | Animación a la que apunta `layerX.anim`; en el mismo fichero | — |

### Capas y sonidos

| Parámetro | Significado | Por defecto |
|---|---|---|
| `layerX.anim = n` | Muestra la acción n; sin ella y sin texto, nada | — |
| `layerX.text = "..."` | Texto con `\n`; no se combina con `anim` en la misma capa | — |
| `layerX.font = fuente, banco, alineación, r, g, b` | Alineación: 1 izquierda, 0 centro, -1 derecha. Banco, solo en bitmap | — |
| `layerX.textdelay = d` | Ticks entre letras | 0: todo de golpe |
| `layerX.offset = dx, dy` | Se suma a `layerall.pos` | 0, 0 |
| `layerX.starttime`, `layerX.endtime` | Cuándo se ve | 0 y fin de la escena |
| `soundX.value = grupo, sonido` | X de 0 a 99 | — |
| `soundX.starttime` | Cuándo suena | 0 |
| `soundX.volumescale` | 100 = 100 %; solo baja | 100 |
| `soundX.pan` | De -127 a 127 | 0 |

### Llevarlo a una consola (Hyper DBZ en Dreamcast)

- **Una escena = un escenario.** El fondo (`bg.name`) ya lo es; cada `layerX` se añade como capa `type = anim` con un id propio, y su
  tiempo como BGCtrl: `Visible = 0` antes de `starttime` y desde `endtime`, y `Anim` en `starttime` para que la acción empiece desde
  su primer frame (como Ikemen). Así el reproductor es el mismo código que dibuja los escenarios.
- **Coordenadas distintas:** las capas cuentan desde la esquina de arriba a la izquierda del lienzo; los fondos, como en un
  escenario, desde el centro (x = 0 en el medio con la cámara en 0). Al pasar una capa a capa de escenario hay que restar
  `localcoord.x / 2`.
- **Cada storyboard trae su `localcoord`:** en Hyper DBZ la intro va a 640×480 y el logo, los finales y los créditos a 320×240. La
  escala de dibujo sale de él, no de la del combate.
- **Tamaño:** la intro de Hyper DBZ (`press_start.sff`, 13 MB) pide hasta 51 MB de VRAM por escena sin pérdida y 15 MB en VQ, pero
  comprimida ocupa 3,5 MB: cabe en la RAM y se sube a la VRAM solo lo que se ve en cada frame. Los finales caben sin pérdida.
- **Fuentes TrueType** (`Type = truetype` en el `.def` de la fuente, MUGEN 1.1): en la consola se pasan a glifos de mapa de bits en
  el PC. Con un solo color en todos los textos, va horneado en la paleta.
- **Fondos con nombres que empiezan igual:** con `bg.name = BG`, `[BG7ctrl 1]` es de otro fondo (`BG7`). El prefijo tiene que ir
  seguido de espacio, `Def`, `CtrlDef` o `Ctrl`.

## Notas / gotchas

- **Número de capas:** la documentación dice de 0 a 9 en un sitio y de 0 a 99 en otro. La capa de número mayor va encima.
- **Herencia:** `clearcolor`, `layerall.pos`, `window` y `bgm` pasan de una escena a la siguiente: un reproductor debe
  guardarlos entre escenas.
- **`bgm.loop` es sin bucle si no se dice.** En Hyper DBZ ningún storyboard pide bucle: todas las músicas suenan una vez.
- **Saltar:** un botón que ya esté pulsado al empezar el storyboard lo salta (si no, el que salta el logo deja ver la intro).
  Sirve también para que las pruebas automáticas lleguen al título manteniendo START.
- La página no detalla los elementos de fondo: ver [escenarios](escenarios-def.md).

## Referencias cruzadas

- [Escenarios](escenarios-def.md)
- [Localización](localizacion.md): las escenas se pueden traducir
- [Barra de vida y fuentes](barra-de-vida-y-asaltos.md)

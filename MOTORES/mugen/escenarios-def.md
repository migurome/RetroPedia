---
titulo: Escenarios de MUGEN — el .def, elementos de fondo y controladores
plataforma: [General]
categoria: [motores]
tags: [mugen, scroll, tiles, formatos, efectos]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [https://network.mugenguild.com/justnopoint/1.0docs/bgs.html, https://network.mugenguild.com/justnopoint/1.0docs/upgrading.html]
---

# Escenarios de MUGEN

## Resumen

Un escenario es un `.def` con ajustes generales, uno o más elementos de fondo y, si hace falta, controladores. El mismo
formato de fondos se usa en las pantallas de sistema y en los storyboards, así que un solo lector sirve para todo. Esta
entrada resume la documentación oficial; para dibujarlos en Dreamcast, ver
[escenarios 2D en el PVR](../../Dreamcast/graficos/escenarios-2d-paletas-y-baldosas.md).

## Detalle técnico

### Ajustes generales

| Grupo | Parámetros |
|---|---|
| `[Info]` | `name`, `displayname`, `versiondate`, `mugenversion`, `author` |
| `[Camera]` | `startx`, `starty`, `boundleft`, `boundright`, `boundhigh`, `boundlow`, `verticalfollow` (0 a 1), `floortension`, `tension` (0 a 160) |
| `[Camera]`, desde 1.0 | `overdrawhigh`, `overdrawlow`, `cuthigh`, `cutlow`. Valores de ejemplo al actualizar: 0, 0, 35 y 25 |
| `[PlayerInfo]` | `p1startx`, `p1starty`, `p2startx`, `p2starty`, `p1facing`, `p2facing`, `leftbound`, `rightbound` |
| `[Bound]` | `screenleft`, `screenright`: distancia mínima a los bordes de la pantalla |
| `[StageInfo]` | `zoffset` (altura del suelo desde arriba, en píxeles), `zoffsetlink`, `autoturn`, `resetBG`, `localcoord`, `xscale`, `yscale` |
| `[Shadow]` | `intensity` (0 a 256; por defecto 128), `color`, `yscale` (por defecto 0.4), `fade.range` |
| `[Reflection]` | `reflect` (0 o 1; por defecto 0) |
| `[Music]` | `bgmusic`, `bgmvolume`, `bgmloopstart`, `bgmloopend` |

En `[Shadow]`, cuanto mayor es un componente de `color`, menos hay de ese color en la sombra.

### Elementos de fondo

Tras `[BGDef]` (`spr = fichero.sff`, `debugbg`), todo lo que sigue en el fichero pertenece a esa sección. Los elementos se
dibujan en el orden en que aparecen, con los posteriores encima.

```ini
[BG suelo]
type = normal      ; normal | anim | parallax | dummy
spriteno = 0,0     ; anim usa actionno y su [Begin Action n] en el mismo .def
layerno = 0        ; 0 detrás de los personajes, 1 delante
start = 0,0        ; respecto al centro superior de la pantalla; y hacia abajo
delta = .5,.5      ; píxeles por píxel de cámara; por defecto 1,1
trans = none       ; none | add | add1 | sub
mask = 0           ; 1 no dibuja el color 0
tile = 0,0         ; 0 sin repetir, 1 infinito, n veces
tilespacing = 0,0
window = 0,0,319,239
windowdelta = 0,0
```

Parámetros para cualquier elemento:

- `id`: lo usan los controladores; varios elementos pueden compartirlo. Por defecto, 0.
- `positionlink = 1`: `start` pasa a ser un desplazamiento respecto al elemento anterior y `delta` deja de tener efecto.
- `velocity`: velocidad inicial.
- `sin.x`, `sin.y`: amplitud, periodo en ticks y fase.

### Diferencias entre tipos

- **`anim`:** siempre tiene `mask = 1`. Si se repite, `tilespacing` es obligatorio y mide de borde izquierdo a borde
  izquierdo, no el hueco entre baldosas como en `normal`.
- **`parallax`:** un solo sprite, sin animación ni transparencia. `xscale = arriba, abajo` multiplica el `delta` horizontal
  de cada borde, con interpolación lineal entre ambos:
  ```
  delta_arriba = delta.x * xscale[0]
  delta_abajo  = delta.x * xscale[1]
  escala_y     = 1 / (yscalestart/100 + yscaledelta/100 * camara_y)
  ```
  `yscalestart` vale 100 por defecto.
- **`dummy`:** sin gráficos; solo sirve para `zoffsetlink`, por ejemplo para que los personajes se balanceen con `sin.y`.

### Controladores de fondo

```ini
[BGCtrlDef grupo]
looptime = -1          ; reinicia el grupo; -1 o nada lo desactiva
ctrlID = 1, 2          ; hasta 10; sin esta línea afecta a todos los elementos

[BGCtrl andar]
type = VelSet
time = inicio, fin, bucle
ctrlID = 3             ; hasta 10; sustituye a la lista del grupo
x = 1
```

El temporizador empieza en 0 con el asalto y sube uno por tick. Sin `fin`, el controlador solo actúa un tick: `time = 4` es
`4, 4, -1`. Ojo al rellenar los valores que faltan con los por defecto: tomarlos desde el primero y no desde su posición deja
`4, 0, -1`, y el controlador no se aplica nunca (pasó en el port de Hyper DBZ: 13 escenarios sin sus BGCtrl de un solo tick hasta
la Fase 5).

Tipos: `null`, `Visible`, `Enabled`, `VelSet`, `VelAdd`, `PosSet`, `PosAdd`, `Anim`, `SinX` y `SinY`. Con `Visible = 0` el
tiempo del elemento sigue corriendo; con `Enabled = 0` se detiene.

## Notas / gotchas

- **Los huecos no se limpian:** lo que no cubra ningún elemento deja el efecto de "sala de espejos". `debugbg = 1` los
  rellena de magenta. Un port puede ahorrarse borrar la pantalla si el escenario la cubre entera, pero conviene comprobarlo.
- **`mask = 0` importa por velocidad,** según la documentación: conviene activarlo solo si hace falta.
- **`bgs.html` documenta `bgvolume`, que 1.0 ignora.** Lo vigente es `bgmvolume` (ver
  [migrar contenido](migrar-contenido-2002-a-1.0.md)).
- **`trans = sub` y Dreamcast:** [Dolmexica](dolmexica-infinite.md) no lo ofrece en la consola por limitación técnica. No
  se ha probado aquí.
- **`mugenversion` del escenario (comprobado en el código de Ikemen GO):** con una anterior a 1.0 solo cambia que las capas
  redondean la cámara hacia abajo (`roundpos`). Sin la línea, cuenta como antigua.
- **`yscalestart`/`yscaledelta` en el PVR (port de Hyper DBZ, 3.070):** se dibujan escalando en vertical cada baldosa desde el eje; la fórmula de arriba coincide con Ikemen GO (es recíproca). El `color` de `[Shadow]` no se puede hacer exacto: Ikemen oscurece y luego resta, y el PVR no resta.

## Referencias cruzadas

- [MUGEN — mapa de la documentación](README.md)
- [Física y golpes del motor](fisica-y-golpes-del-motor.md) (cámara con `tension`)
- [Efectos visuales en el PVR](efectos-visuales-en-dreamcast.md) (sombras)
- [Escenarios 2D en el PVR](../../Dreamcast/graficos/escenarios-2d-paletas-y-baldosas.md)

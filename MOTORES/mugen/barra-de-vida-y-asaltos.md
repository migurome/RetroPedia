---
titulo: Barra de vida (fight.def), fuentes .fnt y asaltos de MUGEN 1.0
plataforma: [General]
categoria: [motores]
tags: [mugen, fight.def, lifebar, hud, fnt, fuentes, roundstate, intro, ko, asaltos]
estado: borrador
fecha: 2026-10-30
fuentes: [fight.def y fuentes de Hyper DBZ Wicked Ambition, código de Ikemen GO (lifebar.go) como referencia del recorte de las barras, proyecto hyperdbz-dreamcast-port (tools/build_hud.py, dc/proto/src/hud.c, logic.c)]
---

# Barra de vida, fuentes y asaltos de MUGEN 1.0

## Resumen

El HUD de un juego de MUGEN está en `data/<lifebar>/fight.def`. Tiene:
- secciones de disposición (`[Lifebar]`, `[Powerbar]`, `[Face]`, `[Name]`, `[Time]`, `[Combo]`, `[Round]`,
  `[WinIcon]`);
- sus propias `[Begin Action]`;
- un `fight.sff`;
- hasta 9 fuentes `.fnt`.

Se puede convertir entero a un banco de sprites normal, con los glifos de las fuentes como sprites más, y
dibujarlo con el mismo código que los personajes. Los asaltos (RoundState) son lógica del motor.

## Detalle técnico

### Formato .fnt (v1)

- **Cabecera:** `"ElecbyteFnt\0"`, versión, y en el byte 16 `u32` con offset y tamaño del PCX y offset y
  tamaño del texto.
- **PCX:** 8 bits con RLE; la paleta va al final (`0x0C` + 768 bytes). Todos los glifos están en una tira.
- **Texto:**
  - `[Def]`: `Size = ancho, alto`, `Spacing = x, y`, `Offset`, `Type = Variable | Fixed` y `Colors`;
  - `[Map]`: una línea por carácter, `carácter x_inicio ancho` (en `Fixed`, solo el carácter, y la posición
    es su índice × ancho).
- **Glifos especiales:** caracteres raros como `` ` `` o `~` suelen ser imágenes. En Hyper DBZ son «FIGHT» y
  «KO», y se usan como `fight.text = "`"`.
- **Al convertir:** cada glifo es un sprite **sin recortar** (ancho del `[Map]`, alto de la fuente), con el
  eje en la esquina. Así el avance es el ancho del sprite más `Spacing.x`.
- **Referencia a una fuente:** `font = n, banco, alineación`. Alineación 1 = desde x, 0 = centrado,
  −1 = hasta x; la y es la línea de base (se resta la altura de la fuente).

### Barras de vida y energía

- **Elementos:** `bg0`, `bg1`, `mid` (la parte roja, que baja con retraso) y `front`. Cada uno es `anim` (acción
  de `fight.def`) o `spr`. P2 lleva `facing = -1`: se refleja sobre su eje.
- **`range.x = a, b`:** la barra está **anclada en a** y crece hacia b según la fracción de vida. Si a > b
  (P1 en Hyper DBZ: −27, −125), la ventana visible es `[pos + a + 1 − ancho·fracción, pos + a + 1]`. Con
  poca vida queda un trozo junto al centro: la barra de P1 se vacía desde el retrato. Se dibuja el sprite
  entero recortado a esa ventana (ajustando la u de la textura).
- **Energía:** `front` con la fracción `power / powermax`. El contador (`counter.font`) muestra el nivel,
  `power / 1000`.
- **Transparencias:** `AS240D40` y similares necesitan mezcla con alfa. En el PVR: con un destino casi
  entero (D ≥ 192), `SRCALPHA + ONE`; si no, `SRCALPHA + INVSRCALPHA`, con el alfa en el vértice y
  `MODULATEALPHA`.

### Asaltos (RoundState)

| RoundState | Qué pasa |
|---|---|
| 0 | `start.waittime` ticks quietos; vida llena; variables a 0 por debajo de `IntPersistIndex`/`FloatPersistIndex` (la energía se conserva) |
| 1 | solo en el asalto 1, los personajes van al estado **190** (intro), y dura **mientras alguno mantenga `AssertSpecial flag = intro`**; luego «Round N» (`round.default.displaytime`) |
| 2 | «Fight»; el control llega `ctrl.time` ticks después y corre el tiempo (`framespercount`) |
| 3 | KO (alguien sin vida), doble KO o fin de tiempo (gana el que tiene más vida) |
| 4 | a los `over.waittime`, poses: **180** al ganador y **170** al que pierde por tiempo, cuando vuelven al estado 0 |

A los `over.time` ticks del KO empieza otro asalto. Con `match.wins` victorias, se acaba el combate.

Además:
- **Mandos:** no hay fuera de la RoundState 2. Muchas IA comprueban `RoundState = 2`, pero las personas también
  tienen que quedarse quietas.
- **Vida:** se queda entre 0 y `lifemax`. La barra no puede ser negativa y `Alive` depende de ello.
- **Intros:** las de Hyper DBZ son largas y dependen del rival (Goku contra Krillin tiene la suya). Un
  límite de seguridad (30 s) evita quedarse para siempre en la RoundState 1 si un personaje no deja de
  afirmar `intro`.

## Notas / gotchas

- **Claves de `[Round]`:** dentro de `[Round]` se llaman `round.default.offset`, etc. Con el prefijo de sección
  quedan como `round.round.default.offset`.
- **Retratos:** son el sprite 9000,0 de cada personaje. Hay que incluirlo en su banco aunque ninguna animación
  lo use.
- **Fuentes y caché:** cada letra es un sprite y un quad; el HUD de Hyper DBZ son unos 30 por frame. Pasan por la
  caché de sprites como los personajes: son pequeños y se usan en todos los frames, así que no se expulsan.

## Referencias cruzadas

- [Física y golpes del motor de MUGEN 1.0](fisica-y-golpes-del-motor.md)
- [Comandos del .cmd de MUGEN y movimientos del motor](comandos-y-entrada.md)
- [Formatos de MUGEN — SFF, AIR y SND](formatos-sff-air-snd.md)
- [Caché de sprites en VRAM](../../Dreamcast/graficos/cache-sprites-vram.md)

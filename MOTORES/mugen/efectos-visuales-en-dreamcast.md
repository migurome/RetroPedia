---
titulo: Efectos visuales de MUGEN en el PVR de Dreamcast
plataforma: [Dreamcast]
categoria: [motores]
tags: [mugen, powervr, paletas, efectos]
estado: borrador
comprobado: emulador
comprobado_en: "Flycast 2.7, hyperdbz-dreamcast-port"
fecha: 2026-11-06
fuentes: [código de Ikemen GO (image.go, char.go, anim.go, system.go), proyecto hyperdbz-dreamcast-port]
---

# Efectos visuales de MUGEN en el PVR de Dreamcast

## Resumen

Cómo hace el port de Hyper DBZ los efectos del CNS. La semántica sale de Ikemen GO (`image.go`, `char.go`, `anim.go`,
`system.go`), comprobada en emulador (Flycast 2.7), no en la consola.

## Detalle técnico

### Color: PalFX, AllPalFX y BGPalFX

MUGEN hace `(píxel + add) * mul / 256`. El PVR hace `textura * color del vértice + color de desplazamiento`, sin coste
de CPU:

- color del vértice = `mul * 255 / 256`
- color de desplazamiento = `add * mul / 256` (hay que activar `PVR_SPECULAR_ENABLE` en la cabecera del polígono)

Lo que el PVR no puede hacer:

| Efecto | Solución | Límite |
|---|---|---|
| `add` negativo | Oscurecer: `mul * (255 + add) / 255` | Aproximado |
| `mul` > 256 | Se queda en 256 (el color del vértice no pasa de 1) | Brilla menos |
| `invertall`, `color` | Otra textura con la paleta cambiada, en la caché de sprites | `color` es todo o nada; no se aplica al escenario |

El efecto se ve desde el tick en que se crea y `time - 1` más: el temporizador baja al **empezar** el tick siguiente.
Un helper sin `ownpal` se ve con la paleta de su padre, pero no puede lanzar `PalFX` ni recibir el `palfx.*` de un golpe; un explod de `fightfx` sin `ownpal` no recibe el efecto.

### Resta (`S`)

El PVR no resta. La mezcla `ZERO` + «1 − color del sprite» (`PVR_BLEND_INVDESTCOLOR` en el destino) da fondo × (1 − sprite):
los píxeles transparentes no cambian nada y el blanco deja negro. No es una resta exacta: oscurece en proporción al fondo.

### Orden de dibujo

Por `sprpriority` y, en un empate, **lo último que se encola va detrás**. Ikemen GO encola proyectiles, personajes y explods,
así que un explod con la misma prioridad que su personaje queda detrás. Entre los `ontop` es al revés: el más nuevo, encima.
Cada copia de `AfterImage` tiene un nivel menos que la anterior, empezando en la prioridad del personaje menos 2.

### Rotación: AngleDraw

Se calculan las cuatro esquinas alrededor de la posición del actor, con el desplazamiento del frame incluido. Con
facing -1 se invierte el ángulo, para que un disparo hacia abajo siga yendo hacia abajo. `AngleDraw` dura un tick;
`AngleSet`, `AngleAdd` y `AngleMul` solo cambian el ángulo guardado.

### Pantalla

- **EnvShake:** `y = ampl * sin(phase + freq * t)`, en grados. Sin `phase`, 90 si `freq >= 90` y 0 si no. Mueve el
  escenario y los actores, no el HUD.
- **EnvColor:** sustituye al escenario. Sin `under`, tampoco se dibujan los personajes (salvo lo `ontop`).
- **SuperPause:** todo al 50 %, menos el jugador que la lanza, durante `time + 1` ticks; `pausebg` para el escenario.

### AfterImage

`AfterImageMax` de `mugen.cfg` limita las copias por jugador y frame (16 en Hyper DBZ). MUGEN y Ikemen GO muestrean distinto. Con personajes de MUGEN, `usable = min(guardados, length - 1)`, una copia cada
`framegap`. Cada copia acumula `paladd` y `palmul` sobre `palbright` y `palcontrast`; `palpostbright` solo se suma una
vez. No se graba durante una SuperPause o una Pause, pero sí en el hitpause.

### Sombras

El sprite del actor, cabeza abajo y aplastado por el `yscale` del `[Shadow]` del escenario, negro y con `intensity` como
alfa. `fade.range` lo desvanece al subir. El port las deja siempre en el suelo y no usa el `color` del escenario.

### RemapPal en Hyper DBZ

269 usos en el CNS, pero en los 231 emparejamientos casi todos los que se ejecutan son identidades. Solo cambian algo
los helpers de Saibamen y Dr. Briefs, de 1,1 a 1,N. Ojo: la paleta 1,N puede ser una `.act` del `.def` (que sustituye a
la del SFF) o estar solo en el SFF, como en casi todo Hyper DBZ. El banco lleva una tabla de 1,1 a 1,12. Conviene medir
antes de meter todas las paletas del SFF.

## Notas / gotchas

- Nada de esto se ha probado en la consola, solo en Flycast 2.7. La resta (`S`) es la más dudosa: se ve bien en Flycast,
  pero no es una resta exacta.
- `add` negativo y `mul` > 256 son aproximaciones (ver la tabla de Color): no esperes el mismo color que en MUGEN.
- El orden de dibujo con empates de `sprpriority` es el de Ikemen GO; si un explod sale detrás de su personaje, es eso.
- Antes de meter todas las paletas del SFF en la caché, mide cuántos `RemapPal` cambian algo de verdad.

## Referencias cruzadas

- [Formatos SFF, AIR y SND](formatos-sff-air-snd.md): de dónde salen los sprites y las paletas.
- [Compilación del CNS](cns-compilacion.md): cómo se ejecutan los controladores que lanzan estos efectos.
- [Escenarios (.def)](escenarios-def.md): `[Shadow]` y el fondo que tocan `BGPalFX` y `EnvColor`.
- [Pipeline del PowerVR](../../Dreamcast/graficos/powervr-pipeline.md): cabeceras de polígono, mezclas y color de desplazamiento.
- [Caché de sprites en VRAM](../../Dreamcast/graficos/cache-sprites-vram.md): donde viven las texturas con la paleta cambiada.

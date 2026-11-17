---
titulo: Contenido de MUGEN 2002 frente a 1.0 — mugenversion, localcoord y sonido
plataforma: [General]
categoria: [motores]
tags: [mugen, port, colisiones, audio, asm]
estado: borrador
comprobado: no
fecha: 2026-11-06
fuentes: [https://network.mugenguild.com/justnopoint/1.0docs/upgrading.html, https://network.mugenguild.com/justnopoint/1.0docs/coordspace.html, https://network.mugenguild.com/justnopoint/1.0docs/history.html, https://network.mugenguild.com/justnopoint/1.0docs/loopguide.html]
---

# Contenido de MUGEN 2002 frente a 1.0

## Resumen

MUGEN 1.0 carga casi todo lo de 2002.04.14, pero algunas funciones dependen de que el contenido declare
`mugenversion = 1.0`. Sirve en dos sentidos: como receta para actualizar contenido antiguo y, en un port, como lista de
cosas que el motor debe tratar según la versión. Es la documentación de Elecbyte; **no se ha contrastado en este
repositorio con personajes de 2002**.

## Detalle técnico

### Personajes

En `[Info]` del `.def`: `mugenversion = 1.0` y `localcoord = 320, 240`. Después, en el CNS:

| Dónde | Cambio |
|---|---|
| `HitDef` | Sumar 1 al primer valor de `pausetime` (corrige un error del motor antiguo) |
| Estado 5900 propio | Añadir el `RemapPal` que tiene el 5900 de `common1.cns` |
| `PlaySnd` | `volume` se ignora; usar `volumescale` en porcentaje (`volume = 0` pasa a `volumescale = 100`) |
| `[Data]` | `volume` también se ignora; usar `volumescale` |
| `AssertSpecial` | Añadir `ignorehitpause = 1` con `nostandguard`, `nocrouchguard`, `noairguard`, `noautoturn`, `noshadow`, `nojugglecheck`, `nowalk`, `unguardable` o `invisible` |
| Eje z | Quitar el tercer valor de `snap`, `mindist`, `maxdist` y `velset`; `gethitvar(zoff)`, `vel z` y `pos z` pasan a 0 |
| `HitDefAttr` | Corregir la sintaxis: `HitDefAttr = SC, NA, SA, HA`, no `HitDefAttr = SC` |
| Tamaño de pantalla | Usar `GameWidth` y `GameHeight` en vez de 320 y 240 |
| Detección de IA y escenario | Usar `AILevel` y `StageVar` en vez de trucos antiguos |

Con `mugenversion` inferior a 1.0, el trigger `Pos Z` devuelve el `pXstartz` del escenario. El motor también da error si
un personaje usa un controlador o trigger que no corresponde a su `mugenversion`.

### Espacios de coordenadas

Todo se traduce a un espacio común, fijado por `GameWidth` y `GameHeight` en `[Config]` de `mugen.cfg`. El factor es
**ancho de destino / ancho de origen**: un personaje de 320x240 se escala por 4 en 1280x720, y por 2 en un juego de 640x480.

| Contenido | Dónde se declara | Notas |
|---|---|---|
| Personaje | `localcoord` en `[Info]` del `.def` | |
| Motif | `localcoord` en `[Info]` de `system.def` | No se usa la altura; no se adapta a otra proporción de pantalla |
| Escenario | `localcoord` en `[StageInfo]` | No se usa la altura; ver `cuthigh`, `cutlow`, `overdrawhigh` y `overdrawlow` en [escenarios](escenarios-def.md) |

### Escenarios y motifs

- **Escenario:** `mugenversion` cambia la cámara: con 2002 va en unidades enteras y con 1.0 admite fracciones. `hires = 1`
  pasa a `xscale = .5` y `yscale = .5`.
- **Motif:** `Doubleres = 4` está en desuso; hay que corregir con `scale`, que suele aplicarse a las fuentes. `fight.def`
  necesita `[FightFx]` con `scale`.
- **Sprites:** 1.0 añade [SFF v2](formatos-sff-air-snd.md), con más funciones de paleta y de compresión. Convertir es
  opcional.

### Sonido

- **Efectos:** el producto de los volúmenes no pasa de 100 %.
- **Música:** `bgmvolume` (100 = sin cambios) sustituye a `bgvolume`, que se ignora.
- **Bucle:** `bgmloopstart` (por defecto 0) y `bgmloopend` (por defecto -1, fin del fichero). La música suena hasta
  `loopend` y luego repite de `loopstart` a `loopend`. En la documentación de 1.0 solo los admite el plugin `mpg123`, con
  valores en muestras.
- **Dónde poner el bucle:** en audio con pérdida, mejor un bucle interno que uno al final del fichero, que puede oírse.
  Conviene colocarlo en un cruce por cero de una parte tranquila.
- **Pistas de CD:** en un escenario, `bgmusic = 3.da` pide la pista 3 del CD y `0.da`, una al azar.

## Notas / gotchas

- **No basta con leer el CNS:** un mismo `pausetime` dura un tick de diferencia según la versión. Un port que admita los
  dos tipos de personaje debe guardar la `mugenversion` de cada uno o normalizar el valor al compilar.
- **1.1 no está aquí:** esta documentación llega a 1.0. [Física y golpes](fisica-y-golpes-del-motor.md) ya avisa de que 1.1
  aplica algunas cosas de otra forma.
- **La documentación escribe la versión antigua de dos formas:** `2002.04.14` en las notas de actualización y `2002,02,14`
  en la de escenarios.

## Referencias cruzadas

- [MUGEN — mapa de la documentación](README.md)
- [Física y golpes del motor](fisica-y-golpes-del-motor.md)
- [Compilar la lógica CNS](cns-compilacion.md)
- [Escenarios de MUGEN](escenarios-def.md)

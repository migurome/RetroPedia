---
titulo: Formato EEF (Echo Envelope Format)
plataforma: [MegaDrive]
categoria: [sonido]
tags: [formato, eef, echo, psg, envolvente, instrumento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/format-eef]
---

# Formato EEF (Echo Envelope Format)

## Resumen

EEF ("Echo Envelope Format") es un formato de archivo para instrumentos PSG introducido por el sound driver Echo. Cada byte del archivo representa un tick (1/60 de segundo) codificando volumen y desplazamiento de tono, con puntos de loop obligatorios. La extensión de archivo sugerida es "`.eef`".

## Detalle técnico

**Tipo:** instrumento PSG

EEF ("Echo Envelope Format") es un formato para instrumentos PSG introducido por [Echo](https://github.com/sikthehedgehog/Echo). La extensión de archivo sugerida es "`.eef`".

Cada byte representa un tick (1/60 de segundo), con el siguiente formato:

- El nibble inferior es el volumen (0 = pico, 15 = mute).
- El nibble superior es un desplazamiento de tono (pitch shift, ver tabla más abajo).

Además, dos valores se usan para indicar los puntos de loop: `$FE` indica dónde empieza el loop, `$FF` indica dónde termina el loop (y el final del instrumento). **Los puntos de loop son obligatorios**; si no se quiere que un instrumento haga loop, hacer un punto de loop de un solo valor (si se quiere que se silencie, usar `$FE $0F $FF` al final).

El desplazamiento de tono es relativo al tono nominal del canal (es decir, la nota que se pretende tocar). El nibble superior puede tener uno de los siguientes valores:

| Value | Shift |
|---|---|
| `$0x` | Intact |
| `$1x` | +1 semitone |
| `$2x` | +2 semitones |
| `$3x` | +3 semitones |
| `$4x` | +4 semitones |
| `$5x` | +6 semitones |
| `$6x` | +8 semitones |
| `$7x` | +12 semitones |
| `$8x` | -1 semitone |
| `$9x` | -2 semitones |
| `$Ax` | -3 semitones |
| `$Bx` | -4 semitones |
| `$Cx` | -6 semitones |
| `$Dx` | -8 semitones |
| `$Ex` | -12 semitones |

Los valores `$F0` a `$FD` no son válidos en la versión actual.

## Notas / gotchas

- Los puntos de loop (`$FE` inicio, `$FF` fin) son obligatorios en todo instrumento EEF; no hay forma de omitirlos.
- Para un instrumento sin loop real, se usa un punto de loop de un solo valor; para que el instrumento acabe en silencio, la convención es terminar con `$FE $0F $FF`.
- El rango `$F0` a `$FD` está reservado/no es válido en la versión actual del formato.
- El desplazamiento de tono no es lineal en semitonos en la parte alta de la tabla (salta de +4 a +6, y de +8 a +12), a diferencia de la parte negativa que es simétrica en magnitud.
- Este formato es usado, entre otros casos, para los instrumentos de [silbido](psg-whistle.md) y [guitarra](psg-guitar.md) de PSG documentados en Plutiedev.

## Referencias cruzadas

- (pendiente de enlazar)

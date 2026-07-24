---
titulo: Formato EIF (Echo Instrument Format)
plataforma: [MegaDrive]
categoria: [sonido]
tags: [formato, eif, echo, fm, instrumento, ym2612]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/format-eif]
---

# Formato EIF (Echo Instrument Format)

## Resumen

EIF ("Echo Instrument Format") es un formato de archivo para instrumentos FM introducido por el sound driver Echo. Es básicamente un volcado directo de los valores de todos los registros relevantes del YM2612 para un instrumento, con la extensión de archivo sugerida "`.eif`".

## Detalle técnico

**Tipo:** instrumento FM

EIF ("Echo Instrument Format") es un formato para instrumentos FM introducido por [Echo](https://github.com/sikthehedgehog/Echo). La extensión de archivo sugerida es "`.eif`".

Un instrumento EIF tiene 29 bytes de longitud y es esencialmente los valores de cada registro del YM2612. El primer byte es el registro `$B0+` del YM2612, luego vienen los registros `$30+`, `$34+`, `$38+`… hasta `$9C+`. Eso es todo.

Los bits que no son usados por el YM2612 son *obligatoriamente* 0. Los drivers de sonido pueden depender de esto.

## Notas / gotchas

- Longitud fija: 29 bytes.
- Los bits no usados por el YM2612 deben ser 0 de forma obligatoria — algunos drivers de sonido pueden asumir esto y comportarse mal si no se respeta.
- Es esencialmente un volcado 1:1 de registros del YM2612, por lo que para interpretarlo hace falta conocer el [mapa de registros del YM2612](ym2612-registers) (orden `$B0+`, luego `$30+`, `$34+`, `$38+`… hasta `$9C+`).

## Referencias cruzadas

- (pendiente de enlazar)

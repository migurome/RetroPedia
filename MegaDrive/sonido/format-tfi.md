---
titulo: Formatos TFI y VGI (instrumentos FM de TFM Maker y VGM Maker)
plataforma: [MegaDrive]
categoria: [sonido]
tags: [formato, tfi, vgi, fm, instrumento, ym2612]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/format-tfi]
---

# Formatos TFI y VGI (instrumentos FM de TFM Maker y VGM Maker)

## Resumen

TFI y VGI son los formatos de instrumento FM de TFM Maker y VGM Maker respectivamente. Son muy similares entre sí y, aunque rara vez los usan directamente los drivers de sonido, son más fáciles de parsear para editores de instrumentos (ya que cada parámetro ocupa su propio byte). Esta entrada documenta ambos formatos, tal como hace la página fuente.

## Detalle técnico

**Tipo:** instrumento FM

TFI y VGI son los formatos de instrumento FM de TFM Maker y VGI Maker, respectivamente. Los dos son muy parecidos y, aunque rara vez los usan directamente los drivers de sonido, son más fáciles de parsear para editores de instrumentos (ya que cada parámetro está en su propio byte).

Dado que son tan parecidos, se documentan ambos aquí.

### TFI (formato de TFM Maker)

Los instrumentos de TFM Maker tienen la extensión de archivo "`.tfi`" y constan de 42 bytes. Los dos primeros bytes son:

| Size | Description | Range |
|---|---|---|
| 1 byte | Algorithm | 0 to 7 |
| 1 byte | Feedback | 0 to 7 |

Los siguientes 40 bytes son para los operadores, donde cada grupo de 10 bytes es un operador. Los operadores vienen en orden S1, S3, S2, S4. Estos bytes son los siguientes (se repiten para cada operador):

| Size | Description | Range |
|---|---|---|
| 1 byte | Multiplier | 0 to 15 |
| 1 byte | Detune | 0 to 6 |
| 1 byte | Total level | 0 to 127 |
| 1 byte | Rate scaling | 0 to 3 |
| 1 byte | Attack rate | 0 to 31 |
| 1 byte | Decay rate | 0 to 31 |
| 1 byte | Sustain rate | 0 to 31 |
| 1 byte | Release rate | 0 to 15 |
| 1 byte | Sustain level | 0 to 15 |
| 1 byte | SSG-EG | 0 to 15 |

Todos los valores usan el mismo formato que los registros correspondientes del YM2612, excepto el detune. Para obtener el detune, restar 3 (para ponerlo en el rango -3 a +3), y luego convertir eso al formato que espera el YM2612.

### VGI (formato de VGM Maker)

Los instrumentos de VGM Maker tienen la extensión de archivo "`.vgi`" y constan de 43 bytes. Es casi idéntico a los archivos TFI, excepto que un byte extra sigue al feedback, que incluye FMS y AMS (en el mismo formato que el registro `$B4+` del YM2612).

## Notas / gotchas

- TFI: 42 bytes totales (2 bytes de cabecera + 40 bytes de operadores, 10 bytes × 4 operadores).
- VGI: 43 bytes totales — un byte más que TFI, insertado justo después del feedback, con el formato de FMS/AMS igual al registro `$B4+` del YM2612.
- El orden de los operadores en el bloque de 40 bytes es S1, S3, S2, S4 — el mismo orden "no intuitivo" que usan los propios registros del YM2612, no el orden lógico S1-S2-S3-S4.
- El campo Detune de TFI/VGI no usa directamente el formato de 3 bits con signo del YM2612: se almacena en rango 0-6 y hay que restar 3 para obtener el rango real -3 a +3 antes de convertirlo al formato de registro del YM2612.
- Ambos formatos rara vez son usados directamente por drivers de sonido en producción; su valor principal es servir de intercambio para editores de instrumentos.

## Referencias cruzadas

- (pendiente de enlazar)

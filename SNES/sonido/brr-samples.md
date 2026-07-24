---
titulo: Formato de muestras BRR
plataforma: [SNES]
categoria: [sonido]
tags: [brr, s-dsp, adpcm, audio, compresion, muestras]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/BRR_samples]
---

# Formato de muestras BRR

## Resumen

BRR (Bit Rate Reduction) es el formato de compresión de audio ADPCM que usa el S-DSP de la SNES para almacenar todas sus muestras de sonido. Comprime audio de 16 bits a un promedio de 4 bits por muestra organizándolo en bloques con un filtro de predicción adaptativo, lo que permite guardar mucho más audio en la Audio-RAM (limitada a 64 KB) a cambio de cierta pérdida de calidad.

## Detalle técnico

### Estructura de un bloque

Las muestras BRR se organizan en bloques de 16 muestras, cada uno ocupando 9 bytes: 1 byte de cabecera seguido de 8 bytes de datos (cada byte contiene dos muestras de 4 bits).

**Byte de cabecera (offset 0):**

| Bits | Campo | Significado |
|---|---|---|
| 7-4 | S (shift) | Desplazamiento a la izquierda, controla la magnitud/amplitud |
| 3-2 | F (filter) | Selección del filtro de predicción (0-3) |
| 1 | L (loop) | Flag de loop |
| 0 | E (end) | Flag de fin de muestra |

**Datos de muestra (offsets 1-8):** cada byte contiene dos muestras de 4 bits (nibble alto primero), interpretadas como valores con signo de -8 a +7 antes de aplicarles el shift y el filtro.

### Filtros de predicción

Los cuatro filtros de decodificación implementan el ADPCM combinando el nibble desplazado con las dos muestras decodificadas anteriores, según esta tabla de coeficientes:

| Filtro | Nibble desplazado | Coeficiente de muestra -1 | Coeficiente de muestra -2 |
|---|---|---|---|
| 0 | 1 | 0 | 0 |
| 1 | 1 | 15/16 | 0 |
| 2 | 1 | 61/32 | 15/16 |
| 3 | 1 | 115/64 | 13/16 |

Los filtros con número más alto sacrifican detalle en frecuencias altas a cambio de reproducir mejor las frecuencias bajas, lo que ayuda a que el sonido resultante sea más natural para ciertos timbres.

### Flags de control

- **End (E):** cuando está activo, la voz se detiene al terminar ese bloque, salvo que el flag de loop también esté activo.
- **Loop (L):** cuando el flag de fin está activo junto con este, la reproducción vuelve al punto de loop en lugar de detenerse. El punto de loop se define mediante el registro SRCN/el directorio de muestras (SCRN). Los puntos de loop deben caer en límites de bloques de 16 muestras, por la propia estructura del formato.
- **Shift (S):** se aplica a cada nibble de muestra antes de pasarlo por el filtro de predicción, y controla la amplitud general del bloque.

### Reproducción

El S-DSP aplica interpolación gaussiana para suavizar la reproducción cuando el pitch no es 1:1 con la tasa de muestreo nativa. La interpolación arranca centrada en la segunda muestra del stream, por lo que se recomienda que las primeras muestras de una BRR sean silencio (cero) para evitar artefactos audibles al empezar a reproducir.

## Notas / gotchas

- Cada bloque BRR ocupa exactamente 9 bytes por cada 16 muestras (proporción de compresión fija, aprox. 4 bits/muestra más la cabecera).
- Los puntos de loop deben alinearse a límites de bloque de 16 muestras; no se puede hacer loop a mitad de un bloque.
- Si no hay flag de loop activo, el flag de end simplemente detiene la voz al terminar el bloque.
- Es recomendable dejar muestras iniciales en silencio al principio del stream BRR, por cómo arranca centrada la interpolación gaussiana del hardware.
- El directorio de muestras que indica dónde empieza (y dónde hace loop) cada sample se controla desde el registro `DIR` del S-DSP — ver la entrada de registros del S-DSP.

## Referencias cruzadas

- (pendiente de enlazar)

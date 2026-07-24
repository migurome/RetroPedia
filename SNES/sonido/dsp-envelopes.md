---
titulo: Envolventes del S-DSP (ADSR y GAIN)
plataforma: [SNES]
categoria: [sonido]
tags: [s-dsp, adsr, gain, envolvente, audio, volumen]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/DSP_envelopes]
---

# Envolventes del S-DSP (ADSR y GAIN)

## Resumen

Cada una de las 8 voces del S-DSP tiene un valor de envolvente que controla su volumen a lo largo del tiempo, y que se genera de una de dos formas: mediante el sistema automático ADSR (Attack-Decay-Sustain-Release) o mediante control directo de ganancia (GAIN), que admite un valor fijo o una rampa lineal/exponencial. Internamente la envolvente es de 11 bits, aunque el registro de solo lectura `VxENVX` únicamente expone los 7 bits más altos.

## Detalle técnico

### Registros ADSR

| Registro | Dirección | Bits | Función |
|---|---|---|---|
| ADSR (1) | `$X5` | `EDDD AAAA` | E = activar ADSR, D = decay rate, A = attack rate |
| ADSR (2) | `$X6` | `LLLR RRRR` | L = sustain level, R = sustain rate |

### Fases del envolvente ADSR

- **Attack**: incremento lineal desde 0 hasta el máximo, al ritmo del periodo `period[A*2+1]`; suma 32 por paso (o 1024 si A = `$F`, es decir, ataque instantáneo).
- **Decay**: descenso exponencial desde el máximo hasta el sustain level, al ritmo del periodo `period[D*2+16]`.
- **Sustain**: descenso exponencial desde el sustain level hasta 0, al ritmo del periodo `period[SR]`.
- **Release**: tras el key-off, decremento fijo de 8 por muestra, independientemente de la fase en la que estuviera.

La fórmula de las fases exponenciales (decay y sustain) es: `envelope -= 1`, y después `envelope -= envelope >> 8`. El valor se acota siempre entre 0 y 2047 (`$7FF`).

### Tablas de tiempos ADSR

**Attack (de 0 al máximo):**

| Valor | Tiempo |
|---|---|
| `$0` | 4100 ms |
| `$1` | 2600 ms |
| `$2` | 1500 ms |
| `$3` | 1000 ms |
| `$4` | 640 ms |
| `$5` | 380 ms |
| `$6` | 260 ms |
| `$7` | 160 ms |
| `$8` | 96 ms |
| `$9` | 64 ms |
| `$A` | 40 ms |
| `$B` | 24 ms |
| `$C` | 16 ms |
| `$D` | 10 ms |
| `$E` | 6 ms |
| `$F` | 0 ms |

**Decay (del máximo al sustain level):**

| Valor | Tiempo |
|---|---|
| 0 | 1200 ms |
| 1 | 740 ms |
| 2 | 440 ms |
| 3 | 290 ms |
| 4 | 180 ms |
| 5 | 110 ms |
| 6 | 74 ms |
| 7 | 37 ms |

**Sustain (del máximo a 0):** 31 valores con espaciado logarítmico, desde `$00` = infinito hasta `$1F` = 18 ms (por ejemplo `$01` = 38000 ms).

### Registro GAIN

| Registro | Dirección | Bits | Función |
|---|---|---|---|
| GAIN | `$X7` | `0VVV VVVV` o `1MMV VVVV` | M = modo, V = valor |

### Modos de GAIN

- **Lineal** (bit 7 = 0): suma o resta 32 por paso.
- **Exponencial** (M = `$A`, decremento): mismo cálculo de dos pasos que en decay/sustain (`envelope -= 1`, luego `envelope -= envelope >> 8`).
- **"Bent"** (M = `$E`, incremento): suma 32 mientras el valor esté por debajo de 1536 (`$600`), y suma solo 8 por encima de ese umbral.
- **Directo** (M = `$8`, `$C`, `$D`): fija valores concretos sin rampa.

### Tablas de tiempos de GAIN

Se documentan cuatro tablas completas de 32 entradas cada una:

- **Decremento lineal** (`$8x`): desde `$80` = infinito hasta `$9F` = 2 ms.
- **Decremento exponencial** (`$Ax`): desde `$A0` = infinito hasta `$BF` = 18 ms.
- **Incremento lineal** (`$Cx`): desde `$C0` = infinito hasta `$DF` = 2 ms.
- **Incremento "bent"** (`$Ex`): desde `$E0` = infinito hasta `$FF` = 3.5 ms.

### Sistema de periodos

El DSP usa una tabla común de 32 periodos que rige la velocidad de todas las operaciones de envolvente (attack, decay, sustain y las rampas de GAIN). Una operación se dispara cuando se cumple: `0 == (counter + offset[rate]) % period[rate]`.

La tabla de periodos está organizada en 32 filas por 3 columnas (`+0`, `+1`, `+2`); por ejemplo la fila 0 contiene infinito, 2048 y 1536, y las filas 1 a 31 van decreciendo hasta llegar a 1. La tabla de offsets asociada sincroniza la fase de cada rate y casi siempre alterna entre los valores 0, 1040 y 536 según la columna.

El contador interno da la vuelta (wraps) a los `$77FF` (30720) ciclos del S-SMP tras un reset.

## Notas / gotchas

- El valor de envolvente es internamente de 11 bits, pero `VxENVX` solo expone los 7 bits altos — no esperar precisión total al leer la envolvente en curso.
- Con `attack rate = $F` el ataque es prácticamente instantáneo (salto de 1024 por paso en vez de 32).
- El release siempre resta un valor fijo de 8 por muestra tras el key-off, sin importar en qué fase (attack/decay/sustain) estuviera la voz.
- GAIN y ADSR son mutuamente excluyentes por voz: el bit 7 de `VxADSR1` decide si esa voz usa ADSR (activado) o el registro GAIN (desactivado).
- El detalle de qué registros controlan esto (`VxADSR1`, `VxADSR2`, `VxGAIN`, `VxENVX`) está en la entrada de registros del S-DSP.

## Referencias cruzadas

- (pendiente de enlazar)

---
titulo: Mapa de registros del YM2612
plataforma: [MegaDrive]
categoria: [sonido]
tags: [ym2612, registros, fm, algoritmo, lfo, hardware]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/ym2612-registers]
---

# Mapa de registros del YM2612

## Resumen

Referencia de hardware para todos los registros documentados del YM2612 (el chip de síntesis FM de la Mega Drive). No incluye bits indocumentados. Cubre los dos bancos de registros, los registros globales, por canal y por operador, con el formato de bits exacto de cada uno.

## Detalle técnico

Explicación inicial (de la fuente): el YM2612 tiene dos conjuntos de registros divididos en bancos: el banco 0 se accede en `$A04000/1` y el banco 1 se accede en `$A04002/3`. Los registros vienen en tres variantes: globales, por canal y por operador. En los dos últimos casos, el banco 0 es para los tres primeros canales y el banco 1 para los tres últimos canales. Mientras tanto, los registros globales solo existen en el banco 0.

Por alguna razón, el orden de los registros es distinto al orden de los operadores (los registros vienen en orden S1, S3, S2, S4), lo cual genera mucha confusión. Esta página usa los nombres de operador tomados del datasheet del YM3438. A la hora de programar un driver de sonido, suele ser más fácil procesarlos en orden de registro (al YM2612 no le importa mientras reciba los valores correctos en los registros correctos).

También se pueden notar discrepancias con la documentación oficial. Esto es porque Sega documentó los valores como si fueran para 8MHz, mientras que el YM2612 en realidad está conectado a un reloj ligeramente más lento (específicamente, el mismo que alimenta al 68000, que es 7.67MHz en NTSC y 7.61MHz en PAL). Esta página intenta usar los valores corregidos.

### Puertos de acceso

- **Banco 0 (canales 1-3):** `$A04000/1`
- **Banco 1 (canales 4-6):** `$A04002/3`

### `$22`: low frequency oscillator (LFO)

Registro global. Formato:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `0` | `LFOEN` | `LFO:2` | `LFO:1` | `LFO:0` |

- **`LFO`:** frecuencia del LFO
- **`LFOEN`:** 1 para activar el LFO

El low frequency oscillator (LFO) se usa para permitir que FMS y AMS (ver registro `$B4+`) funcionen, para efectos simples tipo vibrato y trémolo. Cuando el LFO está activado, están disponibles las siguientes frecuencias:

| Value | LFO frequency |
|---|---|
| `000` | 3.82 Hz |
| `001` | 5.33 Hz |
| `010` | 5.77 Hz |
| `011` | 6.11 Hz |
| `100` | 6.60 Hz |
| `101` | 9.23 Hz |
| `110` | 46.11 Hz |
| `111` | 69.22 Hz |

### `$24`, `$25`: timer A frequency

Registros globales.

Registro `$24` (timer A frequency, high):

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `TMRA:9` | `TMRA:8` | `TMRA:7` | `TMRA:6` | `TMRA:5` | `TMRA:4` | `TMRA:3` | `TMRA:2` |

Registro `$25` (timer A frequency, low):

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `0` | `0` | `0` | `TMRA:1` | `TMRA:0` |

- **`TMRA`:** frecuencia del timer A

El timer A cuenta desde `TMRA` hasta `$400` (cuando "desborda"), y entonces recarga y repite. La frecuencia exacta se calcula (aproximadamente) así:

```
($400 - TMRA) × 18.77µs
```

### `$26`: timer B frequency

Registro global. Formato:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `TMRB:7` | `TMRB:6` | `TMRB:5` | `TMRB:4` | `TMRB:3` | `TMRB:2` | `TMRB:1` | `TMRB:0` |

- **`TMRB`:** frecuencia del timer B (más alto = más rápido)

El timer B cuenta desde `TMRB` hasta `$100` (cuando "desborda"), y entonces recarga y repite. La frecuencia exacta se calcula (aproximadamente) así:

```
($100 - TMRB) × 300.34µs
```

### `$27`: channel 3 mode and timer control

Registro global. Formato:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `MODE:1` | `MODE:0` | `RST:B` | `RST:A` | `ENBL:B` | `ENBL:A` | `LOAD:B` | `LOAD:A` |

- **`LOAD`:** controla si el respectivo timer corre
  - 0: el timer está congelado
  - 1: el timer está corriendo
- **`ENBL`:** determina qué ocurre cuando el respectivo timer desborda
  - 0: no hace nada
  - 1: activa el flag
- **`RST`:** modifica el flag del respectivo timer al leer desde `$A04000`
  - 0: dejarlo tal cual
  - 1: limpiar el flag
- **`MODE`:** modo del canal 3
  - `00`: normal (como los otros canales)
  - `01`: especial (cada operador tiene su propia frecuencia)
  - `10`: CSM

Guía rápida para usar los timers (si solo se quiere dejarlos correr y comprobar cuándo se disparan, que probablemente es lo único que se necesite):

| Value | Timers | ch3 mode |
|---|---|---|
| `$0F` | — | Normal |
| `$1F` | Acknowledge timer A | Normal |
| `$2F` | Acknowledge timer B | Normal |
| `$3F` | Acknowledge both timers | Normal |
| `$4F` | — | Special |
| `$5F` | Acknowledge timer A | Special |
| `$6F` | Acknowledge timer B | Special |
| `$7F` | Acknowledge both timers | Special |

### `$28`: key-on and key-off

Registro global. Formato:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `OPERS3` | `OPERS2` | `OPERS1` | `OPERS0` | `0` | `CH:2` | `CH:1` | `CH:0` |

- **`CH`:** qué canal
- **`OPERS0`:** operador S1 (0 = key-off, 1 = key-on)
- **`OPERS1`:** operador S2 (0 = key-off, 1 = key-on)
- **`OPERS2`:** operador S3 (0 = key-off, 1 = key-on)
- **`OPERS3`:** operador S4 (0 = key-off, 1 = key-on)

Los primeros tres canales son `000-010`, los últimos tres canales son `100-110`.

Cada operador se puede fijar por separado, pero normalmente se fijarán todos igual. El attack rate empieza al pasar de off a on, el release rate empieza al pasar de on a off. La envolvente no se ve afectada si se mantiene igual.

### `$2A`: DAC output

Registro global. Cuando la salida DAC está activada (ver `$2B`), el valor escrito aquí se emite tal cual en el 6º canal. Para reproducir sonido PCM hay que escribir constantemente en este registro en el momento exacto en que las muestras deben salir.

### `$2B`: DAC enable

Registro global. Formato:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `DACEN` | `0` | `0` | `0` | `0` | `0` | `0` | `0` |

- **`DACEN`:** salida del 6º canal
  - 0: salida FM
  - 1: salida DAC

### `$30+`: `MUL` (multiply) y `DT` (detune)

Registro por operador. Asignación de registros:

| Channel | Oper S1 | Oper S2 | Oper S3 | Oper S4 |
|---|---|---|---|---|
| 1st/4th | `$30` | `$38` | `$34` | `$3C` |
| 2nd/5th | `$31` | `$39` | `$35` | `$3D` |
| 3rd/6th | `$32` | `$3A` | `$36` | `$3E` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `DT:2` | `DT:1` | `DT:0` | `MUL:3` | `MUL:2` | `MUL:1` | `MUL:0` |

- **`MUL`:** multiplicador
- **`DT`:** detune

El campo `MUL` especifica por cuánto se multiplica la frecuencia base (por 1, 2, 3… hasta 15). La excepción es si el valor es 0, en cuyo caso multiplica por 0.5 (es decir, reduce la frecuencia a la mitad).

El campo `DT` especifica un detune de frecuencia, de la siguiente forma (donde *E* es un valor pequeño que depende del tono exacto):

| Value | Detune |
|---|---|
| `000` | No detune |
| `001` | +1 × E |
| `010` | +2 × E |
| `011` | +3 × E |
| `100` | No detune |
| `101` | -1 × E |
| `110` | -2 × E |
| `111` | -3 × E |

### `$40+`: `TL` (total level)

Registro por operador. Asignación de registros:

| Channel | Oper S1 | Oper S2 | Oper S3 | Oper S4 |
|---|---|---|---|---|
| 1st/4th | `$40` | `$48` | `$44` | `$4C` |
| 2nd/5th | `$41` | `$49` | `$45` | `$4D` |
| 3rd/6th | `$42` | `$4A` | `$46` | `$4E` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `TL:6` | `TL:5` | `TL:4` | `TL:3` | `TL:2` | `TL:1` | `TL:0` |

- **`TL`:** total level (cada paso es 0.75dB más silencioso)

Total level es lo que normalmente se piensa como el "volumen". Si se quiere afectar el volumen de un canal, hay que asegurarse de tocar únicamente el `TL` de los operadores "slot" (de salida) y no los que alimentan a otros operadores.

### `$50+`: `AR` (attack rate) y `RS` (rate scaling)

Registro por operador. Asignación de registros:

| Channel | Oper S1 | Oper S2 | Oper S3 | Oper S4 |
|---|---|---|---|---|
| 1st/4th | `$50` | `$58` | `$54` | `$5C` |
| 2nd/5th | `$51` | `$59` | `$55` | `$5D` |
| 3rd/6th | `$52` | `$5A` | `$56` | `$5E` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `RS:1` | `RS:0` | `0` | `AR:4` | `AR:3` | `AR:2` | `AR:1` | `AR:0` |

- **`AR`:** attack rate (más alto = más pronunciado)
- **`RS`:** rate scaling

El rate scaling hace que las envolventes sean más pronunciadas a frecuencias más altas (necesario para recrear cómo algunos instrumentos cambian de sonido al subir de tono). Un rate scaling de 0 no hace nada, valores más altos aumentan el efecto.

### `$60+`: `DR` (decay rate) y AM enable

Registro por operador. Asignación de registros:

| Channel | Oper S1 | Oper S2 | Oper S3 | Oper S4 |
|---|---|---|---|---|
| 1st/4th | `$60` | `$68` | `$64` | `$6C` |
| 2nd/5th | `$61` | `$69` | `$65` | `$6D` |
| 3rd/6th | `$62` | `$6A` | `$66` | `$6E` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `AMON` | `0` | `0` | `DR:4` | `DR:3` | `DR:2` | `DR:1` | `DR:0` |

- **`DR`:** decay rate (más alto = más pronunciado)
- **`AMON`:** 1 para dejar que AMS (ver `$B4+`) afecte a este operador

A veces también llamado "first decay rate" (`D1R`).

### `$70+`: `SR` (sustain rate)

Registro por operador. Asignación de registros:

| Channel | Oper S1 | Oper S2 | Oper S3 | Oper S4 |
|---|---|---|---|---|
| 1st/4th | `$70` | `$78` | `$74` | `$7C` |
| 2nd/5th | `$71` | `$79` | `$75` | `$7D` |
| 3rd/6th | `$72` | `$7A` | `$76` | `$7E` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `SR:4` | `SR:3` | `SR:2` | `SR:1` | `SR:0` |

- **`SR`:** sustain rate (más alto = más pronunciado)

A veces también llamado "second decay rate" (`D2R`).

### `$80+`: `RR` (release rate) y `SL` (sustain level)

Registro por operador. Asignación de registros:

| Channel | Oper S1 | Oper S2 | Oper S3 | Oper S4 |
|---|---|---|---|---|
| 1st/4th | `$80` | `$88` | `$84` | `$8C` |
| 2nd/5th | `$81` | `$89` | `$85` | `$8D` |
| 3rd/6th | `$82` | `$8A` | `$86` | `$8E` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `SL:3` | `SL:2` | `SL:1` | `SL:0` | `RR:3` | `RR:2` | `RR:1` | `RR:0` |

- **`RR`:** release rate (más alto = más pronunciado)
- **`SL`:** sustain level (más alto = a un nivel más silencioso)

Release rate tiene un bit menos que las otras rates.

Sustain level es el punto de la envolvente donde cambia del primer al segundo decay rate. 0 es el pico de la envolvente, 15 es el fondo de la envolvente.

### `$90+`: SSG-EG

**Advertencia de la fuente:** este registro a menudo está mal implementado en clones inexactos y, si hay mala suerte, produce ruido horrible. Si de verdad se quiere dar soporte a esos clones, hay que asegurarse de poner este registro a 0 en esos casos.

Registro por operador. Asignación de registros:

| Channel | Oper S1 | Oper S2 | Oper S3 | Oper S4 |
|---|---|---|---|---|
| 1st/4th | `$90` | `$98` | `$94` | `$9C` |
| 2nd/5th | `$91` | `$99` | `$95` | `$9D` |
| 3rd/6th | `$92` | `$9A` | `$96` | `$9E` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `0` | `0` | `SSGEG:EN` | `SSGEG:2` | `SSGEG:1` | `SSGEG:0` |

- **`SSGEG`:** forma de la envolvente

Cuando `SSGEG:EN` está despejado (clear), no pasa nada especial. Cuando está activado (set), la envolvente se procesa de distintas formas (p. ej. con loop) dependiendo de los valores de los bits `SSGEG:2-0`. **Hay que asegurarse de que el attack rate sea 31 o no funcionará correctamente.**

Formas de envolvente SSG-EG disponibles (`SSGEG:2-0` → descripción del comportamiento, según el texto alternativo de las imágenes de la fuente):

| `SSGEG:2-0` | Envolvente |
|---|---|
| `000` | La envolvente hace loop |
| `001` | Suena una vez (igual que si estuviera desactivado) |
| `010` | La envolvente hace loop en zig-zag |
| `011` | Suena una vez, luego pasa a volumen máximo |
| `100` | La envolvente hace loop, está invertida (upside down) |
| `101` | Suena una vez invertida, luego se mantiene a volumen máximo |
| `110` | La envolvente hace loop en zig-zag, invertida |
| `111` | Suena una vez invertida |

### `$A0+`: frecuencia

Registros por canal. Asignación de registros:

| Channel | High half | Low half |
|---|---|---|
| 1st/4th | `$A4` | `$A0` |
| 2nd/5th | `$A5` | `$A1` |
| 3rd/6th | `$A6` | `$A2` |

Cuando el canal 3 está en modo especial, el tercer canal FM (pero *no* el sexto) puede tener su frecuencia fijada por operador. En este caso, los registros del canal 3 son los siguientes:

| Operator | High half | Low half |
|---|---|---|
| S1 | `$AD` | `$A9` |
| S2 | `$AE` | `$AA` |
| S3 | `$AC` | `$A8` |
| S4 | `$A6` | `$A2` |

Formato de los registros de frecuencia (high), `$A4` en adelante:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `BLK:2` | `BLK:1` | `BLK:0` | `FREQ:10` | `FREQ:9` | `FREQ:8` |

Formato de los registros de frecuencia (low), `$A0` en adelante:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `FREQ:7` | `FREQ:6` | `FREQ:5` | `FREQ:4` | `FREQ:3` | `FREQ:2` | `FREQ:1` | `FREQ:0` |

- **`FREQ`:** frecuencia
- **`BLK`:** block (octava)

Para fijar la frecuencia, primero escribir su mitad alta y luego su mitad baja (la frecuencia no se actualiza hasta esta última escritura).

El block funciona como la octava: la frecuencia se duplica cada vez que se incrementa en 1. En cuanto a la frecuencia, estos son algunos valores aproximados (los usados por Echo), aunque están ligeramente desviados dependiendo de si es un sistema NTSC o PAL:

| Semitone | Frequency |
|---|---|
| C | 644 |
| C# | 681 |
| D | 722 |
| D# | 765 |
| E | 810 |
| F | 858 |
| F# | 910 |
| G | 964 |
| G# | 1021 |
| A | 1081 |
| A# | 1146 |
| B | 1214 |

### `$B0+`: algorithm y feedback

Registro por canal. Asignación de registros:

| Channel | Register |
|---|---|
| 1st/4th | `$B0` |
| 2nd/5th | `$B1` |
| 3rd/6th | `$B2` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `0` | `0` | `FEED:2` | `FEED:1` | `FEED:0` | `ALGO:2` | `ALGO:1` | `ALGO:0` |

- **`ALGO`:** algoritmo
- **`FEED`:** feedback del canal 1 (feedback del operador S1)

El algoritmo describe qué operadores modulan a otros operadores y cuáles generan la salida final. Distintos algoritmos pueden generar sonidos radicalmente distintos. Hay ocho disposiciones disponibles:

| Algorithm | Arrangement |
|---|---|
| 0 (`000`) | S1 a S3 a S2 a S4, salida desde S4 |
| 1 (`001`) | S1 y S3 a S2 a S4, salida desde S4 |
| 2 (`010`) | S1 a S4, S3 a S2 a S4, salida desde S4 |
| 3 (`011`) | S1 a S3 a S4, S2 a S4, salida desde S4 |
| 4 (`100`) | S1 a S2, S3 a S4, salida desde S2 y S4 |
| 5 (`101`) | S1 a S2 y S3 y S4, salida desde S2 y S3 y S4 |
| 6 (`110`) | S1 a S2, salida desde S2 y S3 y S4 |
| 7 (`111`) | sin modulación, salida desde todos |

El operador S1 puede modularse a sí mismo (además de la disposición del algoritmo); `FEED` controla cuánto lo hace. 0 es sin feedback, valores más altos incrementan la auto-modulación.

### `$B4+`: panning, PMS, AMS

Registro por canal. Asignación de registros:

| Channel | Register |
|---|---|
| 1st/4th | `$B4` |
| 2nd/5th | `$B5` |
| 3rd/6th | `$B6` |

Formato del registro:

| Bit 7 | Bit 6 | Bit 5 | Bit 4 | Bit 3 | Bit 2 | Bit 1 | Bit 0 |
|---|---|---|---|---|---|---|---|
| `L` | `R` | `AMS:1` | `AMS:0` | `0` | `PMS:2` | `PMS:1` | `PMS:0` |

- **`L`:** 1 para enviar la salida al altavoz izquierdo
- **`R`:** 1 para enviar la salida al altavoz derecho
- **`AMS`:** amplitude modulation sensivity
- **`PMS`:** frequency modulation sensivity

La sensibilidad de frecuencia y amplitud (PMS y AMS respectivamente) están pensadas para usarse junto con el LFO (`$22`) e indican cuánto afecta este a la forma de onda.

PMS se puede usar como una forma barata de vibrato. Los valores son:

| Value | Modulation |
|---|---|
| `000` | None |
| `001` | ±0.034 semitones |
| `010` | ±0.067 semitones |
| `011` | ±0.10 semitones |
| `100` | ±0.14 semitones |
| `101` | ±0.20 semitones |
| `110` | ±0.40 semitones |
| `111` | ±0.80 semitones |

AMS solo afecta a los operadores donde se ha activado el bit `AM` (ver `$60+`), y se puede usar como una forma barata de trémolo. Los valores posibles son:

| Value | Modulation |
|---|---|
| `00` | None |
| `01` | ±1.4 dB |
| `10` | ±5.9 dB |
| `11` | ±11.8 dB |

**Nota de la fuente ("Iwis says"):** `PMS` en realidad significa "*phase* modulation sensivity" (dado que el YM2612 en realidad hace modulación de fase en lugar de modulación de frecuencia). El resultado es más o menos el mismo, aparte de errores de redondeo.

## Notas / gotchas

- El orden de los registros por operador es distinto al orden lógico de los operadores: los registros vienen en orden S1, S3, S2, S4, no S1, S2, S3, S4.
- Los valores documentados por Sega asumen un reloj de 8MHz, pero el YM2612 real va a un reloj ligeramente más lento (7.67MHz en NTSC, 7.61MHz en PAL, el mismo que alimenta al 68000); esta página usa los valores corregidos.
- El registro `$90+` (SSG-EG) suele estar mal implementado en clones inexactos y puede producir ruido horrible; para dar soporte a esos clones, poner este registro a 0. Además, para que SSG-EG funcione correctamente el attack rate debe ser 31.
- Al fijar la frecuencia (`$A0+`/`$A4+`), la frecuencia no se actualiza hasta que se escribe la mitad baja (hay que escribir primero la mitad alta).
- En modo especial del canal 3, solo el tercer canal FM permite frecuencia independiente por operador — el sexto canal no.

## Referencias cruzadas

- (pendiente de enlazar)

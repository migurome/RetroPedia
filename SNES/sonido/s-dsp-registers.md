---
titulo: Registros del S-DSP
plataforma: [SNES]
categoria: [sonido]
tags: [s-dsp, spc700, audio, registros, echo, adsr]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/S-DSP_registers]
---

# Registros del S-DSP

## Resumen

El S-DSP es el sintetizador de sonido de la SNES: mezcla hasta 8 voces (cada una reproduciendo una muestra BRR con su propia envolvente ADSR/GAIN, pitch y panning), añade un generador de ruido, modulación de pitch entre voces y una unidad de eco con filtro FIR. Todo su comportamiento se controla escribiendo en un conjunto de registros internos, accesibles desde el SPC-700 a través de los puertos `DSPADDR`/`DSPDATA` (`$F2`/`$F3` del S-SMP).

## Detalle técnico

### Acceso a los registros

Los registros del S-DSP no están mapeados directamente en el espacio de memoria del SPC-700; se accede a ellos así: se escribe el número de registro en `$F2` (DSPADDR) y luego se lee o escribe el valor en `$F3` (DSPDATA).

### Registros globales

**MVOLL / MVOLR (volumen principal)** — direcciones `$0C` / `$1C`
Valores de 8 bits con signo que controlan el nivel de salida de los canales izquierdo/derecho.

**EVOLL / EVOLR (volumen de eco)** — direcciones `$2C` / `$3C`
Valores de 8 bits con signo para el nivel del efecto de eco en cada canal.

**KON (key on)** — dirección `$4C`
Un bit por voz (bits 0-7 → voces 0-7). Al activarse, resetea la envolvente a 0, entra en fase de attack y reinicia el decodificador BRR de esa voz. Se sondea cada dos muestras; los bits se limpian 63 ciclos después de ser sondeados.

**KOFF (key off)** — dirección `$5C`
Un bit por voz (bits 0-7 → voces 0-7). Al activarse, silencia la voz haciéndola entrar en fase de release. Se sondea cada dos muestras.

**FLG (flags)** — dirección `$6C`
- Bit 7: reset por software (silencia todas las voces)
- Bit 6: mute general (desactiva el amplificador)
- Bit 5: deshabilita las escrituras al buffer de eco
- Bits 0-4: frecuencia del generador de ruido (`$00`-`$1F` → de 0 Hz a 32 kHz)

**ENDX (fin de muestra)** — dirección `$7C`, solo lectura
Un bit por voz (bits 0-7) que se activa cuando el bloque BRR de esa voz tenía el flag de fin (end) activado.

**EFB (echo feedback)** — dirección `$0D`
Valor de 8 bits con signo que controla cuánta señal de eco se realimenta a sí misma.

**PMON (pitch modulation enable)** — dirección `$2D`
Bits 1-7: activan la modulación de pitch para las voces 1-7 (una voz modula a la siguiente). Fórmula aplicada: `pitch = VxPITCH + (OUTX[x-1] >> 5) * VxPITCH >> 10`.

**NON (noise enable)** — dirección `$3D`
Bits 0-7: sustituyen la salida de la voz correspondiente por el generador de ruido.

**EON (echo enable)** — dirección `$4D`
Bits 0-7: enrutan la salida de cada voz hacia la unidad de eco.

**DIR (sample directory)** — dirección `$5D`
Bits 0-7: byte alto de la dirección del directorio de muestras BRR, alineado a 256 bytes. Dirección real del directorio = valor de DIR × `$100`.

**ESA (echo start address)** — dirección `$6D`
Bits 0-7: puntero de página para la dirección de inicio del buffer de eco.

**EDL (echo delay)** — dirección `$7D`
Bits 0-3: tamaño/retardo del buffer de eco. Retardo = valor × 512 muestras; tamaño del buffer = valor × 2048 bytes.

**FIR0-FIR7 (coeficientes del filtro)** — direcciones `$0F`/`$1F`/`$2F`/`$3F`/`$4F`/`$5F`/`$6F`/`$7F`
Coeficientes de 8 bits con signo para el filtro FIR de 8 taps aplicado a la salida de eco. Filtro identidad (sin efecto): `$7F $00 $00 $00 $00 $00 $00 $00`.

### Registros por voz (8 voces, registros `$X0`-`$X9` donde X es el número de voz)

**VxVOLL / VxVOLR (volumen de voz)** — offsets `$X0` / `$X1`
Amplitud de 8 bits con signo para los canales izquierdo/derecho.

**VxPITCHL / VxPITCHH (pitch de voz)** — offsets `$X2` / `$X3`
Valor de 14 bits en formato de punto fijo 2.12. Frecuencia de muestreo resultante = `(VxPITCH × 32000 Hz) / 0x1000`. Rango: `$0000` a `$3FFF`.

**VxSRCN (fuente de muestra)** — offset `$X4`
Bits 0-7: índice de la entrada del directorio de muestras a usar.

**VxADSR1 (control ADSR 1)** — offset `$X5`
- Bit 7: activa el modo ADSR
- Bits 4-6: decay rate (3 bits)
- Bits 0-3: attack rate (4 bits)

**VxADSR2 (control ADSR 2)** — offset `$X6`
- Bits 5-7: sustain level (3 bits)
- Bits 0-4: sustain rate (5 bits)

**VxGAIN (envolvente GAIN)** — offset `$X7`
- Bit 7: selección de modo (0 = fijo, 1 = variable)
- Si es fijo: bits 0-6 = valor de envolvente × 16
- Si es variable: bits 5-6 = modo (decremento lineal / decremento exponencial / incremento lineal / incremento "bent"), bits 0-4 = tasa

**VxENVX (valor de envolvente)** — offset `$X8`, solo lectura
Bits 0-6: nivel actual de envolvente (7 bits).

**VxOUTX (muestra de salida)** — offset `$X9`, solo lectura
Valor de 8 bits con signo de la muestra ya procesada por la envolvente, antes de aplicar el volumen.

## Notas / gotchas

- El acceso a cualquier registro del S-DSP es siempre indirecto, vía `DSPADDR`/`DSPDATA` — no hay mapeo directo en el espacio de memoria del SPC-700.
- `KON`/`KOFF` se sondean solo cada dos muestras y `KON` tarda 63 ciclos en limpiarse tras el sondeo; no asumir que el efecto es instantáneo ciclo a ciclo.
- La dirección real del directorio de muestras se calcula multiplicando `DIR` por `$100` — hay que mantenerlo alineado a página.
- `EDL` en 0 desactiva efectivamente el buffer de eco (retardo y tamaño en 0).
- El desarrollo completo de las envolventes ADSR/GAIN (tablas de tiempos, fórmulas) se cubre en la entrada dedicada de envolventes del DSP; el formato de las muestras referenciadas por `VxSRCN`/`DIR` se cubre en la entrada de muestras BRR.

## Referencias cruzadas

- (pendiente de enlazar)

---
titulo: ADPCM del AICA — formato, codificador y límites
plataforma: [Dreamcast]
categoria: [sonido]
tags: [audio, memoria]
estado: borrador
comprobado: pc
comprobado_en: "conversión y tamaños con los sonidos de Hyper DBZ, hyperdbz-dreamcast-port"
fecha: 2026-11-13
fuentes: [KOS utils/wav2adpcm/wav2adpcm.c, KOS kernel/arch/dreamcast/sound/snd_sfxmgr.c, proyecto hyperdbz-dreamcast-port]
---

# ADPCM del AICA

## Resumen

El AICA reproduce PCM de 16 y 8 bits y **ADPCM de Yamaha de 4 bits** (un cuarto del tamaño de PCM de 16 bits). Con solo 2 MB de RAM de audio, el ADPCM es casi obligatorio para efectos y voces. Está en `borrador` porque el codificador se comprobó solo en PC (ida y vuelta) y en Flycast, todavía sin escucharlo con cuidado ni probarlo en la consola.

## Detalle técnico

### Algoritmo (igual que `pcm2adpcm` de KOS)

Cada muestra de 16 bits da un código de 4 bits: bit 3 = signo y bits 0–2 = magnitud. **La primera muestra va en el nibble bajo.** El estado es `history` (último valor) y `step_size` (127–24576):

```python
STEP = (230, 230, 230, 230, 307, 409, 512, 614)
def encode(samples):
    history, step = 0, 127
    for i, v in enumerate(samples):
        diff = (v & -8) - history
        code = min(7, (abs(diff) << 16) // (step << 14)) | (8 if diff < 0 else 0)
        # ...guardar code: muestras pares en el nibble bajo, impares en el alto
        d = code & 7
        delta = min(32767, ((1 + (d << 1)) * step) >> 3)
        history = clamp(history - delta if code & 8 else history + delta, -32768, 32767)
        step = clamp((STEP[d] * step) >> 8, 127, 24576)
```

Una senoidal de 440 Hz da unos 24 dB de relación señal/ruido al decodificarla con el decodificador de KOS (que añade un filtro paso alto `history * 254 / 256`).

### Cargar sonidos con KOS

- Desde WAV: `snd_sfx_load()` acepta el formato `0x0014` (ITU G.723, canales estéreo separados) y el `0x0020` (Yamaha, canales entrelazados).
- Desde memoria: `snd_sfx_load_raw_buf(buf, len, rate, 4, canales)`, donde 4 bits indica ADPCM. Sube los datos a la RAM de audio.
- `snd_mem_available()` informa de la RAM de audio libre: tras `snd_init()` quedan unos **1.856 KB**.

## Notas / gotchas

- **Límite de longitud:** un efecto de más de **65.534 muestras** (≈3 s a 22.050 Hz) provoca un aviso en `snd_sfx_load_raw_buf`, porque el contador del canal es de 16 bits. Las voces largas necesitan streaming (`snd_stream_*`).
- **Medido en un caso real:** dos personajes de MUGEN suman 613 sonidos y 11 MB en ADPCM a 22 kHz mono. Solo caben 143 a la vez, y 71 superan el límite de longitud. Hay que cargarlos bajo demanda.
- **Streams contados:** KOS tiene `SND_STREAM_MAX = 4` (`stream.h`). Con la música en uno, solo quedan 3 voces de stream: un súper de MUGEN lanza 4-5 sonidos largos a la vez y, si todos van por stream, cada nuevo corta otro (hyperdbz-dreamcast-port, marca de la 7.110). Mejor así: lo de hasta 65.534 muestras se sube **entero a la RAM de audio al pedirlo** (por trozos con `snd_mem_malloc` + `spu_memload`, sin búfer grande en la RAM principal) y se toca en un canal reservado con `snd_sfx_chn_alloc` y un `AICA_CMD_CHAN` como el de `snd_sfx_play_ex` (tipo `AICA_SM_ADPCM`, `length` en muestras); al acabar, `snd_sfx_stop`, `snd_sfx_chn_free` y `snd_mem_free`. El stream queda solo para lo más largo. Pico en un súper de Vegeta SSJ: 96 KB de RAM de audio.
- El remuestreo previo importa tanto como el ADPCM. Un filtro de caja con interpolación lineal sirve para medir tamaños, pero para la calidad final hay que usar herramientas dedicadas (sox, ffmpeg) y escuchar el resultado. En el port de Mega Drive el audio hecho sin método acabó siendo un cuello de botella.

### Bucles y saltos en streaming (música)

El AICA **no olvida el estado** del ADPCM (`history` y `step_size`) ni lo decae (Flycast, `DecodeADPCM`). Si un stream salta en los datos (el `bgmloopend` → `bgmloopstart` de MUGEN), el decodificador sigue con el estado del punto de salida y queda un desfase de continua que no desaparece.

Solución medida en hyperdbz-dreamcast-port (paso 4.4): codificar otra vez el principio del bucle **desde el estado del final**, muestra a muestra, hasta que el estado coincida exactamente con el de la codificación original en la misma muestra (la «costura»). Desde ahí los bytes son idénticos. En la consola: datos hasta el final del bucle → costura → seguir en `inicio + longitud de la costura`. En 42 temas de música, la costura convergió siempre (de 0,01 a 22 s). Con ruido blanco fuerte puede no converger: hay que comprobarlo al convertir.

- Estéreo en `snd_stream` de KOS: un byte por muestra estéreo (izquierdo en el nibble alto), que `snd_adpcm_split` separa; tramos y saltos alineados a 32 bytes.
- Reanudar un stream ADPCM a mitad (una pausa con `snd_stream_stop`) tiene el mismo problema: el canal arranca con el estado inicial.

## Referencias cruzadas

- [Dreamcast — overview](../README.md)

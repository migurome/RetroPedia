---
titulo: Reproducción de PCM por el YM2612 con temporización por ciclos (Bumbershoot)
plataforma: [MegaDrive]
categoria: [sonido]
tags: [ym2612, dac, pcm, z80, bankswitching, timing, ciclos]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/04/01/genesis-digital-playback/]
---

# Reproducción de PCM por el YM2612 con temporización por ciclos (Bumbershoot)

## Resumen

Bumbershoot Software detalla una implementación completa de reproducción de muestras PCM de 8 bits en Genesis a través del DAC del YM2612. `ym2612-operations.md` ya resume el procedimiento en tres pasos (activar DAC, escribir muestras a intervalos, desactivar DAC); esta entrada aporta el "cómo" concreto que falta ahí: temporización exacta por conteo de ciclos del Z80 (no hay DMA para el DAC) y el bankswitching necesario para que el Z80, limitado a 64KB de espacio de direcciones, pueda leer datos de muestra desde cualquier punto de una ROM de hasta 16MB.

## Detalle técnico

### El problema de fondo

El YM2612 tiene DAC de 8 bits igual que una Sound Blaster de la época, pero a diferencia de esta **no tiene ningún mecanismo de DMA para alimentarlo**. Toda la temporización debe hacerse por software: hay que escribir un nuevo valor en el registro del DAC exactamente en el instante en que debe sonar, y mientras se está haciendo esto es prácticamente imposible usar a la vez los canales FM del mismo chip (a menos que el driver esté construido con mucho cuidado). El autor cita como referencia juegos que sí lo consiguieron de forma notable: *Sonic the Hedgehog*, *Earthworm Jim*, y en particular *Toy Story*, que logra tener un reproductor de música tipo MOD completo sonando *a la vez* que clips de voz.

### Cálculo de ciclos

El planteamiento es equivalente a la síntesis de onda cuadrada por conteo de ciclos típica de un Apple II. El Z80 corre a 3.58MHz; para reproducir muestras a 8kHz hace falta escribir un nuevo valor de DAC cada:

```
3580000 / 8000 = 447.5 ciclos
```

Un bucle de temporización básico con `djnz`:

```asm
        ld      b, N
loop:   djnz    loop
```

cuesta **2 + 13×N** ciclos, y se ajusta con NOPs y `ld` adicionales para clavar el número exacto de ciclos objetivo.

### Inicialización del YM2612 para modo DAC

Deshabilita el LFO y el modo de canal 3 independiente, hace key-off de los siete "canales" (los 6 FM más el propio DAC), activa el DAC (registro `$2B`) y habilita la salida por ambos altavoces:

```asm
main:   ld      hl, $4000

        ;; Set up YM2612
        ld      (hl), $22       ; Disable Low-Frequency Oscillator
        xor     a
        ld      ($4001), a
        ld      (hl), $27       ; Disable independent Channel 3
        ld      ($4001), a
        ld      b, 7
ymlp:   ld      (hl), $28       ; Key off all channels
        ld      ($4001), a
        inc     a
        djnz    ymlp
        ld      (hl), $2b       ; Enable DAC
        ld      a, $80
        ld      ($4001), a
        ld      a, $b6          ; Enable speakers for DAC
        ld      ($4002), a
        ld      a, $c0
        ld      ($4003), a
```

(Los registros `$22`, `$27`, `$28`, `$2B` están detallados bit a bit en `ym2612-registers.md`; aquí se ven encadenados en una secuencia de inicialización real.)

### Bankswitching: leer una ROM de 16MB desde un Z80 de 64KB

El Z80 solo direcciona 64KB de forma nativa, pero la Genesis expone hasta 16MB de espacio de cartucho dividido en 512 bancos de 32KB. Un puerto serie de 9 bits en `$6000` selecciona qué banco de 32KB ocupa la ventana `$8000-$FFFF` del Z80. La rutina siguiente convierte una dirección de 24 bits (guardada en `ptr`) en los 9 bits de banco (enviados en serie, un bit por escritura a `$6000`) más el offset dentro de la ventana:

```asm
go:     ld      a, (ptr)
        ld      e, a
        ld      a, (ptr+1)
        push    af
        or      $80
        ld      d, a
        pop     af
        rlca
        ld      ($6000), a
        ld      a, (ptr+2)
        ld      b, 8
ldlp:   ld      ($6000), a
        rrca
        djnz    ldlp
```

Tras esto, `de` apunta dentro de la ventana `$8000-$FFFF` a la dirección real de la muestra, ya bankswitcheada.

### Espera antes de empezar y bucle de reproducción

Bucle de espera hasta que un contador de 16 bits (`counter`) tenga algún bit distinto de cero (es decir, hasta que haya trabajo pendiente):

```asm
wait:   ld      a, (counter)
        and     a
        jr      nz, go
        ld      a, (counter+1)
        and     a
        jr      z, wait
```

Bucle principal: escribe el siguiente byte de muestra en el registro `$2A` (DAC output) del banco de registros en `$4001`, avanza el puntero de datos y decrementa el contador de muestras restantes:

```asm
        ld      (hl), $2a
        ld      a, (de)
        ld      ($4001), a
        inc     de
        ld      bc, (counter)
        dec     bc
        ld      (counter), bc
        ld      a, b
        or      c
        jr      nz, lp
```

Este cuerpo de bucle cuesta 102 ciclos fijos; de los 447 ciclos objetivo por muestra quedan 345 ciclos por rellenar, que a 13 ciclos por vuelta del bucle `djnz` equivalen a aproximadamente 26 iteraciones de relleno para clavar la temporización a 8kHz.

### Arranque mínimo

```asm
        org     0

        di
        im      1
        ld      sp, $2000
        jr      main
```

## Notas / gotchas

- No hay DMA para el DAC del YM2612: toda la temporización de reproducción PCM es responsabilidad exclusiva del software, ciclo a ciclo.
- Mezclar FM y DAC a la vez es extremadamente difícil de conseguir bien (solo unos pocos juegos comerciales lo lograron); si no es imprescindible, es más simple sacrificar los canales FM mientras se reproduce PCM.
- El Z80 solo ve 64KB a la vez: cualquier banco de datos de muestra que no quepa en la ventana fija de RAM/registros necesita bankswitching a través del puerto serie de `$6000` (9 bits, uno por escritura, más el bit alto de dirección combinado aparte).
- La temporización exacta (cuántos NOPs/instrucciones de relleno hacen falta) depende de la frecuencia de muestreo objetivo; el ejemplo aquí está ajustado a 8kHz sobre un Z80 a 3.58MHz.

## Referencias cruzadas

- [Operaciones comunes del YM2612](ym2612-operations.md) — resume el mismo procedimiento de PCM en 3 pasos de alto nivel; esta entrada aporta la implementación de bajo nivel (ciclos exactos, bankswitching) que falta ahí.
- [Mapa de registros del YM2612](ym2612-registers.md) — detalle bit a bit de los registros `$22`, `$27`, `$28`, `$2A` y `$2B` usados en la inicialización.
- [Driver de música PSG en Z80 con decaimiento de subframe (Bumbershoot)](bumbershoot-z80-driver-psg.md) — mismo blog y mismo enfoque de driver en Z80, pero para el PSG en vez del DAC del YM2612.

---
titulo: S-SMP — el procesador de sonido del SNES
plataforma: [SNES]
categoria: [sonido]
tags: [s-smp, spc700, audio, timers, apu, memoria]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/S-SMP]
---

# S-SMP — el procesador de sonido del SNES

## Resumen

El módulo de sonido de la SNES (SHVC-SOUND) integra el S-SMP (la CPU de sonido, basada en el procesador Sony SPC-700), el S-DSP (el sintetizador), la Audio-RAM, un DAC y un amplificador estéreo. El S-SMP corre de forma totalmente independiente de la CPU principal (5A22/65816) y solo se comunica con ella a través de un pequeño conjunto de puertos de E/S; todo el código y los datos de audio (incluidas las muestras BRR) deben transferirse por ahí.

## Detalle técnico

### Visión general de hardware

El SPC-700 dentro del S-SMP corre a 1.024 MHz e incluye tres temporizadores (dos a 8 kHz y uno a 64 kHz), registros de comunicación con la CPU principal, y una ROM de arranque (IPL Boot ROM) que se encarga de recibir el programa de sonido real desde el cartucho.

### Mapa de memoria

El SPC-700 direcciona 64 KB en su espacio de memoria de 16 bits:

| Rango | Contenido |
|---|---|
| `$0000-$00EF` | RAM de página cero |
| `$00F0-$00FF` | Registros de la CPU de sonido |
| `$0100-$01FF` | RAM de pila (stack) |
| `$0200-$FFBF` | RAM |
| `$FFC0-$FFFF` | IPL ROM o RAM (la ROM se puede desmapear vía el bit 7 del registro Control) |

### Registros mapeados en memoria

Ocupan los últimos 16 bytes de la página cero:

| Registro | Dirección | Bits | Tipo | Función |
|---|---|---|---|---|
| TEST | `$F0` | `IIEE TRWH` | W8 | No documentado; controla wait states |
| CONTROL | `$F1` | `I.CC .210` | W8 | Activa la IPL, limpia puertos, activa timers |
| DSPADDR | `$F2` | `RAAA AAAA` | RW8 | Selecciona el registro del DSP (bit 7 = solo lectura) |
| DSPDATA | `$F3` | `DDDD DDDD` | RW8 | Acceso al dato del registro del DSP seleccionado |
| CPUIO0-3 | `$F4-$F7` | `DDDD DDDD` | RW8 | Puertos de comunicación con la CPU principal |
| T0TARGET | `$FA` | `TTTT TTTT` | W8 | Intervalo del timer 0 (8kHz) |
| T1TARGET | `$FB` | `TTTT TTTT` | W8 | Intervalo del timer 1 (8kHz) |
| T2TARGET | `$FC` | `TTTT TTTT` | W8 | Intervalo del timer 2 (64kHz) |
| T0OUT | `$FD` | `0000 CCCC` | R8 | Cuenta del timer 0 (se limpia al leer) |
| T1OUT | `$FE` | `0000 CCCC` | R8 | Cuenta del timer 1 (se limpia al leer) |
| T2OUT | `$FF` | `0000 CCCC` | R8 | Cuenta del timer 2 (se limpia al leer) |

Al encender la consola, `TEST` vale `$0A`. Tanto al encender como al resetear, `CONTROL` vale `$B0`.

### Arranque (IPL)

La IPL (Initial Program Loader) inicializa el SPC-700 y habilita la transferencia de un programa desde la CPU principal a través de los puertos de comunicación (`CPUIO0-3`, que se corresponden con `$2140-$2143` vistos desde el 65816). La secuencia usa señales de negociación (`$AA`/`$BB`/`$CC`), transferencia de la dirección destino por los puertos 2-3, y carga de datos por el puerto 1 con confirmación (eco) por el puerto 0. Cargar por IPL tarda aproximadamente 520 ciclos de reloj maestro por byte transferido, lo que permite subir del orden de 650 bytes por cada frame a 60 Hz. El protocolo completo, paso a paso, se documenta en la entrada dedicada de arranque del SPC700 de esta base de conocimiento.

## Notas / gotchas

- Nunca se debe escribir en el registro `TEST` (`$F0`): activar cualquiera de sus bits de wait state ralentiza el reloj del SPC-700 y puede provocar comportamiento indefinido o cuelgues.
- El acceso al S-DSP desde el SPC-700 siempre pasa por el par de registros `DSPADDR`/`DSPDATA` (`$F2`/`$F3`); no existe acceso directo a los registros del DSP desde el mapa de memoria del SPC-700.
- La velocidad de carga por IPL (~520 ciclos de reloj maestro por byte) es una limitación real a tener en cuenta al diseñar la subida de datos/código de audio durante el arranque del juego; para transferencias más rápidas existen técnicas alternativas (ver Blargg SPC upload).
- El bit 7 del registro `CONTROL` permite desmapear la IPL ROM de `$FFC0-$FFFF`, dejando esa zona como RAM normal una vez que ya no se necesita el bootloader.

## Referencias cruzadas

- (pendiente de enlazar)

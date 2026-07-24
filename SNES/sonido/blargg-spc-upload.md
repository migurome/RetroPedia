---
titulo: Técnica de subida "Blargg SPC upload"
plataforma: [SNES]
categoria: [sonido]
tags: [spc700, s-smp, spc, upload, audio, emulacion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Blargg_SPC_upload]
---

# Técnica de subida "Blargg SPC upload"

## Resumen

Es un método (ideado por Blargg) para reproducir en hardware real un archivo `.spc` (una captura completa del estado del SPC700 y del S-DSP en un instante dado, tal y como la generan los emuladores/rippers de música). El truco consiste en restaurar cuidadosamente ese estado capturado —registros del DSP, RAM completa, registros de la CPU— a través del protocolo de arranque normal de la IPL, sin que la propia carga interfiera con el sonido que se está restaurando.

## Detalle técnico

### Idea general

El proceso se divide en tres fases secuenciales de restauración, todas ellas subidas usando el protocolo estándar de arranque del SPC700 (ver la entrada de arranque del SPC700).

### 1. Restauración de los registros del DSP

Antes de subir el resto de la instantánea, se modifican a propósito algunos registros del DSP para evitar efectos no deseados durante la carga:

| Registro | Valor forzado | Motivo |
|---|---|---|
| KOFF | `$00` | Evita detener notas que ya estén sonando |
| KON | `$00` | Retrasa el disparo de notas hasta que la restauración esté completa |
| FLG | `$60` | Silencia la salida y desactiva las escrituras de eco mientras se sube la RAM |
| EDL | `$00` | Vacía el puntero del buffer de eco de forma segura |

Un pequeño programa en ensamblador transfiere estos valores mediante los puertos `$F2` (dirección) y `$F3` (dato):

```asm
mov x,#dsp_regs
mov y,#0
next: mov a,(x)+
mov $F2,y
mov $F3,a
inc y
bpl next
jmp !$FFC0
```

### 2. Restauración de la RAM, con parcheo de ciertas direcciones

Algunas posiciones de E/S necesitan un tratamiento especial en vez de subirse tal cual:

| Dirección | Valor | Motivo |
|---|---|---|
| `$00F1` | `$80` | Activa la IPL ROM; detiene los timers |
| `$00F2` | `$6C` | Deja seleccionado el registro de dirección del DSP (FLG) |
| `$00F4` | `$F4` | Necesario para la estabilidad del propio loader |

Además, las direcciones `$0000-$0001` deben sobrescribirse con los mismos valores que tenía la instantánea original, ya que el propio bootloader las usa internamente como punteros de destino durante la carga.

### 3. Código final de restauración

Una subrutina personalizada, colocada en una zona libre de la RAM (a menudo rellenada con secuencias de bytes `$FF` en el resto del espacio), se encarga de restaurar el estado final del procesador:

```asm
mov $00,#spc.ram[0]
mov $01,#spc.ram[1]
mov x,#spc.sp - 1
mov sp,X
mov y,#spc.y
mov x,#spc.x
mov a,#spc.a
mov $F1,#spc.ram[$F1] AND $CF
mov $F3,#spc.dsp[FLG]
mov $F2,#$7D
mov $F3,#spc.dsp[EDL]
mov $F2,#$4C
mov $F3,#spc.dsp[KON]
mov $F2,#spc.ram[$F2]
pop psw
setp or clrp
jmp !spc.pc
```

## Notas / gotchas

- El puntero del buffer de eco debe llegar a cero antes de restaurar los valores finales de `EDL`; en el peor caso esto puede tardar del orden de 1/4 de segundo, así que la restauración final no es instantánea.
- Los bits 4-5 de `$F1` se limpian explícitamente para evitar que los puertos de entrada interfieran durante la restauración.
- La ejecución del código final debe evitar solaparse con las direcciones de inicio del buffer de eco, para no introducir condiciones de carrera justo al arrancar.
- Los valores de los puertos de entrada deben restaurarse inmediatamente después de que termine de ejecutarse el parche, para no perder sincronismo con lo que el juego/driver espera.
- Existe una implementación de referencia en C, `spc_loader.c`, mencionada en la documentación original, útil como base para portar esta técnica.

## Referencias cruzadas

- (pendiente de enlazar)

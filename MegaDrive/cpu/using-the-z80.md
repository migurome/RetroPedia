---
titulo: Usando el Z80 en la Mega Drive
plataforma: [MegaDrive]
categoria: [cpu]
tags: [z80, 68000, sonido, ym2612, dma, comunicacion-cpu]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/using-the-z80]
---

# Usando el Z80 en la Mega Drive

## Resumen

El Z80 funciona como segundo procesador en la Mega Drive, encargado principalmente del hardware de sonido. Puede usarse en modo PCM (con el 68000 gestionando el resto de tareas de sonido) o de forma independiente para manejar todo el audio. Esta entrada cubre cómo cargar un programa en el Z80, cómo comunicarse con él desde el 68000, y las precauciones necesarias al combinarlo con DMA.

## Detalle técnico

### Constantes esenciales

```
Z80Ram:     equ $A00000  ; Where Z80 RAM starts
Z80BusReq:  equ $A11100  ; Z80 bus request line
Z80Reset:   equ $A11200  ; Z80 reset line
```

Características técnicas:
- `Z80Ram`: dirección base de la RAM del Z80.
- `Z80BusReq` y `Z80Reset`: registros de 16 bits para el control del Z80.
- Capacidad máxima de la RAM del Z80: 8KB.
- Rango de ejecución: desde la dirección $0000.

### Procedimiento de carga del programa

**Fase 1: Obtener acceso al bus**

```
move.w  #$000, (Z80Reset)
move.w  #$100, (Z80BusReq)
move.w  #$100, (Z80Reset)
```

Tres pasos secuenciales:
1. Afirmar reset escribiendo $000 a `Z80Reset`.
2. Solicitar el bus escribiendo $100 a `Z80BusReq`.
3. Liberar reset escribiendo $100 a `Z80Reset`.

**Fase 2: Copiar el programa**

Requisito crítico: "you must use byte accesses when touching Z80 RAM" — los accesos en palabra (word) no funcionarán.

```
lea     (Z80Prog), a0
lea     (Z80Ram), a1
move.w  #Z80ProgSize-1, d0
@Loop:
move.b  (a0)+, (a1)+
dbf     d0, @Loop
```

**Fase 3: Reset apropiado**

```
move.w  #$000, (Z80Reset)
move.w  #20, d0
@Wait:
dbf     d0, @Wait
```

Hay que esperar un mínimo de 192 ciclos (esperar más no supone ningún problema) para que el YM2612 funcione correctamente.

**Fase 4: Liberar el control**

```
move.w  #$100, (Z80Reset)
move.w  #$000, (Z80BusReq)
rts
```

### Comunicación entre el 68000 y el Z80

El único método es el acceso a direcciones compartidas en la RAM del Z80. No se puede acceder a ella mientras el Z80 está ejecutándose, pero se puede solicitar el bus, lo que pausa al Z80.

**Macro PauseZ80**

```
PauseZ80: macro
    move.w  #$100, (Z80BusReq)
@WaitZ80\@:
    btst    #0, (Z80BusReq)
    bne.s   @WaitZ80\@
endm
```

Procedimiento: se escribe $100 en `Z80BusReq`, y luego se lee repetidamente hasta que la lectura devuelva $100.

**Macro ResumeZ80**

```
ResumeZ80: macro
    move.w  #$000, (Z80BusReq)
endm
```

No requiere espera posterior.

**Macro FastPauseZ80**

```
FastPauseZ80: macro
    move.w  #$100, (Z80BusReq)
endm
```

Se usa para los casos en los que solo se necesita evitar que el Z80 acceda al bus del 68000, sin necesidad de acceder a la RAM del Z80.

### Acceso al YM2612

El chip YM2612 reside en el bus del Z80. El procedimiento seguro para acceder a él es:

1. Invocar `PauseZ80`.
2. Acceder al YM2612.
3. Invocar `ResumeZ80`.

Este patrón reemplaza al método anterior de mantener el bus solicitado de forma continua.

### Interacción entre el Z80 y las transferencias DMA

Problema de compatibilidad citado por el autor: "the Z80 and DMA transfers don't play nice in some board revisions" (especialmente en versiones tempranas de la placa), lo que causa datos corruptos en la memoria de vídeo. Esto no ocurre en la mayoría de revisiones ni en los emuladores.

Solución obligatoria:

1. Solicitar el bus del Z80 (usando `FastPauseZ80`).
2. Ejecutar las transferencias DMA.
3. Liberar el bus del Z80 (usando `ResumeZ80`).

`FastPauseZ80` es suficiente porque no se accede a la RAM del Z80 durante el DMA, y el Z80 estará definitivamente pausado en el momento en que se escriba el comando DMA.

## Notas / gotchas

- Los accesos a la RAM del Z80 desde el 68000 deben hacerse siempre en byte (`move.b`), nunca en word.
- El Z80 puede quedar interrumpido a mitad de una instrucción: "The Z80 may get interrupted in the middle of an instruction!" Si el Z80 estaba ejecutando una lectura/escritura de 16 bits, podría haber completado solo la mitad. La operación atómica máxima segura es un acceso de 8 bits.
- Tras el reset del Z80, hay que esperar al menos 192 ciclos antes de que el YM2612 funcione correctamente.
- En algunas revisiones de placa (sobre todo tempranas), el Z80 y las transferencias DMA no conviven bien y pueden corromper la memoria de vídeo; hay que pausar el bus del Z80 durante el DMA como medida de seguridad.

## Referencias cruzadas

- (pendiente de enlazar)

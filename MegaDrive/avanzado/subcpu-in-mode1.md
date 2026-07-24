---
titulo: Usar el Sub-CPU de Mega CD en modo 1
plataforma: [MegaDrive]
categoria: [avanzado]
tags: [mega-cd, sub-cpu, modo-1, gate-array, prg-ram]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/subcpu-in-mode1]
---

# Usar el Sub-CPU de Mega CD en modo 1

## Resumen

Esta entrada trata sobre el **Sub-CPU de la Mega CD** (el segundo 68000 del add-on de CD de Mega Drive). Explica cómo ejecutar programas personalizados en el Sub-CPU arrancando desde cartucho, sin cargar la BIOS de Mega CD (el llamado "modo 1"), útil cuando no se necesita acceso a CD ni RAM de respaldo.

## Detalle técnico

### Configuración de la cabecera de la ROM

Añadir el carácter `'C'` al campo de soporte de dispositivos para indicar compatibilidad con Mega CD:

```
dc.b  'JC'
dcb.b $1A0-*, ' '
```

**Advertencia clave:** "El soporte de esto en emuladores y flashcarts es irregular en este momento, con muchos de ellos teniendo su propio procedimiento particular para detectar el modo 1."

### Direcciones de memoria

```
HwVersion:   equ $A10001   ; Console region, version, etc.
CdSubCtrl:   equ $A12000   ; Sub-CPU reset/busreq, etc.
CdMemCtrl:   equ $A12002   ; Mega CD memory mode, bank, etc.
CdBootRom:   equ $400000   ; Main-CPU boot ROM
CdPrgRam:    equ $420000   ; PRG-RAM window
CdWordRam:   equ $600000   ; WORD-RAM window
```

### Detección de Mega CD

Dos métodos de detección:

1. Comprobar si el bit 5 de `HwVersion` es 0
2. Comprobar la firma "SEGA" en `CdBootRom+$100`

El código proporcionado comprueba ambas condiciones:

```
TestForCd:
    btst    #5, (HwVersion)
    beq.s   @Yep
    cmpi.l  #'SEGA', (CdBootRom+$100)
    beq.s   @Yep
@Nope:
    moveq   #0, d0
    rts
@Yep:
    moveq   #1, d0
    rts
```

Devuelve `d0 = 0` si no hay Mega CD, `1` si está presente.

### Reset del Gate Array

La secuencia exacta es crítica — no se permiten desviaciones:

```
InitCd:
    move.w  #$FF00, (CdMemCtrl)
    move.b  #$03, (CdSubCtrl+1)
    move.b  #$02, (CdSubCtrl+1)
    move.b  #$00, (CdSubCtrl+1)
    moveq   #$7F, d0
    dbf     d0, *
    rts
```

### Carga del programa del Sub-CPU

Primero, resetear y solicitar acceso al bus:

```
LoadSubCpu:
    lea     (CdSubCtrl+1), a0
    move.b  #$00, (a0)                  ; Reset the Sub-CPU
@Reset:
    move.b  (a0), d0
    and.b   #$01, d0
    cmp.b   #$00, d0
    bne.s   @Reset
    
    move.b  #$03, (a0)                  ; Request the Sub-CPU bus
@BusReq:
    move.b  (a0), d0
    and.b   #$03, d0
    cmp.b   #$03, d0
    bne.s   @BusReq
```

Copiar el programa a PRG-RAM (hasta 128KB):

```
    lea     (CdPrgRam), a5
    move.w  #$0000, (CdMemCtrl)
    subq.w  #1, d7
@Load:
    move.w  (a6)+, (a5)+
    dbf     d7, @Load
```

Resetear el Sub-CPU de nuevo y liberarlo:

```
    move.b  #$00, (a0)
@Reset2:
    move.b  (a0), d0
    and.b   #$01, d0
    cmp.b   #$00, d0
    bne.s   @Reset2
    
    move.b  #$01, (a0)
@StartUp:
    move.b  (a0), d0
    and.b   #$01, d0
    cmp.b   #$01, d0
    bne.s   @StartUp
    rts
```

**Nota crítica:** "Es _absolutamente_ necesario resetearlo de nuevo o no funcionará, a pesar de que nunca le dimos margen para ejecutarse antes."

### Puertos de comunicación

Direcciones del Main-CPU:

```
CdCommMain1-8:  $A12010–$A1201E  ; Main→Sub (16-bit each)
CdCommSub1-8:   $A12020–$A1202E  ; Sub→Main (16-bit each)
```

Direcciones del Sub-CPU: reemplazar `$A120xx` por `$FF80xx`

Activación de interrupción (Main→Sub vía IRQ2):

```
move.b  #$81, (CdSubCtrl)
@WaitIrqAck:
    move.b  (CdSubCtrl), d0
    and.b   #$01, d0
    beq.s   @WaitIrqAck
```

### Programa mínimo para el Sub-CPU

Tabla de vectores (256 bytes) con punto de entrada y manejadores:

```
dc.l    $80000, EntryPoint, ErrorInt, ErrorInt
[... 60 more vector entries ...]

EntryPoint:
    bra.s   *

Irq2:
    rte

ErrorInt:
    bra.s   *
```

## Notas / gotchas

- El soporte de este modo en emuladores y flashcarts es irregular; muchos usan su propio procedimiento particular para detectar el modo 1.
- La secuencia de reset del Gate Array debe seguirse exactamente, sin desviaciones.
- Es obligatorio resetear el Sub-CPU una segunda vez tras cargar el programa en PRG-RAM, aunque nunca se le haya dado margen de ejecución previamente — de lo contrario no funcionará.
- La carga a PRG-RAM está limitada a 128KB.

## Referencias cruzadas

- (pendiente de enlazar)

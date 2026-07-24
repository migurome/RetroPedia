---
titulo: Configuración inicial del VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, inicializacion, registros, vram, cram, vsram]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/vdp-setup]
---

# Configuración inicial del VDP

## Resumen

Antes de trabajar con el hardware de vídeo de la Mega Drive es necesario preparar el VDP (Video Display Processor) y limpiar sus distintas memorias (VRAM, CRAM, VSRAM) para partir de un estado conocido. Esta entrada cubre los puertos de hardware, los registros de configuración inicial y la rutina de limpieza de memoria de vídeo.

## Detalle técnico

### Puertos de hardware del VDP

El VDP dispone de tres puertos accesibles como valores de 16 o 32 bits:

```
VdpCtrl:    equ $C00004  ; Puerto de control del VDP
VdpData:    equ $C00000  ; Puerto de datos
HvCounter:  equ $C00008  ; Contador H/V
```

El puerto de control ($C00004) se utiliza para escribir comandos y leer el estado. El puerto de datos ($C00000) accede a la memoria de vídeo tras establecer la dirección mediante el puerto de control. El contador H/V ($C00008) reporta la posición de píxel actual durante la renderización.

### Registros del VDP

Se definen las siguientes constantes para acceso a registros:

```
VDPREG_MODE1:     equ $8000  ; Registro de modo #1
VDPREG_MODE2:     equ $8100  ; Registro de modo #2
VDPREG_MODE3:     equ $8B00  ; Registro de modo #3
VDPREG_MODE4:     equ $8C00  ; Registro de modo #4

VDPREG_PLANEA:    equ $8200  ; Dirección tabla Plano A
VDPREG_PLANEB:    equ $8400  ; Dirección tabla Plano B
VDPREG_SPRITE:    equ $8500  ; Dirección tabla Sprite
VDPREG_WINDOW:    equ $8300  ; Dirección tabla Window
VDPREG_HSCROLL:   equ $8D00  ; Dirección tabla HScroll

VDPREG_SIZE:      equ $9000  ; Tamaño Plano A y B
VDPREG_WINX:      equ $9100  ; Posición división Window X
VDPREG_WINY:      equ $9200  ; Posición división Window Y
VDPREG_INCR:      equ $8F00  ; Autoincremento
VDPREG_BGCOL:     equ $8700  ; Color fondo
VDPREG_HRATE:     equ $8A00  ; Frecuencia IRQ HBlank

VDPREG_DMALEN_L:  equ $9300  ; Longitud DMA (bajo)
VDPREG_DMALEN_H:  equ $9400  ; Longitud DMA (alto)
VDPREG_DMASRC_L:  equ $9500  ; Fuente DMA (bajo)
VDPREG_DMASRC_M:  equ $9600  ; Fuente DMA (medio)
VDPREG_DMASRC_H:  equ $9700  ; Fuente DMA (alto)
```

### Configuración del VDP

**Paso inicial:** `tst.w (VdpCtrl)` — leer el puerto de control para cancelar operaciones previas y establecer un estado conocido.

**Configuración de registros:**

```
lea     (VdpCtrl), a0

move.w  #VDPREG_MODE1|$04, (a0)
move.w  #VDPREG_MODE2|$04, (a0)
move.w  #VDPREG_MODE3|$00, (a0)
move.w  #VDPREG_MODE4|$81, (a0)

move.w  #VDPREG_PLANEA|$30, (a0)
move.w  #VDPREG_PLANEB|$07, (a0)
move.w  #VDPREG_SPRITE|$78, (a0)
move.w  #VDPREG_WINDOW|$34, (a0)
move.w  #VDPREG_HSCROLL|$3D, (a0)

move.w  #VDPREG_SIZE|$01, (a0)
move.w  #VDPREG_WINX|$00, (a0)
move.w  #VDPREG_WINY|$00, (a0)
move.w  #VDPREG_INCR|$02, (a0)
move.w  #VDPREG_BGCOL|$00, (a0)
move.w  #VDPREG_HRATE|$FF, (a0)
```

### Limpieza de memoria de vídeo

Tres memorias requieren limpieza: VRAM, CRAM y VSRAM.

**Comandos de dirección:**

```
VRAM_ADDR_CMD:  equ $40000000
CRAM_ADDR_CMD:  equ $C0000000
VSRAM_ADDR_CMD: equ $40000010
```

**Tamaños en bytes:**

```
VRAM_SIZE:    equ 65536
CRAM_SIZE:    equ 128
VSRAM_SIZE:   equ 80
```

**Código de limpieza:**

```
moveq   #0, d0
lea     (VdpCtrl), a0
lea     (VdpData), a1

; Limpiar VRAM
move.l  #VRAM_ADDR_CMD, (a0)
move.w  #(VRAM_SIZE/4)-1, d1
@ClearVram:
    move.l  d0, (a1)
    dbf     d1, @ClearVram

; Limpiar CRAM
move.l  #CRAM_ADDR_CMD, (a0)
move.w  #(CRAM_SIZE/4)-1, d1
@ClearCram:
    move.l  d0, (a1)
    dbf     d1, @ClearCram

; Limpiar VSRAM
move.l  #VSRAM_ADDR_CMD, (a0)
move.w  #(VSRAM_SIZE/4)-1, d1
@ClearVsram:
    move.l  d0, (a1)
    dbf     d1, @ClearVsram
```

## Notas / gotchas

- Advertencia crítica del autor: "Do _not_ use the `clr` instruction for this. Because of a quirk of the 68000, that instruction will first do a dummy read before writing back a 0, and the VDP gets confused if this happens." (No usar la instrucción `clr` para limpiar memoria de vídeo: por una particularidad del 68000, esa instrucción hace primero una lectura ficticia antes de escribir el 0, y eso confunde al VDP).
- El primer `tst.w (VdpCtrl)` es necesario para dejar el VDP en un estado conocido antes de configurar registros.

## Referencias cruzadas

- (pendiente de enlazar)

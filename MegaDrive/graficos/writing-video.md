---
titulo: Escritura en memoria de vídeo (VRAM/CRAM/VSRAM)
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, vram, cram, vsram, dma, puerto-control]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/writing-video]
---

# Escritura en memoria de vídeo (VRAM/CRAM/VSRAM)

## Resumen

Explica cómo direccionar y escribir datos en las tres memorias de vídeo del VDP (VRAM, CRAM y VSRAM) a través del puerto de control y el puerto de datos, tanto con direcciones fijas conocidas en tiempo de ensamblaje como con direcciones variables calculadas en registros de datos.

## Detalle técnico

### Constantes esenciales

Las direcciones base del VDP son:
- `VdpCtrl: equ $C00004` (puerto de control VDP)
- `VdpData: equ $C00000` (puerto de datos VDP)

Los comandos de dirección para diferentes memorias:
- `VRAM_ADDR_CMD: equ $40000000` (RAM de vídeo)
- `CRAM_ADDR_CMD: equ $C0000000` (RAM de color/paletas)
- `VSRAM_ADDR_CMD: equ $40000010` (RAM de desplazamiento vertical)

### Configuración de dirección fija

Para direcciones conocidas en tiempo de ensamblaje, la fórmula de cálculo es:

`((address AND $3FFF) << 16) OR ((address AND $C000) >> 14) OR constant`

Macro genérica proporcionada:

```
SetXramAddr: macro addr, cmd
    move.l  #(((addr)&$3FFF)<<16))|(((addr)&$C000)>>14)|(cmd), (VdpCtrl)
    endm
```

Macros especializadas derivadas:
- `SetVramAddr addr`
- `SetCramAddr addr`
- `SetVsramAddr addr`

### Configuración de dirección variable

Para direcciones dinámicas almacenadas en registros de datos (d0-d7), se requiere manipulación de bits:

```
SetXramAddrReg: macro reg, cmd
    and.l   #$FFFF, reg
    lsl.l   #2, reg
    lsr.w   #2, reg
    swap    reg
    or.l    #cmd, reg
    move.l  reg, (VdpCtrl)
    endm
```

### Escritura de datos

"write everything to `VdpData` (do _not_ increment the address, i.e. always write to the same location)"

Requisitos críticos:
- Escribir solo palabras (words) o palabras largas (longwords).
- Prohibido escribir bytes.
- No incrementar dirección entre escrituras (el VDP autoincrementa según el registro de autoincremento configurado).

Ejemplo de rutina completa:

```
CopyToVram:
    SetVramAddrReg  d0
    subq.w  #1, d1
    blo.s   @End
    lea     (VdpData), a1
@Loop:
    move.w  (a0)+, (a1)
    dbf     d1, @Loop
@End:
    rts
```

Parámetros: d0.w (dirección VRAM), d1.w (número de palabras), a0.l (puntero a datos).

Optimizaciones mencionadas: una escritura de longword equivale a dos palabras consecutivas; se puede desenrollar el bucle si el tamaño es un múltiplo conocido.

## Notas / gotchas

- No se debe escribir en bytes individuales al puerto de datos, solo en words o longwords.
- No incrementar manualmente la dirección entre escrituras a `VdpData`: el propio VDP se encarga del autoincremento configurado previamente en `VDPREG_INCR`.
- Usar `move.l` (longword) permite escribir dos palabras por instrucción, siendo una optimización habitual frente a escrituras de `move.w` sueltas.

## Referencias cruzadas

- (pendiente de enlazar)

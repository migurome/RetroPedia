---
titulo: Resoluciones de pantalla del VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, resolucion, h32, h40, interlace, pal, ntsc]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/screen-resolution]
---

# Resoluciones de pantalla del VDP

## Resumen

Detalla los registros del VDP que controlan la resolución horizontal y vertical de la pantalla (H32/H40, V28/V30, modo interlazado 448px) y los valores hexadecimales/binarios necesarios para configurar cada combinación de modo.

## Detalle técnico

### Registros VDP involucrados

**VDP Register `$8Cxx` (VDPREG_MODE4):** Controla la resolución de pantalla y efectos de sombra/resalte.

**VDP Register `$81xx` (VDPREG_MODE2):** Controla la altura vertical (solo bits específicos).

### Estructura de bits en VDPREG_MODE4

Según la documentación, la composición es:

- **Bits 7 y 0:** Resolución horizontal
  - Ambos 0 = 256px (H32)
  - Ambos 1 = 320px (H40)

- **Bits 2 y 1:** Resolución vertical
  - Ambos 0 = 224px (8×8 tiles)
  - Ambos 1 = 448px (8×16 tiles, modo interlazado)

- **Bit 3:** Shadow/highlight
  - 0 = deshabilitado
  - 1 = habilitado

### Valores hexadecimales por modo

```
GFXMODE_256x224:      equ %00000000
GFXMODE_320x224:      equ %10000001
GFXMODE_256x448:      equ %00000110
GFXMODE_320x448:      equ %10000111
GFXMODE_256x224_SH:   equ %00001000
GFXMODE_320x224_SH:   equ %10001001
GFXMODE_256x448_SH:   equ %00001110
GFXMODE_320x448_SH:   equ %10001111
```

### Código de implementación

`move.w  #VDPREG_MODE4|GFXMODE_320x224, (VdpCtrl)` ejemplifica la escritura directa.

Macro recomendado:

```
SetGfxMode macro mode
    move.w  #VDPREG_MODE4|(mode), (VdpCtrl)
    endm
```

### Modo interlazado (448px)

- Los tiles se duplican verticalmente: 8×16 píxeles.
- El refresco se hace en dos pasadas: líneas pares, luego impares.
- Requiere dos interrupciones de blanking vertical.
- Las interrupciones de horizontal blank mantienen la resolución baja.
- Para efectos raster en alta resolución, verificar el bit 4 de `$C00004`: 0=líneas pares, 1=impares.

### Resoluciones PAL/NTSC (240px/480px)

En VDPREG_MODE2, bit 3:

- **0:** 224px/448px (V28)
- **1:** 240px/480px (V30)

**Advertencia crítica del autor:** "only works properly on PAL systems" (solo funciona correctamente en sistemas PAL); en NTSC debe ser 0 u ocurren fallos en consola.

### Nomenclatura de tiles

- **H32:** 32 tiles horizontales (256px)
- **H40:** 40 tiles horizontales (320px)
- **V28:** 28 tiles verticales (224px)
- **V30:** 30 tiles verticales (240px)

## Notas / gotchas

- Activar V30 (240px/480px) en un sistema NTSC provoca fallos de consola: esta configuración solo es segura en sistemas PAL.
- El modo interlazado (448px) requiere lógica adicional: dos pasadas de refresco (líneas pares/impares) y dos interrupciones de vblank.
- Para raster effects en el modo interlazado de alta resolución, hay que comprobar el bit 4 del puerto `$C00004` para saber si la pasada actual es de líneas pares o impares.

## Referencias cruzadas

- (pendiente de enlazar)

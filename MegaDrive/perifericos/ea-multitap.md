---
titulo: EA Multitap (4 Way Play)
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [multitap, ea, 4-way-play, io, multijugador]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/ea-multitap]
---

# EA Multitap (4 Way Play)

## Resumen

El EA Multitap ("4 Way Play") es un adaptador multijugador de Electronic Arts para Mega Drive. A diferencia del multitap de Sega, solo soporta mandos y ocupa ambos puertos de I/O simultáneamente.

## Detalle técnico

### Configuración de puertos

Los puertos I/O deben configurarse así:
- `IoCtrl1`: `$40`
- `IoCtrl2`: `$7F`
- `IoData1`: `$40`

### Valores de selección de controlador

```
EAMULTI_1P:   equ $0C  ; Controller #1
EAMULTI_2P:   equ $1C  ; Controller #2
EAMULTI_3P:   equ $2C  ; Controller #3
EAMULTI_4P:   equ $3C  ; Controller #4
EAMULTI_ID:   equ $7C  ; Multitap ID
```

### Detección del multitap

El dispositivo se detecta comparando lecturas de los bits 1-0: deben ser `00` en la lectura de ID pero diferentes en la lectura del controlador #1.

### Protocolo de lectura

El multitap funciona como un multiplexor donde TH (bit 6) es compartido. La secuencia requiere:

1. Escribir `$40` o `$00` en `IoData1`.
2. Escribir los selectores (`EAMULTI_1P` a `EAMULTI_4P`) en `IoData2`.
3. Leer desde `IoData1` tras esperas con `NOP`.

Se recomienda realizar múltiples ciclos de lectura: 2 para mandos de 3 botones, 7 para soportar mandos de 6 botones.

### Cabecera de la ROM

Agregar `"4"` en el campo de dispositivos (offset `$000190`) para indicar soporte de multitap.

## Notas / gotchas

- El EA Multitap ocupa ambos puertos de I/O a la vez, a diferencia del Sega Multitap que solo usa uno.
- Solo soporta mandos, no ratones ni otros periféricos.

## Referencias cruzadas

- (pendiente de enlazar)

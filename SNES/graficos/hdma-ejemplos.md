---
titulo: HDMA — ejemplos prácticos
plataforma: [SNES]
categoria: [graficos]
tags: [hdma, dma, ppu, registros, timing, tablas]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/HDMA_examples]
---

# HDMA — ejemplos prácticos

## Resumen

Recopilación de patrones de uso real del HDMA (Horizontal-Blanking DMA) de SNES: los cinco formatos de transferencia (DMAP), el formato de las tablas (modo directo e indirecto), las restricciones de timing que hay que respetar, y ejemplos concretos como degradados de paleta por CGRAM, ondas senoidales de scroll, y HDMA con doble buffer para animaciones dinámicas.

## Detalle técnico

### Patrones de transferencia (valores de DMAP)

**Patrón 0 — un registro**: transfiere un único byte a la dirección de bus B (+0). No sirve para registros de escritura doble o lectura doble.
Registros a los que aplica: BGMODE, MOSAIC, BG1SC-BG4SC, BG12NBA, BG34NBA, M7SEL, CGADD, W12SEL, W34SEL, WOBJSEL, WBGLOG, WOBJLOG, TM, TS, TMW, TSW, CGWSEL, CGADSUB, SETINI, COLDATA.

**Patrón 1 — dos registros**: transfiere dos bytes a direcciones de bus B adyacentes (+0, +1). Tampoco compatible con registros de escritura/lectura doble.
Ejemplos: posiciones de ventana (WH0/WH1, WH2/WH3), activación de capas (TM/TS, TMW/TSW), ajustes de color math.

**Patrón 2 — un registro, escritura doble**: transfiere dos bytes al mismo registro de escritura/lectura doble (+0, +0).
Usos: BGnHOFS, BGnVOFS, la matriz de modo 7 (M7A-M7Y), CGDATA, COLDATA.

**Patrón 3 — dos registros, escritura doble**: transfiere cuatro bytes a dos registros adyacentes de escritura/lectura doble (+0, +0, +1, +1).
Aplicaciones: offsets de scroll de fondo, pares adyacentes de la matriz de modo 7, la combinación CGADD/CGDATA (nota: el primer byte se ignora, el segundo contiene el índice de paleta, y los bytes 3-4 contienen el dato de color).

**Patrón 4 — cuatro registros**: transfiere cuatro bytes a cuatro direcciones (+0, +1, +2, +3). Requiere registros que no sean de escritura doble.
Caso de uso: las cuatro posiciones de ventana a la vez (WH0/WH1/WH2/WH3).

### Restricciones de timing

Restricciones críticas a respetar:

- El HDMA debe permanecer desactivado durante Force-Blank, para no corromper la memoria de la PPU ni contaminar los latches compartidos entre varios registros de escritura doble.
- Las escrituras a **HDMAEN** solo deben hacerse durante VBlank con la pantalla activa.
- Los registros de estado interno del HDMA (A2An, NLTRn, DASn) se inicializan en el scanline 0.
- Activar el HDMA después del scanline 0 provoca lecturas de datos de frame corruptas/glitcheadas.
- Los registros de un canal HDMA activo solo se pueden escribir durante VBlank.
- Las tablas de HDMA deben quedar configuradas antes del scanline 0.
- Las tablas no se pueden modificar mientras el HDMA está activo procesándolas; modificarlas es seguro durante VBlank.
- Las tablas dinámicas grandes necesitan doble buffer para evitar tearing.

### Formato de las entradas de tabla HDMA

El byte de "contador de línea" de cada entrada se interpreta así:

- Valor `0x00`: marca de fin de tabla; el canal se desactiva hasta el siguiente frame.
- Valores `0x01`-`0x80`: entrada sin repetición; escribe una vez y espera el número de scanlines indicado.
- Valores `0x81`-`0xFF`: entrada con repetición; escribe en cada scanline durante (valor - `0x80`) scanlines.

#### Entrada sin repetición

Formato: 1 byte de contador de línea + datos de transferencia (tamaño según el patrón DMAP usado).

Ejemplo (patrón 0, un registro):

```
db  32              // 32 scanlines
db  0x13            // Valor de registro
db  64              // 64 scanlines
db  0x04
db  1               // 1 scanline
db  0x13
db  0               // Marca de fin
```

Máximo 128 scanlines por entrada; secuencias más largas se reparten en varias entradas con el mismo dato.

Ejemplo (patrón 2, escritura doble):

```
db  127             // 127 scanlines
dw  0               // valor de 16 bits
db  33              // 33 scanlines
dw  0               // (127+33=160 en total)
```

#### Entrada con repetición

Formato: 1 byte de contador de línea (`0x80 | cuenta`) + datos repetidos "cuenta" veces.

Ejemplo (patrón 2):

```
db  0x80 | 127      // 127 scanlines, repetición
dw  144, 145, 146, ...  // 127 valores de palabra
db  0x80 | 97       // 97 scanlines
dw  271, 272, ...   // 97 valores de palabra
db  0               // Marca de fin
```

Máximo 127 scanlines por entrada con repetición.

#### Modo HDMA indirecto

Todas las entradas ocupan 3 bytes: 1 byte de contador de línea + 2 bytes de dirección de palabra (combinada con el registro de banco DASBn).

Configuración:

```
DMAP7 = 0x42                    // Bit 6 activa modo indirecto
BBAD7 = .lobyte(BG1HOFS)
A1T7  = .loword(IndirectTable)
B1B7  = .bankbyte(IndirectTable)
DASB7 = .bankbyte(DataArray)
HDMAEN = 1 << 7
```

Ejemplo de tabla (224 scanlines mapeados a un array contiguo):

```
db  0x80 | 112
dw  ContiguousArray
db  0x80 | 112
dw  ContiguousArray + 112*2
db  0
```

### HDMA dinámico

#### Modificación durante VBlank

Editar la tabla de forma segura durante el periodo en que el HDMA está desactivado (VBlank). Adecuado para ajustar la altura de un efecto scanline a scanline.

Ejemplo — animación de "cortina" (wipe) de un cuadro de texto:

```
subroutine VBlank_TextboxWipeAnimation:
    hdmaBuffer[2] = textboxHeight
    HDMAEN = 1 << 7
```

Las variables controlan la altura visible (máximo 128 scanlines).

#### Doble buffer en modo directo

Se alterna entre dos buffers de RAM: uno lo usa el controlador de HDMA mientras el bucle principal escribe en el otro.

Configuración clave:

```
DMAP7 = 2
BBAD7 = .lobyte(BG1HOFS)
B1B7  = .bankbyte(hdmaBufferA)
// A1T7 se escribe en VBlank según el flag activeHdmaBuffer
```

Intercambio de buffers en VBlank:

```
if activeHdmaBuffer == 0:
    A1T7 = .loword(hdmaBufferB)
else:
    A1T7 = .loword(hdmaBufferA)
HDMAEN = 1 << 7
```

#### Doble buffer en modo indirecto

Una tabla HDMA fija en ROM referencia dos arrays de scanlines en RAM.

Configuración:

```
DMAP7 = 0x42
BBAD7 = .lobyte(BG1HOFS)
B1B7  = .bankbyte(HdmaTable_A)
DASB7 = .bankbyte(scanlineArray_A)
```

Bucle de animación de "shear" (cizalla):

```
for i = 0 to N_SCANLINES-1:
    tmp = tmp - 2
    scanlineArray[i] = clamp(tmp, 0, 256)
```

### Escritura de CGRAM por HDMA

#### Método de dos canales

El canal 6 fija el índice de CGRAM vía CGADD (patrón 0); el canal 7 escribe el dato de color vía CGDATA (patrón 2).

Configuración:

```
DMAP6 = 0
BBAD6 = .lobyte(CGADD)
A1T6  = .loword(CGADD_HdmaTable)
DMAP7 = 2
BBAD7 = .lobyte(CGDATA)
A1T7  = .loword(CGDATA_HdmaTable)
HDMAEN = (1 << 6) | (1 << 7)
```

Las entradas de ambas tablas deben tener los mismos conteos de línea para quedar sincronizadas:

```
CGADD_HdmaTable:
    db  32
    db  COLOR_TO_CHANGE
    .repeat 28, i
        db  2
        db  COLOR_TO_CHANGE
    .endrepeat
    db  0

CGDATA_HdmaTable:
    db  32
    dw  0
    .repeat 28, i
        db  2
        dw  (28-i) << 10
    .endrepeat
    db  0
```

#### Método de un solo canal (patrón 3)

Usa el par CGADD/CGDATA en modo de escritura doble emparejada. Cada transferencia de 4 bytes es: byte dummy + byte de índice + 2 bytes de valor de color.

Configuración:

```
DMAP7 = 3
BBAD7 = .lobyte(CGADD)
HDMAEN = 1 << 7
```

Formato de tabla:

```
HdmaTable:
    db  32
    db  0, COLOR_TO_CHANGE
    dw  0
    .repeat 28, i
        db  2
        db  0, COLOR_TO_CHANGE
        dw  (28-i) << 10
    .endrepeat
    db  0
```

### Ejemplo: registro TM sin repetición

```
DMAP7 = 0
BBAD7 = .lobyte(TM)
A1T7  = .loword(HdmaTable)
B1B7  = .bankbyte(HdmaTable)
HDMAEN = 1 << 7

HdmaTable:
    db  32
    db  0x13
    db  64
    db  0x04
    db  1
    db  0x13
    db  0
```

### Ejemplo: BG1HOFS con repetición (onda senoidal)

```
DMAP7 = 0x42
BBAD7 = .lobyte(BG1HOFS)
A1T7  = .loword(IndirectHdmaTable)
DASB7 = .bankbyte(SineTable)

SineTable:
    dw  0, 2, 4, 6, 8, 10, 12, 13, 14, 15, 16, 16, 16, 16, 16, 15, 14, 13, 12, 10, 8, 6, 4, 2, 0, -2, -4, -6, -8, -10, -12, -13, -14, -15, -16, -16, -16, -16, -16, -15, -14, -13, -12, -10, -8, -6, -4, -2

IndirectHdmaTable:
    db  0x80 | 48
    dw  SineTable
    db  0x80 | 48
    dw  SineTable
    db  0x80 | 48
    dw  SineTable
    db  0x80 | 48
    dw  SineTable
    db  0x80 | 48
    dw  SineTable
    db  0
```

Este ejemplo aplica la misma tabla senoidal de 48 valores cinco veces seguidas (reutilizando la tabla en modo indirecto), para conseguir un scroll horizontal ondulado repetitivo a lo largo de toda la pantalla sin necesitar una tabla de 240 valores completa.

### Recursos relacionados citados en la página

La página original enlaza además a la guía de HDMA de nesdoug y a la guía de HDMA de Grog en superfamicom.org como lecturas complementarias externas.

## Notas / gotchas

- HDMAEN solo se debe escribir durante VBlank con pantalla activa; activarlo en Force-Blank o fuera de tiempo corrompe memoria de la PPU y contamina latches compartidos.
- Las tablas deben estar completamente listas antes del scanline 0 del frame en que se van a usar; activarlas después provoca datos corruptos ese frame.
- Máximo 128 scanlines por entrada sin repetición y 127 por entrada con repetición — hay que partir tramos más largos en varias entradas.
- Los patrones 0 y 1 (uno o dos registros) no sirven para registros de escritura doble (BGnHOFS, M7A-M7Y, CGDATA, COLDATA); para esos hace falta patrón 2 o 3.
- Las tablas dinámicas que se reconstruyen cada frame necesitan doble buffer (directo o indirecto) para evitar tearing/glitches visibles.
- El modo indirecto permite compartir una misma tabla de "instrucciones" de HDMA en ROM contra distintos arrays de datos en RAM, útil para animaciones (shear, wipes) sin tener que regenerar la tabla completa cada vez.

## Referencias cruzadas

- Ver `vblank-routine.md` (por qué y cuándo se actualizan las tablas/registros de HDMA dentro de la rutina de VBlank).
- Ver `formas-de-ventana.md` (aplicación concreta de HDMA para animar las posiciones de ventana WH0-WH3).
- Ver `modo-7-efectos-perspectiva.md` (uso de HDMA para cambiar la matriz de modo 7 scanline a scanline).

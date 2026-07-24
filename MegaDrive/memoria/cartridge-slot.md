---
titulo: Conector de cartucho (pinout)
plataforma: [MegaDrive]
categoria: [memoria]
tags: [pinout, cartucho, cart-slot, m3, dtack, senales]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/cartridge-slot]
---

# Conector de cartucho (pinout)

## Resumen

Referencia técnica completa del conector de cartucho de la Mega Drive: distribución de pines, señales en modo Mega Drive y en modo Master System, decodificación de direcciones, señales de vídeo/audio y de reset.

## Detalle técnico

### Visión general de los pines del cartucho

El conector de cartucho de Mega Drive contiene 32 pines repartidos en dos caras (A y B). El pinout incluye líneas de bus de direcciones (VA23-VA1), líneas de bus de datos (VD15-VD0), señales de control, salidas de vídeo y entradas de audio.

### Señales en modo Mega Drive

Señales clave para el funcionamiento estándar del cartucho:

- **`/M3`**: "Dejar flotante para modo Mega Drive"
- **`/CART`**: "Poner a nivel bajo para arrancar desde el cartucho"
- **Bus de direcciones**: VA23-VA1 proporciona el direccionamiento completo del 68000
- **Bus de datos**: VD15-VD0 para transferencias de tamaño palabra
- **`/CE_0`**: chip enable para `$000000-$3FFFFF`
- **`/TIME`**: chip enable para `$A13000-$A130FF`
- **`/UWR`/`/LWR`**: write enable para byte alto y bajo respectivamente
- **`/CAS0`**: output enable para `$000000-$DFFFFF`
- **`/CAS2`**: column address strobe para DRAM
- **`/ASEL`**: control de multiplexado de fila/columna de dirección
- **`/DTACK`**: acuse de recibo de datos del 68000
- **`/AS`**: address strobe del 68000
- **`VCLK`**: reloj del 68000
- **`EDCLK`**: reloj de vídeo H40
- **`/VSYNC`, `/HSYNC`**: sincronización de vídeo
- **`/YS`**: "activada al mostrar el color de fondo"
- **`SL1`, `SR1`**: líneas de entrada de audio analógico
- **`/MRES`, `/VRES`**: señales de reset duro y suave

### Señales en modo Master System

Cuando `/M3` se pone a nivel bajo, los pines del cartucho funcionan de forma diferente:

- **Bus de datos**: VD7-VD0 (8 bits, no 16 bits)
- **Bus de direcciones**: VA16-VA1
- **VA17**: chip enable de tarjeta (card)
- **VA18**: chip enable de cartucho
- **VA19**: `/IOREQ` de la Master System
- **VA21**: "poner a nivel bajo cuando se ejecuta desde el slot de tarjeta"
- **VA22**: "??? (el PBC lo pone a nivel bajo)"
- **VA23**: botón de pausa (lógica baja)
- **`/CE_0`**: chip enable para `$0000-$7FFF`
- **`/CAS2`**: chip enable para `$8000-$BFFF`
- **`/VRES`**: "siempre a nivel bajo (¡ignorarlo!!)"

Las señales de vídeo y audio funcionan igual que en modo Mega Drive.

### Decodificación de direcciones estándar

Para una implementación de cartucho típica:

1. Poner `/CART` a nivel bajo (conectado a tierra)
2. Dejar `/M3` flotante (sin conectar)

"Al hacer una lectura, el 68000 siempre espera una palabra y elige los bytes que quiere." Para escrituras, "`/UWR` se activa para bytes pares, `/LWR` para bytes impares, y ambos para accesos de palabra."

### Decodificación de direcciones avanzada

La consola "_no_ activa `/DTACK` para los rangos `$800000-$9FFFFF`, `$A15000-$A15FFF` y `$B00000-$BFFFFF`". Los cartuchos que usen estos rangos deben poner `/DTACK` a nivel bajo de forma independiente.

Rangos de memoria reservados que no deben sobrescribirse:

- **`$400000-$7FFFFF`**: memoria de Mega CD
- **`$A12000-$A12FFF`**: puertos de Mega CD
- **`$800000-$9FFFFF`**: memoria de 32X
- **`$A15000-$A15FFF`**: puertos de 32X y SVP

Rangos de compatibilidad con Teradrive:

- **`$AE0000-$AEFFFF`**: registros de Teradrive
- **`$AF0000-$AFFFFF`**: ventana de puerto PC de Teradrive
- **`$B00000-$BFFFFF`**: ventana de memoria PC de Teradrive

### La señal `/CART`

"La Mega Drive puede arrancar desde el cartucho o desde el slot de expansión" mediante esta señal. El mapeo de direcciones cambia según el estado de `/CART`:

- **`/CART` a nivel bajo**: cartucho en `$000000-$3FFFFF`; expansión en `$400000-$7FFFFF`
- **`/CART` a nivel alto**: cartucho en `$400000-$7FFFFF`; expansión en `$000000-$3FFFFF`

"La señal `/CE_0` se activará siempre para el rango que corresponda al slot de cartucho."

### La señal `/M3`

"Los juegos de Mega Drive deben mantener esta línea flotante, mientras que los juegos de Master System deben ponerla a nivel bajo."

### Señales de vídeo

Cuatro señales proporcionan datos de vídeo:

- **`EDCLK`**: "Un píxel equivale a dos ciclos de `EDCLK`" y representa el "reloj del modo H40, _incluso si el VDP está en ese momento generando H32_"
- **`/VSYNC`, `/HSYNC`**: pulsos de sincronización
- **`/YS`**: "se pone a nivel bajo siempre que el VDP está generando el color de fondo"

### Entrada de audio

"El cartucho puede alimentar audio a la consola a través de las líneas `SL1` y `SR1`" para los canales izquierdo y derecho. El 32X produce "±2V (con eliminación de bias)" con una impedancia de aproximadamente "16 ohmios".

### Señales de reset

"`/MRES`" se activa al encender la consola; "`/VRES`" se activa al encender y al pulsar el botón de Reset. Ambas son "open drain y están conectadas directamente al bus". Advertencia crítica: "**Nunca ponerlas a nivel alto cuando no se están activando.**" `/MRES` resetea toda la consola; `/VRES` resetea solo las CPUs y el YM2612. En modo Master System, "`/VRES` es completamente inútil... ya que se mantiene constantemente a nivel bajo".

## Notas / gotchas

- Nunca poner `/MRES` ni `/VRES` a nivel alto cuando no se están activando activamente (son open drain, conectadas directamente al bus).
- La consola no activa `/DTACK` en los rangos `$800000-$9FFFFF`, `$A15000-$A15FFF` y `$B00000-$BFFFFF`; los cartuchos que usen esos rangos deben generar su propio `/DTACK`.
- No sobrescribir los rangos reservados para Mega CD, 32X/SVP ni Teradrive.
- `/VRES` no sirve de nada en modo Master System porque se mantiene siempre a nivel bajo.

## Referencias cruzadas

- (pendiente de enlazar)

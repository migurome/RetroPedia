---
titulo: Turbo File Twin (ASCII Corporation)
plataforma: [SNES]
categoria: [perifericos]
tags: [turbo-file-twin, guardado, memoria, io, protocolo]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Turbo_File_Twin]
---

# Turbo File Twin (ASCII Corporation)

## Resumen

El Turbo File Twin es un periférico de almacenamiento externo fabricado por ASCII Corporation que se conecta al puerto de mando de SNES. Permite a los juegos guardar y recuperar datos de partida en una memoria respaldada por dos pilas AA reemplazables, e incluye dos modos de funcionamiento seleccionables por un interruptor físico: uno de compatibilidad con el Turbo File II de Famicom y otro con más capacidad propio de SNES.

## Detalle técnico

### Modos de operación

- **Modo TFII**: emula un Turbo File II original mediante un adaptador, ofreciendo 4 bancos de 8 KiB cada uno. Da compatibilidad hacia atrás con archivos guardados en Famicom.
- **Modo STF**: da acceso a 128 KiB de espacio de guardado independiente, con acceso arbitrario (seek) en lugar de la lectura estrictamente secuencial del modo TFII.

Un interruptor físico selecciona el modo activo, y existe además protección adicional contra escritura.

### Protocolo en modo TFII

**Lectura de estado** (a través de la línea D0):
- Bits 0-7: valor `$FF`, indica que el dispositivo está en modo TFII.
- Bits 8-11: ID de controlador `$E`.
- Bit 23: 1 si el dispositivo está desbloqueado, 0 si está bloqueado.
- D1: valor del bit actual apuntado en la memoria.

**Desbloqueo**: poner OUT0 a 1, generar pulso 15 veces, poner OUT0 a 0, generar pulso 1 vez.

**Bloqueo**: poner OUT0 a 1, generar pulso 1 vez, poner OUT0 a 0, generar pulso 1 vez.

**Acceso a datos**: una transición de OUT0 de 1 a 0 escribe el bit de IO en la posición de memoria actual y avanza el puntero.

### Protocolo en modo STF

**Lectura de estado** (32 bits vía D0):
- Bits 0-3: capacidad (`$0` = 128 KiB).
- Bit 4: protección de escritura activa.
- Bit 5: pilas bajas.
- Bit 6: modo lectura activo.
- Bit 7: modo escritura, sin bytes escritos todavía.
- Bits 8-15: `$FE`, identificador de modo STF.
- Bits 16-31: ID de controlador `$E`.

**Comando inicial** (28 bits, LSB primero):
- Bits 0-15: dirección de byte objetivo.
- Bits 16-27: código de comando (`0x24` = lectura, `0x75` = escritura).

**Secuencia de lectura**: OUT0 de 1 a 0, generar pulso 8 veces (D1 devuelve el inverso del bit leído). Para salir de la secuencia: mantener OUT0 en alto durante un pulso.

**Secuencia de escritura**: OUT0 en alto, generar pulso 8 veces con los bits IO deseados, luego OUT0 a bajo. Para salir: transición de OUT0 de 1 a 0 sin generar pulso adicional.

### Formato de datos en modo STF

**Tabla FAT** (primeros 4 KiB del área de almacenamiento):
- `$00000`-`$001EF`: 124 entradas de la FAT.
- `$001F0`-`$00FFF`: entradas sin usar, más el identificador "FAT0" ubicado en `$00FFC`-`$00FFF`.
- `$01000` en adelante: datos organizados en bloques de 1 KiB.

**Estructura de cada entrada FAT** (32 bits, little-endian):
- Bits 0-11: tamaño en bloques (si es el primer bloque del archivo) o bien `$000`/`$FFF`.
- Bits 12-23: siguiente bloque encadenado (`$NNN`; `$FFF` si es el último bloque o está libre).
- Bits 24-31: checksum de 8 bits (suma de los 1024 bytes del bloque), o `$FF` si el bloque está libre.

Ejemplo de entrada: los bytes `$08 $10 $00 $6A` forman el valor `$6A001008`, que se interpreta como tamaño `$008` bloques, siguiente bloque `$001`, checksum `$6A`.

**Metadatos de archivo** (primeros 24 bytes del primer bloque del archivo):
- `$0000`-`$0003`: identificador "SHVC".
- `$0004`-`$0007`: código de juego tomado del header extendido de cartucho.
- `$0008`-`$0017`: nombre de archivo (relleno con espacios).
- `$0018` en adelante: resto de los datos del bloque.

### Rutinas de ejemplo (65816, acumulador de 8 bits)

Se documentan dos rutinas en ensamblador:

- **ReadTFIIByte**: itera la lectura de 8 bits usando la dirección `$4017` (leyendo la línea D1), almacenando el resultado byte a byte.
- **WriteTFIIByte**: itera la escritura de 8 bits usando `$4016` (OUT0) y `$4201` (WRIO/IO).

## Notas / gotchas

- Algunos juegos comprueban únicamente los 4 bits superiores para detectar el valor `$FFF` (fin de cadena/bloque libre), en lugar de los 12 bits completos.
- Se recomienda que los juegos mantengan el dispositivo bloqueado salvo cuando estén accediendo activamente a los datos, para evitar corrupciones accidentales.
- Debe desactivarse la lectura automática de mando (registro `$4200`) mientras se realiza acceso al Turbo File Twin, ya que ambos mecanismos compiten por las mismas líneas del puerto.
- El primer acceso tras iniciar una secuencia de lectura o escritura en modo STF se trata de forma especial: no avanza la posición ni accede realmente a memoria, es una especie de ciclo de sincronización inicial.
- Existe un formato heredado con los bits invertidos, procedente de *Castle Excellent*: algunos juegos (como *Wizardry V*) deben detectar este formato heredado y manejarlo de forma diferenciada.

## Referencias cruzadas

- (pendiente de enlazar)

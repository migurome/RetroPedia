---
titulo: Rutina de VBlank (patrones prácticos)
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, vblank, hdma, oam, dma, joypad, buffers]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/VBlank_routine]
---

# Rutina de VBlank (patrones prácticos)

## Resumen

La rutina de VBlank es el código que realmente se ejecuta durante el periodo de borrado vertical y que aplica a la PPU todos los cambios preparados por el resto del juego. Esta entrada recoge los patrones prácticos habituales para organizarla: variables sombra, buffers, colas, gestión de HDMA y lectura de mandos, todos pensados para aprovechar al máximo el poco tiempo disponible en VBlank.

## Detalle técnico

### Dónde vive la rutina de VBlank

Puede ejecutarse dentro de la propia rutina de servicio de interrupción (ISR) de la NMI, dentro del bucle principal (que se queda esperando a que empiece el VBlank), o incluso dentro de una rutina de IRQ cuando se necesita alargar artificialmente el VBlank. Es habitual que un mismo juego tenga varias rutinas de VBlank distintas según el modo de juego en el que se esté (menú, gameplay, transición, etc.), cada una con su propio conjunto de tareas.

### Variables sombra (shadow variables)

Una variable sombra es una copia en RAM del valor que se quiere que tenga un registro de la PPU. El resto del código lee y escribe libremente esa copia en cualquier momento del frame (sin restricciones de timing), y solo la rutina de VBlank se encarga de volcar el valor final al registro real de la PPU.

Ventajas de este patrón:

- Permite "leer" el valor actual de registros que en realidad son de solo escritura (ya que se lee la copia en RAM, no el registro).
- Permite tratar como una sola variable de 16 bits registros que en hardware son "de escritura doble" (word), simplificando el código que los manipula.
- Evita recalcular el valor a escribir en cada frame si no ha cambiado, ahorrando tiempo de CPU.

Por eficiencia de direccionamiento, conviene que las variables sombra vivan en Low-RAM (rangos `$00`-`$3f`, `$7e`, `$80`-`$bf`).

**Ejemplo con un registro de un solo byte y escritura simple (MOSAIC)**: el bucle principal modifica libremente la variable sombra `mosaicShadow`; la rutina de VBlank simplemente hace `lda mosaicShadow` / `sta MOSAIC`.

**Ejemplo con registros de escritura doble (BG1HOFS/BG1VOFS)**: el bucle principal trabaja con un registro de índice de 16 bits como si fuera una única variable; la rutina de VBlank se encarga de partirlo y escribirlo byte a byte, respetando el orden que exige el registro real (primero el byte bajo, luego el alto).

### Buffers

Un buffer de tamaño fijo es una zona de RAM que el bucle principal va rellenando con datos (de OAM, de CGRAM, de texto, de un tilemap, de una fuente de ancho variable, etc.), y que la rutina de VBlank transfiere de una vez a la PPU —normalmente por DMA— cuando una bandera de control indica que el buffer está listo. La regla de oro es que **el buffer no debe modificarse una vez que se ha activado la bandera de transferencia**, porque si el bucle principal sigue escribiendo en él mientras el DMA lo está leyendo (o justo antes de que la rutina de VBlank lo procese), el resultado queda corrupto.

**Ejemplo de buffer de OAM**: un buffer de 544 bytes que se transfiere completo por DMA usando el canal 0. Tras completarse la transferencia, se resetea la bandera correspondiente.

**Ejemplo de buffer de columna de tilemap**: un buffer de 64 bytes, con variables separadas para la dirección VRAM de destino y el tamaño a transferir. Antes de lanzar la transferencia, hay que dejar **VMAIN** configurado en modo de acceso por palabra (`$81`, incremento en modo columna de tilemap).

### Colas

Una cola acumula varias actualizaciones pendientes de PPU para que la rutina de VBlank las procese todas de una vez. El propio artículo avisa: "hay una cantidad limitada de tiempo de VBlank, y una cola demasiado grande puede provocar fácilmente un desbordamiento de VBlank" (VBlank overrun), es decir, que la rutina de VBlank no termine antes de que acabe el borrado vertical real.

**Ejemplo de cola de metatiles**: una cola tipo LIFO con capacidad para 4 entradas. Cada entrada contiene la dirección de palabra en VRAM de destino más las cuatro palabras de tilemap correspondientes a las cuatro esquinas del metatile (arriba-izquierda, arriba-derecha, abajo-izquierda, abajo-derecha). Un índice (`mtQueueIndex`) lleva la cuenta de la posición actual; se considera llena cuando `mtQueueIndex == 2 * QUEUE_SIZE`. La rutina de VBlank procesa todas las entradas en orden y luego resetea el índice.

### HDMA

El periodo de VBlank es el mejor momento para actualizar las tablas/registros de HDMA mientras la pantalla está activa. Hay una restricción de timing crítica: **el HDMA debe quedar configurado y activado antes de que empiece el siguiente frame**, porque pocos ciclos después de arrancar el scanline 0, el controlador de DMA ya toma una "foto" del estado de los canales HDMA para usarla durante ese frame. Otros puntos señalados: evitar compartir canales entre DMA normal y HDMA, y evitar que un desbordamiento de VBlank retrase la activación del HDMA (lo que dejaría el frame sin el efecto HDMA correspondiente).

### Lectura de mandos

El final de la rutina de VBlank es un buen momento para leer los mandos. Es importante recordar que **los registros JOY1-JOY4 no deben leerse mientras el auto-lectura de joypad sigue en curso**: hay que sondear el flag correspondiente en el registro **HVBJOY** (que indica si el auto-lectura sigue activo) antes de acceder a esos registros, para no leer datos a medio actualizar.

## Notas / gotchas

- Un buffer no debe tocarse después de activar su bandera de transferencia: el bucle principal tiene que esperar a que la rutina de VBlank lo procese antes de volver a escribir en él.
- Las colas de actualización de PPU deben dimensionarse con cuidado: si la cola es demasiado grande, procesarla entera puede hacer que la rutina de VBlank se salga del tiempo real de VBlank ("VBlank overrun").
- El HDMA debe quedar completamente configurado y con `HDMAEN` activado antes de que arranque el scanline 0 del siguiente frame; si la rutina de VBlank se retrasa (por un overrun), el HDMA de ese frame puede quedar mal aplicado o no aplicarse.
- Nunca compartir el mismo canal de DMA para transferencias normales y para HDMA a la vez.
- No leer JOY1-JOY4 mientras el auto-lectura de joypad sigue activo (comprobar el flag en HVBJOY primero); si se lee demasiado pronto, los datos pueden estar a medio actualizar.
- Las variables sombra deberían vivir en Low-RAM para que el direccionamiento sea eficiente.

## Referencias cruzadas

- Ver `vblank-interrupts.md` (mecanismo de la interrupción NMI que dispara/enmarca esta rutina de VBlank: NMITIMEN, bandera de sincronización, guardado de registros).
- Ver `hdma-ejemplos.md` (detalle de cómo se configuran los canales y tablas de HDMA mencionados aquí).
- Ver `leer-escribir-memoria-ppu.md` (detalle de VMAIN y los registros de acceso a VRAM/CGRAM/OAM usados por los buffers).

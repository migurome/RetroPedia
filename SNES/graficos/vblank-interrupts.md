---
titulo: Interrupciones VBlank (NMI)
plataforma: [SNES]
categoria: [graficos]
tags: [ppu, nmi, interrupciones, vblank, timing, registros]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/VBlank_interrupts]
---

# Interrupciones VBlank (NMI)

## Resumen

La mayor parte de la PPU de SNES solo puede tocarse de forma segura durante el periodo de borrado vertical (VBlank), mientras la pantalla está activa. La forma estándar de garantizar ese timing es apoyarse en la interrupción NMI que el hardware dispara al entrar en VBlank: el bucle principal prepara los datos, y la rutina de VBlank (ejecutada dentro de la NMI) los vuelca a la PPU en el momento correcto.

## Detalle técnico

### Idea central

El patrón habitual es: el bucle principal (main loop) va acumulando en RAM los cambios que hay que aplicar a la PPU (scroll, paletas, tiles, OAM, etc.), y es la rutina de VBlank —lanzada por la interrupción NMI— la que efectivamente escribe esos datos en los registros de la PPU, porque solo entonces el acceso es seguro.

### La interrupción es de tipo NMI

Las interrupciones de VBlank son NMI (Non-Maskable Interrupt): se disparan automáticamente al empezar el borrado vertical, siempre que el bit de habilitación de NMI por VBlank esté activo en el registro **NMITIMEN**. Cuando salta, la CPU ejecuta la rutina de servicio de interrupción (ISR) apuntada por el vector de interrupción NMI.

Pasos típicos que debe hacer esa ISR de NMI:

- Saltar a una dirección en banco FastROM si el proyecto usa FastROM.
- Guardar los registros de la CPU (A, X, Y en modo 16 bits; el registro de flags P se gestiona automáticamente por el propio mecanismo de interrupción).
- Reiniciar los registros DB (data bank) y DP (direct page) a un valor conocido, ya que el código de la ISR no puede asumir en qué banco/página estaba el código interrumpido.
- Invocar la rutina de VBlank propiamente dicha (la que hace el trabajo real de refresco de PPU).
- Restaurar los registros de la CPU guardados al principio.
- Salir con la instrucción `rti`.

### Patrón de sincronización con una bandera

Para evitar que la rutina de VBlank se ejecute en momentos en que la PPU está siendo configurada manualmente desde el bucle principal (por ejemplo, durante la carga inicial de un nivel, cuando se están tocando a mano direcciones de CGRAM/OAM/VRAM), se recomienda usar una bandera de un byte que indique si la rutina de VBlank debe ejecutarse o no en esa pasada. Así el bucle principal puede "desactivar" temporalmente el refresco automático mientras hace una configuración delicada, evitando que la NMI corrompa esas escrituras a medio hacer.

### Registros y variables involucrados

- **NMITIMEN**: registro que controla, entre otras cosas, el bit de habilitación de la NMI por VBlank (y también el auto-lectura de mandos).
- **RDNMI**: registro de flag de VBlank; leerlo forma parte de la inicialización/reconocimiento de la interrupción.
- **STAT78**: contiene el bit que indica si la consola está en modo PAL o NTSC, útil para calcular tiempos reales a partir del contador de frames.
- **WMADD, WMDATA**: registros de acceso a Work-RAM vía "bus B"; el texto insiste en que el bucle principal debería tener acceso exclusivo a ellos (junto con los registros de multiplicación y división) para evitar que una NMI que salte a mitad de una operación deje esos registros en un estado inconsistente.
- **DB, DP**: registros de la CPU 65C816 que la ISR de NMI debe resetear explícitamente al entrar.

### Uso como contador de tiempo

Como la NMI de VBlank se dispara una vez por frame, es habitual usarla para llevar un contador de frames (`frameCounter`, incrementado en cada NMI). Dividiendo ese contador entre 50 (consolas PAL) o 60 (consolas NTSC) se obtiene el tiempo transcurrido en segundos de forma sencilla.

### Advertencia sobre recursos compartidos

Es muy recomendable que el bucle principal mantenga acceso exclusivo a los registros de Work-RAM (WMADD/WMDATA), a los registros de multiplicación y a los de división, precisamente porque estos registros requieren varios pasos consecutivos (escribir operandos, esperar, leer resultado) y una NMI que interrumpa esa secuencia a mitad de camino puede dejarlos en un estado corrupto o devolver un resultado incorrecto.

### Ejemplo de implementación (variables y rutinas)

Variables típicas usadas en el patrón de ejemplo:

- `vBlankFlag` (u8): bandera de control que indica si toca ejecutar la rutina de VBlank.
- `frameCounter` (u32): contador de interrupciones NMI recibidas (para medir tiempo).
- `lagCounter` (u16): cuenta los "frames de lag" (vueltas de NMI en las que el juego no llegó a tiempo, ver más abajo).

**EnableVBlankInterrupts**: rutina de inicialización que limpia `vBlankFlag`, lee `RDNMI` (para limpiar cualquier flag de VBlank pendiente) y escribe en `NMITIMEN` el valor `$81` (activa a la vez la NMI de VBlank y el auto-lectura de joypad).

**WaitForVBlank**: pone `vBlankFlag` a 1 y entra en un bucle que ejecuta `wai` (Wait for Interrupt) hasta que la bandera se limpie —es decir, hasta que la siguiente NMI la procese—.

**NmiIsr** (rutina de servicio de la interrupción NMI):

1. Salta al banco FastROM correspondiente (banco `$80` o `$00`, según el mapeo del proyecto).
2. Guarda A, X, Y, y los registros B (data bank) y D (direct page) en modo 16 bits.
3. Resetea el data bank register (DB) usando la secuencia `phk` / `plb`.
4. Pone el direct page (DP) a 0.
5. Comprueba `vBlankFlag`: si ya estaba a cero (es decir, el bucle principal no llegó a pedir la espera antes de que llegara esta NMI), incrementa `lagCounter` para detectar frames de lag.
6. Limpia `vBlankFlag` y ejecuta la rutina de VBlank real.
7. Incrementa el contador de 32 bits `frameCounter`.
8. Restaura los registros guardados y sale con `rti`.

### Convenciones de memoria usadas en el ejemplo

El ejemplo sitúa las variables mencionadas en Low-RAM (rangos de direcciones `$00`-`$3f`, `$7e`, `$80`-`$bf`), que es la zona de RAM de acceso más rápido/directo en el 65C816. También fija explícitamente el tamaño de los registros con directivas de ensamblador tipo `.a8` (acumulador de 8 bits) y `.i16` (índices de 16 bits) antes de las secciones de código que lo requieren.

## Notas / gotchas

- Es NMI (no maskeable), así que no se puede desactivar con `cli`/`sei`; la única forma de "silenciarla" es limpiar el bit correspondiente en NMITIMEN.
- Sin la bandera de control (`vBlankFlag` o equivalente), una NMI que salte en mitad de una configuración manual de PPU (carga de nivel, etc.) puede corromper direcciones de CGRAM/OAM/VRAM a medio escribir.
- WMADD/WMDATA y los registros de multiplicación/división son secuencias de varios pasos: si el bucle principal los está usando y llega una NMI que también los toca, el resultado puede quedar corrupto. Por eso se recomienda que solo el bucle principal los use, nunca la rutina de VBlank, o viceversa, pero no ambos.
- El concepto de "lag frame" (frame en el que el juego no terminó su lógica a tiempo antes de la siguiente VBlank) se puede detectar comprobando si `vBlankFlag` seguía en cero cuando llegó la nueva NMI.
- La ISR de NMI debe resetear DB y DP explícitamente: no se puede asumir en qué banco/página estaba ejecutándose el código interrumpido.

## Referencias cruzadas

- Ver `vblank-routine.md` (qué hace exactamente la rutina de VBlank invocada desde esta interrupción: shadow variables, buffers, colas, HDMA, lectura de mandos).

---
titulo: Driver de música PSG en Z80 con decaimiento de subframe (Bumbershoot)
plataforma: [MegaDrive]
categoria: [sonido]
tags: [psg, sn76489, z80, driver, interrupciones, envolvente, im1]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/03/24/genesis-the-z80-and-the-sn76489/]
---

# Driver de música PSG en Z80 con decaimiento de subframe (Bumbershoot)

## Resumen

Bumbershoot Software documenta la construcción de un reproductor de música mínimo para el PSG (SN76489) que corre *enteramente* en el Z80, disparado por su interrupción periódica (equivalente al VBLANK del 68000). A diferencia de `psg.md` (que cubre el chip a nivel de bits) o `using-the-z80.md` (que cubre el control del bus Z80 desde el 68000), esta entrada aporta un ejemplo completo de driver: formato de datos de canción, lectura de notas por interrupción, y una técnica de decaimiento de volumen con precisión de "subframe" usando solo aritmética entera de 8 bits.

## Detalle técnico

### El chip desde el punto de vista del Z80

El PSG se ve desde el Z80 en el puerto `$7F11` (equivalente al `$C00011` visto desde el 68000; ver `psg.md` para el mapeo completo). Todas las escrituras empiezan con un byte cuyo bit 7 está activado: bits 6-4 seleccionan el registro, bits 3-0 llevan los 4 bits bajos del valor. Los registros de tono (0, 2, 4) requieren un segundo byte con los 6 bits altos restantes.

Fórmula de frecuencia a partir del reloj de 3.58MHz del PSG:

```
n = 3580000 / (32*hz)
```

Los registros de volumen son de atenuación logarítmica (0-15, cada paso -2dB, 15 = silencio) y son todos los registros impares del chip; por eso la secuencia para "silenciar el PSG por completo" es escribir los cuatro bytes `$9F, $BF, $DF, $FF` (uno por canal, todos a atenuación máxima).

### Modo de interrupción 1 del Z80

El driver corre en el gestor de la interrupción periódica del Z80. Detalles de hardware relevantes que no están cubiertos en otras entradas de RetroPedia sobre el Z80:

- En modo de interrupción 1 (`im 1`), cualquier interrupción salta siempre a `RST $38` (dirección fija, sin vector).
- A diferencia de otros procesadores de esta plataforma, en el Z80 `reti` y `retn` no hacen nada distinto de un `ret` normal a nivel de chip (solo importan para hardware externo que vigile esas señales, como un controlador de interrupciones encadenado).
- La instrucción `ei` retrasa su efecto: las interrupciones no se reactivan hasta *después* de que termine la instrucción siguiente a `ei`. Esto evita que la propia rutina de interrupción se re-interrumpa a sí misma nada más volver de `reti`/`ret`.

### Rutina de reset e inicialización

```asm
rst_0:  di
        im      1
        ld      sp, $2000
        ei
rst_lp: jr      rst_lp
```

Deshabilita interrupciones, fija el modo 1, coloca la pila en `$2000` (justo por encima del final de la RAM del Z80), reactiva interrupciones y entra en bucle infinito esperando la siguiente interrupción periódica.

### Estado del reproductor

Variables mantenidas entre la rutina de reset y el manejador de `$38`:

- **`ptr`**: puntero de 16 bits a la posición actual dentro de los datos de la canción (inicializado a `song`).
- **`vol`**: array de 3 bytes, un valor de atenuación "de trabajo" por voz (inicializados a `$1f`, silencio).
- **`wait`**: frames restantes hasta el siguiente registro de notas (inicializado a 1).

### Formato de los datos de canción

Cada registro de la canción tiene un byte de retardo (frames a esperar antes de procesar el siguiente registro; `0` indica "volver al punto de loop") seguido de una entrada por voz: `0` significa "no tocar nada en esta voz", cualquier otro valor indica que siguen dos bytes que se escriben tal cual al puerto del PSG (el tono ya preformateado). Los registros tienen longitud variable, ya que el puntero se avanza a medida que se consumen los valores.

### Lectura de notas en la interrupción

```asm
        ld      hl, wait
        dec     (hl)
        jr      nz, decay

        ld      hl, (ptr)
        ld      a, (hl)
        and     a
        jr      nz, nolp
        ld      hl, segno
        ld      a, (hl)
nolp:   ld      (wait), a
        inc     hl

        ld      a, (hl)
        inc     hl
        and     a
        jr      z, v2
        ;; Voice 1
        ld      (de), a
        ld      a, (hl)
        inc     hl
        ld      (de), a
        ld      a, 7
        ld      (vol),a
v2:

vdone:  ld      (ptr), hl
```

Si el byte de retardo leído es `0`, se relee desde la etiqueta de loop (`segno`) en vez de seguir avanzando. Para cada voz, si el byte leído no es cero se escriben directamente sus dos bytes al puerto del PSG (registro `de`, apuntando a `$7F11`) y se resetea el volumen de trabajo de esa voz a 7 (el patrón se repite igual para las voces 2 y 3, omitido aquí por brevedad).

### Decaimiento de envolvente con precisión de subframe

El PSG no tiene envolvente por hardware (ver `psg.md`), así que hay que simularla. La particularidad de este driver es que, en vez de decrementar el volumen un paso entero por frame (lo que da un decaimiento muy basto, ya que solo hay 16 niveles), incrementa un contador de trabajo hasta *el doble* del valor máximo de atenuación y solo entonces lo divide entre 2 antes de escribirlo al chip — la misma idea que el "subpíxel" usado en motores de física para acumular movimiento fraccionario:

```asm
decay:  ld      a, (vol)
        cp      a, $1f
        jr      z, nodec1
        inc     a
        ld      (vol), a
nodec1: srl     a
        or      $90
        ld      (de), a
```

Esto hace que la atenuación real solo avance un paso cada dos frames en la mitad de los casos, dando un decaimiento efectivamente el doble de fino que el rango de 0-15 del registro permitiría de otro modo. El bucle de interrupción termina con:

```asm
ei
reti
```

### Carga del código Z80 en el cartucho

El ensamblador Z80 usado (z80asm) produce un binario que se convierte a directivas `dc.b`/hex para incluirlo en el build del 68000. La carga e inicialización del Z80 en tiempo de ejecución es una variación menor del cargador genérico de la rutina de arranque (ver `using-the-z80.md` para el procedimiento estándar de petición de bus/reset).

## Notas / gotchas

- `ei` no reactiva las interrupciones de inmediato: el efecto se retrasa hasta después de la instrucción siguiente, precisamente para evitar que el propio handler se re-interrumpa al volver de `reti`.
- `reti`/`retn` en el Z80 real no hacen nada especial a nivel de CPU respecto a un `ret` simple; solo importan de cara a hardware externo que las distinga (irrelevante en la Mega Drive salvo que se implemente algo muy específico).
- Bug real del autor: olvidar el `align 2` tras el blob de código Z80 incluido en el binario del 68000 desalineó el resto del código a partir de ahí, con fallos difíciles de rastrear.
- Otros bugs reales documentados: olvidar reactivar interrupciones (solo suena la primera nota), no volver a guardar el puntero de canción actualizado en `ptr` tras procesarlo, valores de tono que exceden el rango de 10 bits del PSG, y una etiqueta de loop mal escrita (`signum` en vez de `segno`) que compilaba pero apuntaba a la dirección equivocada.
- La secuencia de silenciar los cuatro canales del PSG de golpe es escribir los bytes `$9F, $BF, $DF, $FF`.

## Referencias cruzadas

- [PSG (Programmable Sound Generator)](psg.md) — mapa de bits y formato de registros del SN76489 en detalle; esta entrada se centra en un driver funcional construido sobre esa base.
- [Usando el Z80 en la Mega Drive](../cpu/using-the-z80.md) — control del bus del Z80 desde el 68000 (carga de programas, pausa/reanudación); no cubre el modo de interrupción 1 ni `ei`/`reti`, que sí se detallan aquí.

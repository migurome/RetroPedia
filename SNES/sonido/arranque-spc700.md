---
titulo: Arranque del SPC700 (protocolo IPL)
plataforma: [SNES]
categoria: [sonido]
tags: [spc700, s-smp, ipl, boot, arranque, audio, apuio]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Booting_the_SPC700]
---

# Arranque del SPC700 (protocolo IPL)

## Resumen

Al encender la consola, el SPC-700 ejecuta su propia ROM interna (IPL) y queda a la espera de recibir un programa de sonido desde la CPU principal (65816). Esta entrada documenta el protocolo de negociación por software que usa la IPL para transferir código y datos byte a byte hacia la RAM de audio, y cómo arrancar la ejecución del programa recién subido.

## Detalle técnico

### Registros de comunicación

El 65816 se comunica con el SPC-700 mediante cuatro puertos: `$2140`, `$2141`, `$2142` y `$2143` (vistos desde el bus del 65816). Estos mismos cuatro bytes se corresponden, desde el lado del SPC-700, con `$F4`, `$F5`, `$F6` y `$F7` (llamados aquí `APUIO0`-`APUIO3`).

### Paso 1: esperar a que el SPC700 esté listo

La IPL señala que ha terminado su inicialización escribiendo `$AA` en `APUIO0` y `$BB` en `APUIO1`. El 65816 debe sondear estos puertos hasta ver esos valores antes de continuar.

### Paso 2: fijar la dirección de destino

1. Escribir el byte bajo de la dirección destino en `APUIO2`.
2. Escribir el byte alto de la dirección destino en `APUIO3`.
3. Escribir un valor distinto de cero en `APUIO1`.
4. Escribir `$CC` en `APUIO0`.
5. Esperar a que el SPC700 haga eco de `$CC` de vuelta en `APUIO0`.

Los programas de SPC700 normalmente empiezan en `$0200`, ya que `$0000-$00FF` contiene la página cero y los registros, y `$0100-$01FF` contiene la pila.

### Paso 3: enviar los datos

Para cada byte del bloque a transferir:

1. Escribir el byte de datos en `APUIO1`.
2. Escribir el byte bajo del índice actual en `APUIO0`.
3. Esperar la confirmación (eco) del SPC700.
4. Incrementar el índice y repetir.

### Paso 4: cambiar de dirección (si hace falta subir más de un bloque)

Escribir la nueva dirección en `APUIO2`/`APUIO3`, y luego escribir en `APUIO0` un valor que sea al menos 2 unidades mayor que el último valor escrito anteriormente, para que el SPC700 detecte el cambio de bloque.

### Paso 5: arrancar la ejecución

Escribir la dirección de inicio del programa en `APUIO2`/`APUIO3`, escribir 0 en `APUIO1` (esto es la señal de "arrancar"), escribir en `APUIO0` un valor incrementalmente mayor que el anterior, y esperar la confirmación correspondiente.

### Acceso directo al DSP durante el arranque

También es posible fijar la dirección de destino a `$00F2` y, en lugar de subir un programa, escribir directamente un número de registro del DSP seguido de su valor — esto permite programar el S-DSP sin necesidad de subir código ejecutable al SPC700.

## Notas / gotchas

- Todo el protocolo depende de sondear (polling) los puertos `APUIO0`-`APUIO3` desde el 65816; si el SPC700 no responde como se espera, el arranque se queda colgado — es una fuente típica de bugs de "el sonido no arranca" en homebrew.
- El valor de `APUIO0` debe ir siempre incrementando (o subiendo en al menos 2) entre bloques, es el mecanismo que usa la IPL para distinguir "sigue el mismo byte" de "nuevo byte/bloque".
- Es habitual colocar el programa de sonido a partir de `$0200` para no pisar la página cero ni la pila del SPC700.
- Esta técnica de arranque es relativamente lenta (ver límite de ~520 ciclos de reloj maestro por byte en la entrada de S-SMP); para transferencias de datos más rápidas durante el juego (no solo en el arranque) existe la técnica de subida "Blargg", documentada aparte.

## Referencias cruzadas

- (pendiente de enlazar)

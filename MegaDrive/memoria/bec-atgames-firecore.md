---
titulo: Depurando el AtGames Firecore — bug de tono/pitch de audio (TANGLEWOOD)
plataforma: [MegaDrive]
categoria: [memoria]
tags: [bigevilcorporation, tanglewood, atgames, firecore, clones, audio, debugging]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2018/04/18/taming-the-atgames-firecore/]
---

# Depurando el AtGames Firecore — bug de tono/pitch de audio (TANGLEWOOD)

## Resumen

Artículo de Matt (Big Evil Corporation) publicado el 18/04/2018, "Taming the AtGames Firecore – Part 1". Relata la depuración de un bug real encontrado durante el desarrollo de TANGLEWOOD (juego comercial publicado): en el clon de hardware AtGames Firecore, todo el audio del juego sonaba desafinado (pitch incorrecto), con desviaciones distintas entre el FM y el PSG, y con "chillidos" (screeching) en ciertos timbres.

## Detalle técnico

### El problema

El autor se había comprometido a garantizar que TANGLEWOOD funcionara en cualquier consola capaz de ejecutar Sonic 2, lo que incluye clones "todo en uno" como los de AtGames. En el Firecore, el audio completo aparecía con el tono desplazado hacia abajo, y la desviación no era igual entre el sintetizador FM y el PSG, además de aparecer distorsión ("screeching") en algunos timbres concretos.

### Causa raíz

El Firecore implementa un bloque de registros de control no estándar (no presente en una Mega Drive original) mapeado a partir de la dirección `0x00B00000`. Estos registros permiten ajustar las frecuencias de reloj internas de los sintetizadores FM y PSG, y en el hardware del autor estaban configuradas con valores desviados de los nominales de una Mega Drive real, lo que explica el pitch incorrecto y la distinta desviación entre FM y PSG. Además, el modo `SSG-EG` de los operadores del YM2612 no estaba correctamente emulado en este clon, lo que producía la distorsión/screeching adicional en ciertos timbres.

### Mecanismo de acceso (latch) a los registros del Firecore

Para que las escrituras a partir de `0x00B00000` surtieran efecto, había que "desbloquear" el acceso escribiendo un valor de latch concreto en la dirección `$000000`, hacer las escrituras necesarias, y luego volver a bloquear escribiendo otro valor distinto en la misma dirección:

```
; AtGames audio fix
move.w #0xFFFF, 0x00000000  ; Latch 0x0000
move.b #0x78, 0x00B00018    ; Write new PSG frequency
move.b #0x26, 0x00B01054    ; Write new FM frequency
move.b #0x20, 0x00B01055    ; Write new FM clock
move.w #0xFFF7, 0x00000000  ; Un-latch
```

### Registros identificados y valores de corrección aplicados

| Dirección | Función | Valor de corrección |
|---|---|---|
| `0x00B00018` | Control de frecuencia del PSG (bits bajos = afinación fina; resto = divisor) | `0x78` |
| `0x00B01054` | Ajuste de pitch del FM | `0x26` |
| `0x00B01055` | Control de reloj FM/PSG (tempo) | `0x20` |
| `0x00000000` | Registro de latch, necesario para validar cualquier escritura en el rango `0xB00000+` | `0xFFFF` (activar) / `0xFFF7` (desactivar) |

### Método de diagnóstico

El autor usó su propio fork del emulador *megaEx* con logging de accesos a direcciones desconocidas para detectar los patrones de lectura/escritura sospechosos del hardware real. A partir de ahí, construyó una pantalla de prueba de sonido interactiva ("address poker") dentro del propio juego que le permitió escribir valores de prueba a cada registro candidato en tiempo real sobre hardware Firecore físico, hasta acotar qué registros afectaban al tono y en qué proporción.

## Notas / gotchas

- No fue posible detectar el hardware Firecore de forma automática en tiempo de ejecución: leer cualquiera de los registros de configuración de AtGames en `0xB00000+` cuelga una Mega Drive original genuina, así que no hay una comprobación segura y portable para decidir "esto es un Firecore, aplica el fix" sin arriesgar colgar la consola real.
- Como consecuencia del punto anterior, tampoco pudo condicionar la desactivación de `SSG-EG` solo al caso Firecore de forma fiable.
- Es un ejemplo de ingeniería inversa de hardware clon no documentado en un juego comercial ya publicado, no un bug de la propia Mega Drive.

## Referencias cruzadas

- [`hardware-issues.md`](hardware-issues.md) (fuente plutiedev.com) — menciona brevemente que "la función SSG-EG del YM2612 no está correctamente implementada en los clones vendidos por AtGames"; esta entrada es el relato detallado de primera mano (con registros y valores concretos) detrás de esa nota breve.
- [`bec-atgames-flashback.md`](bec-atgames-flashback.md) — segundo artículo de la misma investigación del autor sobre compatibilidad con clones AtGames, esta vez centrado en un bug del emulador de Z80 del modelo Flashback (también con SSG-EG no emulado).
- [`../sonido/ym2612-registers.md`](../sonido/ym2612-registers.md) — referencia de registros del YM2612 estándar, útil para contrastar contra el comportamiento no estándar del Firecore.

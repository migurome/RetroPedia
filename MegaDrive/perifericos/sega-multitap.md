---
titulo: Sega Multitap (Team Player / Sega Tap)
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [multitap, team-player, io, protocolo, multijugador]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/sega-multitap]
---

# Sega Multitap (Team Player / Sega Tap)

## Resumen

El Sega Multitap (conocido como Team Player o Sega Tap según la región) es un adaptador de cuatro jugadores que ocupa un único puerto de mando, permitiendo conectar hasta dos multitaps simultáneamente. Soporta mandos de 3 botones, mandos de 6 botones y ratones.

## Detalle técnico

### Requisitos de configuración

Para habilitar la funcionalidad multitap, hay que poner el dispositivo en modo MULTI. La detección implica comprobar el ID de periférico (valor 7, o `%0111`). Se inicializan tanto el puerto de control como el de datos a `$60`:

```
move.b  #$60, (IoCtrl1)
move.b  #$60, (IoData1)
```

### Protocolo de lectura

La secuencia de comunicación comienza así:
1. Leer del puerto de datos.
2. Escribir `$20` en el puerto de datos.
3. Leer de nuevo.

La validación inicial comprueba que los bits 3-0 sean `%0011` en la primera lectura y `%1111` en la segunda.

### Procedimiento de lectura de nibbles

El multitap transmite datos en nibbles mediante un mecanismo de handshake:

1. Alternar el bit 5 del puerto de datos.
2. Esperar hasta que el bit 4 coincida con el nuevo estado del bit 5.
3. Extraer los bits 3-0 como el valor del nibble.

### Formato de cabecera

Los primeros seis nibbles forman una cabecera:
- Nibbles 1-2: deben ser `0000` (validación).
- Nibbles 3-6: IDs de dispositivo para los puertos 1-4.

Valores de ID de dispositivo:
- `0000` = mando de 3 botones
- `0001` = mando de 6 botones
- `0010` = ratón
- `1111` = nada conectado

### Formatos de datos por dispositivo

Mando de 3 botones (2 nibbles):
- Nibble 1: direccionales Right, Left, Down, Up.
- Nibble 2: botones Start, A, C, B.

Mando de 6 botones (3 nibbles):
- Nibbles 1-2: igual que el de 3 botones.
- Nibble 3: botones Mode, X, Y, Z.

Ratón (6 nibbles):
- Nibbles 1-2: datos de overflow y botones.
- Nibbles 3-6: valores de coordenadas X e Y.

Todos los bits de botón usan lógica invertida (0=presionado, 1=liberado).

### Declaración en la cabecera de la ROM

Se especifica el soporte de multitap añadiendo `"4"` en el campo de dispositivos en el offset `$000190`. Incluir `"6"` para soporte de 6 botones y `"M"` para soporte de ratón, solo si estas características están realmente implementadas.

## Notas / gotchas

- "Make sure to add a timeout while waiting for bit 4 to change! If the multitap gets unplugged in the middle of waiting, your game will get stuck." — hay que añadir un timeout al esperar el cambio del bit 4, o el juego se puede quedar bloqueado si se desconecta el multitap.

## Referencias cruzadas

- (pendiente de enlazar)

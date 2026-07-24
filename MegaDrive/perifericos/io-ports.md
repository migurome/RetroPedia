---
titulo: Puertos de I/O de la Mega Drive
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [io, puertos, serial, paralelo, registros, interrupcion]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/io-ports]
---

# Puertos de I/O de la Mega Drive

## Resumen

La Mega Drive dispone de tres puertos de I/O físicos (Player 1, Player 2 y Modem/EXT) que se controlan mediante registros mapeados en memoria. Cada puerto puede operar en modo paralelo (acceso directo a pines) o en modo serial (comunicación tipo UART). Esta entrada documenta las direcciones de estos registros y su configuración.

## Detalle técnico

### Puertos disponibles

La Mega Drive posee tres puertos I/O: Player 1, Player 2 y Modem (EXT). Los dos primeros alojan controles; el puerto de módem solo existe en consolas tempranas, ubicado en la parte trasera con un conector invertido.

### Modo paralelo — direcciones base

| Puerto | Control | Datos |
|--------|---------|-------|
| Player 1 | `$A10009` | `$A10003` |
| Player 2 | `$A1000B` | `$A10005` |
| Modem | `$A1000D` | `$A10007` |

Cada puerto dispone de 7 pines configurables como entrada (0) o salida (1) mediante el registro de control. El registro de datos lee/escribe los valores en los pines.

Configuración típica:

```
move.b  #$40, (IoCtrl1)
move.b  #$40, (IoData1)
```

### Interrupción externa

Se activa cuando el Pin 6 cambia de alto a bajo. Requiere: bit 7=1 en el registro de control, bit 3=1 en el registro `$8Bxx` del VDP, e interrupciones del 68000 habilitadas. Corresponde a la IRQ 2.

### Modo serial — direcciones

| Puerto | Control | RxData | TxData |
|--------|---------|--------|--------|
| Player 1 | `$A10013` | `$A10011` | `$A1000F` |
| Player 2 | `$A10019` | `$A10017` | `$A10015` |
| Modem | `$A1001F` | `$A1001D` | `$A1001B` |

Velocidades seriales:
- `00`: 4800 bps (480 bytes/sec)
- `01`: 2400 bps (240 bytes/sec)
- `10`: 1200 bps (120 bytes/sec)
- `11`: 300 bps (30 bytes/sec)

## Notas / gotchas

- "You _must_ read from the serial control register even if you're sure it's OK to send a new value, or otherwise the hardware may not send the correct data" — hay que leer siempre el registro de control serial antes de enviar un nuevo valor, incluso si se está seguro de que se puede enviar.
- El puerto de Modem solo está presente físicamente en consolas tempranas.

## Referencias cruzadas

- (pendiente de enlazar)

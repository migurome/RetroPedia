---
titulo: NTT Data Keypad (NDK10) de Super Famicom
plataforma: [SNES]
categoria: [perifericos]
tags: [ntt-data-keypad, teclado-numerico, modem, io, protocolo]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/NTT_Data_Keypad]
---

# NTT Data Keypad (NDK10) de Super Famicom

## Resumen

El NTT Data Keypad (modelo NDK10) es un mando especializado japonés que añade 15 botones adicionales (un teclado numérico) a un mando estándar de SNES, pensado para usarse junto con el módem de comunicaciones NTT Data del Super Famicom. Servía para introducir números y comandos en los servicios de red que ofrecía dicho módem.

## Detalle técnico

### Extensión del protocolo a 32 bits

El dispositivo amplía el protocolo estándar de 16 bits a 32 bits. Los primeros 16 bits (0-15) mantienen la misma disposición que un mando normal, aunque con una firma distinta que permite identificarlo como periférico especial. Los 16 bits superiores (16-31) siguen el formato del Controlador de Red de Famicom, correspondiente al teclado numérico.

### Bits 0-15 — parte de mando estándar

- Bit 0: B
- Bit 1: Y
- Bit 2: ᐊ / página anterior (前ページ)
- Bit 3: ᐅ / página siguiente (次ページ)
- Bits 4-7: direccionales (Arriba, Abajo, Izquierda, Derecha)
- Bit 8: A
- Bit 9: X
- Bits 10-11: L, R
- Bits 12, 14-15: siempre 0
- Bit 13: siempre 1

### Bits 16-31 — teclado numérico

- Bits 16-25: dígitos del 0 al 9
- Bit 26: `*` (asterisco)
- Bit 27: `#` (numeral)
- Bit 28: `.` (punto)
- Bit 29: `C` (clear/cancelar)
- Bit 30: siempre 0
- Bit 31: fin de comunicación (通信終了)
- Bits 32 en adelante: siempre 1

## Notas / gotchas

- Los bits fijos en 0 o 1 (por ejemplo el bit 13 siempre en 1) actúan como firma de identificación, permitiendo distinguir este periférico de un mando estándar o de otros dispositivos especiales antes de interpretar el resto del paquete.
- Al ser un dispositivo de nicho ligado a un servicio de red japonés (el módem NTT Data), su soporte en software es prácticamente inexistente fuera de aplicaciones específicas de la época.

## Referencias cruzadas

- (pendiente de enlazar)

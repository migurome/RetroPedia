---
titulo: Conector de mando de SNES (puerto de controlador)
plataforma: [SNES]
categoria: [perifericos]
tags: [pinout, mando, conector, hardware, io]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Controller_connector]
---

# Conector de mando de SNES (puerto de controlador)

## Resumen

El puerto de mando de SNES es un conector de 7 pines (idéntico en forma para los dos puertos de la consola) que transporta alimentación, las señales de reloj/latch necesarias para el protocolo serie de lectura, dos líneas de datos de entrada y una línea de E/S bidireccional adicional usada por periféricos especiales. Esta entrada documenta el pinout de ambos puertos y el cableado de color estándar de un mando oficial.

## Detalle técnico

### Pinout — Puerto 1

| Pin | Señal | Función | Tipo |
|-----|-------|---------|------|
| 1 | +5V | Alimentación | Voltaje |
| 2 | joypad 1 /OE | Reloj del mando | Salida (de la consola) |
| 3 | OUT0 | Latch de estado de botones | Salida (de la consola) |
| 4 | joypad 1 D0 | Entrada de datos 0 | Entrada (a la consola) |
| 5 | joypad 1 D1 | Entrada de datos 1 | Entrada (a la consola) |
| 6 | joypad IO D6 | E/S bidireccional | Bidireccional |
| 7 | GND | Tierra | Voltaje |

### Pinout — Puerto 2

| Pin | Señal | Función | Tipo |
|-----|-------|---------|------|
| 1 | +5V | Alimentación | Voltaje |
| 2 | joypad 2 /OE | Reloj del mando | Salida (de la consola) |
| 3 | OUT0 | Latch de estado de botones | Salida (de la consola) |
| 4 | joypad 2 D0 | Entrada de datos 0 | Entrada (a la consola) |
| 5 | joypad 2 D1 | Entrada de datos 1 | Entrada (a la consola) |
| 6 | joypad IO D7 | E/S bidireccional | Bidireccional |
| 7 | GND | Tierra | Voltaje |

### Función de las señales de control

- **OUT0**: hace de "latch" del estado de botones en el registro de desplazamiento interno del mando; se controla desde el registro JOYOUT de la consola.
- **/OE (reloj)**: se activa al leer los registros JOYSER0/JOYSER1, o automáticamente durante la lectura automática de mandos.
- **Línea IO D6/D7**: es una línea bidireccional adicional; en el puerto 2 (D7) puede funcionar además como entrada para un lápiz óptico ("light pen").

### Cableado de color estándar de un mando

| Color | Pin | Función |
|-------|-----|---------|
| Blanco | 1 | +5V |
| Amarillo | 2 | joypad /OE |
| Naranja | 3 | OUT0 |
| Rojo | 4 | joypad D0 |
| Marrón | 7 | GND |

(No se especifican colores estándar para los pines 5 y 6 en la fuente.)

## Notas / gotchas

- El pin 6 (puerto 1) y el pin 6 (puerto 2, ahí llamado D7) no son equivalentes en todos los periféricos: en el puerto 2 esa línea puede actuar como entrada de lápiz óptico, algo específico de ese puerto.
- El protocolo serie que viaja por D0/D1 y se controla con /OE y OUT0 es el mismo que se documenta en la entrada de lectura de mando; esta página cubre únicamente la asignación física de pines, no la temporización del protocolo.
- Periféricos como el ratón, el Super Scope o el Turbo File Twin reutilizan estas mismas líneas físicas con semánticas de protocolo distintas a las de un mando estándar.

## Referencias cruzadas

- (pendiente de enlazar)

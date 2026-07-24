---
titulo: Pinout de los puertos de I/O (Mega Drive y Saturn)
plataforma: [MegaDrive]
categoria: [perifericos]
tags: [pinout, db9, io, hardware, saturn]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/io-pinout]
---

# Pinout de los puertos de I/O (Mega Drive y Saturn)

## Resumen

Esta entrada documenta el pinout físico de los puertos de mando de Mega Drive (conector DE-9) y de Saturn, así como la correspondencia de nombres de señal entre ambos sistemas, útil para construir adaptadores.

## Detalle técnico

### Pinout del puerto I/O de Mega Drive

El puerto utiliza conectores con forma DE-9. La numeración (vista frontal) es:
- Fila superior: 1, 2, 3, 4, 5
- Fila inferior: 6, 7, 8, 9

Asignación de pines:

| Pin | Función |
|-----|---------|
| 1 | data0 |
| 2 | data1 |
| 3 | data2 |
| 4 | data3 |
| 5 | VCC |
| 6 | TL |
| 7 | TH |
| 8 | ground |
| 9 | TR |

### Pinout del puerto I/O de Saturn

Los puertos disponen de 9 conexiones en línea. Numeración: 1 a 9 de izquierda a derecha.

| Pin | Función |
|-----|---------|
| 1 | VCC |
| 2 | data1 |
| 3 | data0 |
| 4 | TH |
| 5 | TR |
| 6 | TL |
| 7 | data3 |
| 8 | data2 |
| 9 | ground |

Los puertos de Saturn comparten las mismas líneas que los de Mega Drive, pero con conectores diferentes. Es posible crear adaptadores pasivos para intercambiar periféricos entre ambas plataformas.

### Tabla de equivalencias de nomenclatura

| Nombre común | Nombre Mega Drive |
|---|---|
| VCC | VCC |
| GND | GND |
| D0 | DATA0 |
| D1 | DATA1 |
| D2 | DATA2 |
| D3 | DATA3 |
| S0 | TH |
| S1 | TR |
| +5V | TL |

## Notas / gotchas

- Los periféricos de Master System utilizan el mismo pinout que Mega Drive y funcionan en juegos homebrew de Mega Drive.
- "La detección automática frecuentemente no es viable", por lo que se recomienda mantener compatibilidad con las convenciones de Mega Drive cuando sea posible.
- Advertencia crítica: los joysticks de sistemas antiguos (principalmente computadoras) con puertos DE-9 tienen un "pinout generalmente incompatible". Se requiere un adaptador para evitar dañar componentes, dado que VCC y GND están distribuidos de forma irregular entre plataformas.

## Referencias cruzadas

- (pendiente de enlazar)

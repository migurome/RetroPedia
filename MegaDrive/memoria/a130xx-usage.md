---
titulo: Uso del rango $A130xx
plataforma: [MegaDrive]
categoria: [memoria]
tags: [a130xx, time-signal, mapper, eeprom, cartucho]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/a130xx-usage]
---

# Uso del rango $A130xx

## Resumen

Describe el rango especial de direcciones `$A13000` a `$A130FF` en cartuchos de Mega Drive/Genesis, usado por distintos periféricos y mappers de terceros, y cómo repartirse ese espacio sin generar conflictos.

## Detalle técnico

Este rango es especial porque "la señal `/TIME` en el conector de cartucho se activa siempre que el 68000 accede a él".

### Tabla de asignación de direcciones

Las siguientes direcciones ya están reclamadas:

| Rango de direcciones | Usado por |
|---|---|
| `$A13000`–`$A1300F` | Uso privado |
| `$A130C0`–`$A130CF` | Mega Wifi |
| `$A130D0`–`$A130E7` | Mega Everdrive |
| `$A130EC`–`$A130EF` | Firma 32X |
| `$A130F0`–`$A130FF` | Mapper de Sega |

## Notas / gotchas

- **Uso de Pier Solar:** el documento advierte que Pier Solar "usa la región `$A13000` a `$A1300F` para su mapper personalizado y acceso a EEPROM", lo que la hace inadecuada para compatibilidad cruzada entre juegos.
- **Estandarización de EEPROM:** la dirección `$A130E8` "se está considerando para una posible estandarización del uso de EEPROM".
- La página enfatiza la necesidad de coordinación cuidadosa para evitar conflictos de direcciones dentro de esta ventana limitada de 256 bytes.

## Referencias cruzadas

- (pendiente de enlazar)

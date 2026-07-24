---
titulo: Formatos de archivo de ROM
plataforma: [SNES]
categoria: [memoria]
tags: [rom, formatos-archivo, copier-header, lorom, hirom]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/ROM_file_formats]
---

# Formatos de archivo de ROM

## Resumen

Describe cómo se organizan los archivos de ROM de SNES en disco: extensiones habituales, la diferencia entre archivos "con cabecera" y "sin cabecera" (los 512 bytes extra que añadían algunos dispositivos copiadores de época), y cómo detectar y tratar esa cabecera de copiadora al escribir herramientas propias.

## Detalle técnico

### Extensiones de archivo habituales

Las extensiones más comunes para ROMs de SNES son `.sfc` (la más habitual hoy en día) y `.smc`, con variantes menos frecuentes como `.fig` y `.swc`.

### Archivos con cabecera vs. sin cabecera

La diferencia entre ambas variantes es únicamente de tamaño: la versión *con cabecera* tiene 512 bytes extra al principio del archivo, antepuestos a los datos reales de la ROM.

- **Tamaño de la cabecera de copiadora:** 512 bytes.
- **Posición:** al principio del archivo (offset `$000`-`$1FF`), antes de que empiece el contenido real de la ROM.
- **Origen:** metadatos añadidos por los dispositivos "copiadores" (copiers) que se usaban antiguamente para volcar cartuchos.
- Ese contenido extra se considera generalmente inútil hoy en día, salvo para el propio dispositivo copiador con el que se generó originalmente. El formato sin cabecera es el preferido en el uso moderno (emuladores, herramientas de desarrollo).

### Organización de la ROM según el modo de mapeo

| Modo de mapeo | Organización |
|---|---|
| LoROM | bancos de 32 KiB a partir de `$800000` |
| HiROM | bancos de 64 KiB a partir de `$C00000` |
| ExHiROM | bancos de 64 KiB a partir de `$C00000`, y tras los primeros 4 MiB continúa desde `$400000` |

La posición de la cabecera de la ROM (ver `rom-header`) dentro del *archivo* (sin contar una posible cabecera de copiadora) varía según el modo, aunque siempre se proyecta en `$00FFC0` en el espacio de direcciones del CPU:

| Modo de mapeo | Offset de la cabecera en el archivo |
|---|---|
| LoROM | `$007FC0` |
| HiROM | `$00FFC0` |
| ExHiROM | `$40FFC0` |

### Cómo detectar si un archivo tiene cabecera de copiadora

Método simple: comprobar si el tamaño del archivo módulo 1024 es igual a 512 (`tamaño % 1024 == 512`). Si es así, el archivo tiene los 512 bytes de cabecera de copiadora antepuestos y hay que descontarlos al calcular los offsets reales de ROM. Como alternativa/verificación adicional, se puede buscar la cabecera de la ROM (ver `rom-header`) desplazada 512 bytes respecto a la posición esperada, si no se encuentra en la posición estándar.

## Notas / gotchas

- Al escribir cualquier herramienta que procese ROMs de SNES (parcheadores, extractores de gráficos, etc.), lo primero es detectar y, si aplica, descartar la cabecera de copiadora de 512 bytes — de lo contrario todos los offsets calculados quedarán desplazados.
- El check `tamaño % 1024 == 512` es una heurística práctica, no una garantía absoluta; conviene combinarla con la verificación de la cabecera de la ROM en la posición esperada.
- Las extensiones de archivo (`.sfc`, `.smc`, `.fig`, `.swc`) no determinan de forma fiable si el archivo tiene o no cabecera de copiadora — hay que comprobarlo por tamaño/contenido, no por extensión.

## Referencias cruzadas

- (pendiente de enlazar)

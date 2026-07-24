---
titulo: Formato de compresión SLZ
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [compresion, slz, lz, mdtools, homebrew]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/format-slz]
---

# Formato de compresión SLZ

## Resumen

SLZ es un formato de compresión LZ simple diseñado para homebrew de Mega Drive, adecuado para comprimir datos genéricos que no son críticos en tiempo. Incluye una variante SLZ24 para volúmenes de datos mayores.

## Detalle técnico

"SLZ es un formato de compresión LZ simple hecho para homebrew de Mega Drive, adecuado para la compresión de datos genéricos que no son críticos en tiempo."

### Estructura del archivo

**Cabecera:**
Los primeros dos bytes (big endian) contienen la longitud de los datos sin comprimir, que determina el punto de terminación de la descompresión.

**Formato del flujo comprimido:**
El proceso de descompresión sigue esta secuencia:

1. Leer un byte de control que define los tipos de token para las siguientes ocho entradas (un bit cada una)
2. Procesar los bits desde la posición 7 hasta la 0
3. Valor de bit 0 = byte crudo (leer y almacenar tal cual)
4. Valor de bit 1 = referencia a cadena (codificación de dos bytes)
5. Repetir con un nuevo byte de control cuando se agoten los bits

**Codificación de cadena:**
Dos bytes (big endian) que contienen:

- Bits 3-0: valor de longitud (sumar 3 para obtener la longitud real)
- Bits 15-4: valor de distancia (sumar 3 para obtener la distancia real)

El descompresor copia el número especificado de bytes desde los datos ya descomprimidos, a la distancia calculada.

### Variante SLZ24

Esta variante usa tres bytes para la longitud sin comprimir en lugar de dos, permitiendo "un máximo de 16MB" para sistemas como Mega CD con RAM ampliada.

## Notas / gotchas

- El código fuente (rutinas de descompresión en ensamblador y C, más la herramienta de compresión) está disponible bajo licencia zlib en el repositorio de GitHub de mdtools.

## Referencias cruzadas

- (pendiente de enlazar)

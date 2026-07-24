---
titulo: Formato de compresión UFTC
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [compresion, uftc, tiles, mdtools, homebrew]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/format-uftc]
---

# Formato de compresión UFTC

## Resumen

UFTC ("ultra fast tile compression") es un formato de compresión de gráficos para homebrew de Mega Drive. Su ratio de compresión no es excepcional (alrededor del 50% usado como está pensado), pero su punto fuerte es que puede descomprimirse sobre la marcha con razonable rapidez.

## Detalle técnico

Según la página: "su ratio de compresión no es tan bueno (alrededor del 50% si se usa como está pensado), pero su atractivo principal es que puede descomprimirse sobre la marcha con razonable rapidez."

### Mecanismo principal

El formato emplea compresión basada en diccionario dividiendo cada tile en bloques de 4×4 píxeles. La estructura contiene:

1. **Contador de bloques**: valor de 16 bits que indica el número de bloques únicos (máximo 8192)
2. **Diccionario de bloques**: cada bloque de 4×4 ocupa 8 bytes, con "cada palabra de 16 bits representando 4 píxeles (a 4 bits por píxel)"
3. **Lista de índices de tiles**: cada tile de 8×8 se almacena como cuatro índices de 16 bits (arriba-izquierda, arriba-derecha, abajo-izquierda, abajo-derecha)

### Variantes

**UFTC16** (estándar actual): usa offsets sin signo, soporta hasta 8192 bloques únicos a velocidad de descompresión completa.

**UFTC15** (legado): usaba offsets de palabra con signo, limitando la capacidad a 4096 bloques. La página señala que "el descompresor más nuevo puede leer sin problemas datos comprimidos para la versión anterior."

### Características clave

- Soporta descompresión de acceso aleatorio sin necesidad de procesar todo el flujo
- La compresión óptima se logra procesando hojas de sprites completas juntas, maximizando la reutilización de bloques

## Notas / gotchas

- El código fuente (herramienta de compresión, rutinas de descompresión en ensamblador y C) está disponible bajo licencia zlib a través del repositorio de GitHub de mdtools.

## Referencias cruzadas

- (pendiente de enlazar)

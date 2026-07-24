---
titulo: Formatos de compresión clásicos de Sega — Nemesis, Kosinski, Enigma
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [compresion, nemesis, kosinski, enigma, saxman, sega, referencia-indirecta]
estado: borrador
fecha: 2026-07-21
fuentes: [https://s3unlocked.blogspot.com/2017/06/mega-drive-compression-formats.html, https://clownacy.wordpress.com/2024/02/12/making-a-nemesis-compressor/, https://segaretro.org/Enigma_compression, https://github.com/sonicretro/KENSSharp]
---

# Formatos de compresión clásicos de Sega — Nemesis, Kosinski, Enigma

## Resumen

Sega usó internamente (y en la práctica totalidad de sus juegos de Mega Drive, incluida la saga Sonic) tres esquemas de compresión distintos para ahorrar espacio en el cartucho: **Kosinski**, **Nemesis** y **Enigma**, cada uno especializado en un tipo de dato distinto (tiles gráficos comprimidos rápido para VRAM, tiles con muchas repeticiones de nibble, y mapeos de plano respectivamente). Son formatos mucho más extendidos en juegos comerciales reales que los formatos SLZ/UFTC ya documentados en esta base (fuente plutiedev.com, más orientados a homebrew moderno). La herramienta de referencia para trabajar con ellos hoy es [KENS/KENSSharp](https://github.com/sonicretro/KENSSharp).

## Detalle técnico

### Kosinski

Formato de propósito general basado en el algoritmo **LZSS**: reduce los datos mediante codificación de diccionario, creando un diccionario de patrones frecuentes y sustituyendo sus apariciones por referencias a la entrada del diccionario. Tiene una tasa de compresión alta, y puede descomprimirse directamente a VRAM — pero su velocidad de descompresión es lenta, lo que limita su uso a solo unos pocos tiles por frame (no apto para streaming masivo en tiempo real). Recibe su nombre de **Brett Kosinski**, quien lo diseñó/reverse-engineered originalmente.

### Nemesis

Esquema de **run-length encoding basado en nibbles** (medio byte), donde cada "run" (secuencia repetida) se coloca en una especie de diccionario que lo sustituye por un código de longitud variable. Es un formato basado en entropía, pero tarda bastante en descomprimirse. Funciona mejor con archivos que tienen largas secuencias del mismo nibble repetido. Recibe su nombre del usuario **Nemesis**, que fue quien descifró el formato (y también el de Enigma).

### Enigma

Variante de run-length encoding, usada principalmente para comprimir **mapeados de plano** (plane mappings), que normalmente contienen largas secuencias de words repetidas. Se usa sobre todo para comprimir mapeados de fondo estáticos y grandes, como pantallas de menú o la pantalla de Special Stage.

### Saxman

Mencionado como otro formato soportado por KENSSharp junto a los tres anteriores — no se ha encontrado detalle técnico propio en las fuentes consultadas; pendiente de documentar con más profundidad.

## Notas / gotchas

- No ha sido posible acceder directamente a `segaretro.org` ni a la wiki de Sonic Retro (ambas devuelven 403) para contrastar el detalle técnico exacto (formato de bits, estructura de la tabla de códigos) — este resumen se ha construido combinando varias fuentes secundarias independientes (blog de Sonic 3 Unlocked, blog de Clownacy sobre implementar un compresor Nemesis, resultados de búsqueda de Sega Retro), que coinciden entre sí, pero conviene contrastar con la implementación real de [KENSSharp](https://github.com/sonicretro/KENSSharp) antes de escribir un (de)compresor propio.
- Elegir el formato según el tipo de dato y el momento de descompresión: Kosinski para gráficos que se descomprimen a VRAM con presupuesto de tiempo ajustado, Nemesis para tiles con muchas repeticiones donde el tiempo de descompresión importa menos, Enigma específicamente para mapeados de plano.

## Referencias cruzadas

- [Disassemblies de Sonic Retro](sonic-retro-disassemblies.md)
- [Formatos de compresión SLZ/UFTC](format-slz.md), [format-uftc.md](format-uftc.md) (fuente plutiedev.com)
- [`../graficos/tiles-and-palettes.md`](../graficos/tiles-and-palettes.md)

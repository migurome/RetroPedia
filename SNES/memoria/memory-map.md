---
titulo: Mapa de memoria (LoROM / HiROM / ExHiROM)
plataforma: [SNES]
categoria: [memoria]
tags: [mapa-memoria, lorom, hirom, exhirom, bancos, direccionamiento]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Memory_map]
---

# Mapa de memoria (LoROM / HiROM / ExHiROM)

## Resumen

El 65C816 de la SNES direcciona memoria mediante bancos de 64 KiB (dirección de 24 bits: banco de 8 bits + offset de 16 bits). Cómo se reparte la ROM del cartucho dentro de ese espacio depende del modo de mapeo elegido por el diseño de la placa: LoROM, HiROM o ExHiROM. El modo de mapeo determina en qué bancos aparece la ROM, cómo se conectan físicamente las líneas de dirección del cartucho a las del bus del 65C816, y dónde cae la cabecera de la ROM.

## Detalle técnico

### Visión general

La consola ofrece acceso nativo a RAM y a los registros de E/S en zonas concretas del espacio de direcciones; el resto lo ocupa el cartucho. No todas las zonas responden a la misma velocidad de bus. Los primeros 8 KiB de RAM de trabajo (WRAM) se reflejan (mirror) en muchos bancos para poder acceder a ellos cómodamente desde cualquier banco de "sistema", mientras que los bancos `$7E`-`$7F` dan acceso lineal y continuo a los 128 KiB completos de WRAM.

La cabecera de la ROM siempre termina apareciendo, en el mapa de memoria del CPU, en `$00FFC0`, independientemente del modo de mapeo — lo que cambia es el offset físico dentro del *archivo* de ROM donde hay que colocarla.

### LoROM

- **Tamaño de banco:** 32 KiB (la ROM ocupa solo la mitad superior, `$8000`-`$FFFF`, de cada banco de 64 KiB direccionable).
- **Conexión de las líneas de dirección del cartucho:**

| Línea del bus SNES | Línea del cartucho |
|---|---|
| A0-A14 | A0-A14 (conexión directa) |
| A15 | no conectada |
| A16-A22 | A15-A21 |
| A23 | no conectada |

- Los juegos LoROM pueden llegar hasta 4 MiB de tamaño.
- Los mirrors de RAM y de los registros de E/S solo están presentes en los bancos `$00`-`$3F` y `$80`-`$BF` (la mitad baja de esos bancos); los bancos `$40`-`$7D` y `$C0`-`$FF` normalmente solo contienen ROM, y su mitad baja no usada suele ser un espejo de la mitad alta del mismo banco.
- La cabecera reside en el offset `$007FC0` del archivo de ROM, que queda mapeado en `$00FFC0` en el espacio de direcciones del CPU.

### HiROM

- **Tamaño de banco:** 64 KiB completos por banco.
- **Conexión de las líneas de dirección del cartucho:**

| Línea del bus SNES | Línea del cartucho |
|---|---|
| A0-A21 | A0-A21 (mapeo 1:1) |
| A22 | no conectada |
| A23 | no conectada |

- Al ser un mapeo 1:1, los datos que cruzan un límite de banco no requieren trato especial en el código — simplifica el acceso a bloques de datos grandes y contiguos.
- HiROM permite una vista lineal y continua de toda la ROM en los bancos `$C0`-`$FF`.
- Los bancos `$00`-`$3F`/`$80`-`$BF` exponen, en su mitad alta (`$8000`-`$FFFF`), la misma imagen que ocupa la última mitad de 32 KiB de cada banco correspondiente en la zona `$C0`-`$FF`; la mitad baja no usada de los bancos `$40`-`$7D` normalmente es un espejo de `$C0`-`$FD`.
- La cabecera está en `$00FFC0` tanto en el archivo de ROM como en el espacio de direcciones del CPU (misma posición en ambos).

### ExHiROM

Modo pensado para superar el límite de 4 MiB de HiROM/LoROM normal:

- Los bancos `$80`-`$FF` apuntan a los primeros 4 MiB del archivo de ROM, igual que en un mapeo normal.
- Los bancos `$00`-`$7D` pueden apuntar a hasta 4 MiB *adicionales* de ROM (la segunda mitad del archivo).
- La cabecera reside en el offset `$40FFC0` del archivo de ROM, y queda igualmente mapeada en `$00FFC0` en el espacio de direcciones del CPU.

### Estructura de la página fuente

La página de la wiki, tal como está redactada, se limita a estas secciones: *Overall* (visión general de WRAM y ubicación de cabecera), *LoROM*, *HiROM* y *ExHiROM* — cada una con un diagrama de bancos y su tabla de conexión de pines de dirección. No incluye (en esta página) tablas detalladas de sub-rangos dentro de un banco (por ejemplo qué ocupa exactamente `$2000`-`$5FFF` o `$6000`-`$7FFF`), ni secciones dedicadas a SA-1, ExLoROM, SPC7110 o Super FX/GSU — esos modos de mapeo especiales, si están documentados en la wiki, viven en páginas propias no cubiertas por esta entrada.

## Notas / gotchas

- El modo de mapeo (LoROM/HiROM/ExHiROM/etc.) se declara explícitamente en el byte de "ROM Speed/Map Mode" de la cabecera (ver entrada `rom-header`); el hardware real no lo detecta automáticamente, es la propia placa/mapper del cartucho la que determina cómo caen las líneas de dirección.
- En LoROM, como cada banco solo aporta 32 KiB de ROM útil, el código que necesita bloques de datos grandes y contiguos (más de 32 KiB) debe gestionar manualmente el cruce de banco; en HiROM esto no hace falta gracias al mapeo 1:1.
- La cabecera SIEMPRE aparece en `$00FFC0` en el mapa de memoria del CPU sea cual sea el modo — solo cambia dónde hay que colocarla físicamente dentro del archivo `.sfc`/`.smc` sin cabecera de copiadora (ver `formatos-archivo-rom`).
- Esta página de la wiki no cubre en detalle la zona de registros de hardware (`$2100`-`$21FF`, `$4200`-`$421F`, `$4300`-`$437F`, etc.); para eso ver las entradas específicas de `mmio-registers` y `dma-registers`.

## Referencias cruzadas

- (pendiente de enlazar)

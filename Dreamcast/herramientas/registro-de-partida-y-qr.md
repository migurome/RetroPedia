---
titulo: Registro de partida — reproducir en PC lo que pasó en la consola, con la VMU o códigos QR
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [depuracion, replay, determinismo, qr, base45, vmu, telemetria, gdemu, flycast]
estado: revisado
fecha: 2026-10-31
fuentes: [ISO/IEC 18004 (QR), RFC 9285 (base45), implementación de referencia de Project Nayuki, zxing-cpp, proyecto hyperdbz-dreamcast-port (dc/proto/src/replay.c, qr.c, tools/replay.py)]
---

# Registro de partida

## Resumen

Para depurar lo que pasa en la consola sin cable serie (GDEMU no tiene, y no se puede escribir en su
tarjeta), la herramienta más rentable es un **registro de partida**. Si la lógica del juego es
**determinista** (mismo código, mismos datos, misma semilla y mismas entradas dan el mismo resultado en el PC y
en la consola, ver [coma flotante determinista](../cpu/coma-flotante-determinista.md)), no hace falta guardar
el estado: basta con guardar **lo que entra** (semilla y mando) y algunas comprobaciones. Un arnés de PC con el
mismo código reproduce la partida y enseña el estado de cualquier tick.

El registro sale de la consola de dos formas, ambas probadas en Flycast:
- **Partida guardada en la VMU.** En el emulador es lo más cómodo: la VMU es un archivo del PC.
- **Códigos QR en pantalla**, para una foto con el móvil. Es el camino para la consola real con GDEMU.

## Detalle técnico

### Qué se guarda

| Dato | Para qué | Tamaño |
|---|---|---|
| Cabecera: versión, personajes, escenario, semilla, IA, crc32 de los datos de lógica | Reproducir con lo mismo; avisar si los datos del PC no son los del disco | ~50 bytes |
| Entrada de cada jugador humano, solo al cambiar (ticks desde el anterior + valor, varints) | Reproducir | ~3 bytes por cambio |
| Cada 300 ticks: huella encadenada del estado + estado, vida, x, y y helpers de cada jugador | Saber **en qué tramo** se separa una reproducción y cómo estaba cada uno | ~20 bytes |
| Cambios de asalto | Leer el registro sin reproducirlo | pocos |
| Rendimiento de cada segundo (solo en la consola): CPU media y máxima, vblanks perdidos, fallos de caché... | Telemetría para después | ~15 bytes/s |

Un combate de 90 s ocupa ~2 KB, 1,5 KB con zlib (que en KOS está en kos-ports). Cada registro empieza por
los ticks desde el anterior y un byte de tipo, así que el formato crece sin romper nada.

Grabar en la consola y en el arnés **con el mismo código** permite comparar los dos registros registro a
registro. Lo que no sea igual (salvo la telemetría) es una diferencia de simulación.

### QR desde la consola

- **Modo alfanumérico + base45** (RFC 9285, el de los certificados COVID de la UE): cada 2 bytes son 3
  caracteres del juego alfanumérico del QR, que se codifica a 5,5 bits por carácter. Se pierde solo un 3 %
  frente al modo binario, y el QR es texto puro que cualquier lector entrega sin estropear.
- **Tamaño:** versión 21 (101 módulos) con corrección M caben 1.035 caracteres = 690 bytes por QR. Con
  4 píxeles por módulo y el margen de 4 módulos ocupa 436 píxeles de alto en 640×480. A partir de ahí, varias
  páginas, cada una con una cabecera: `'H'`, el id del registro (crc32 de lo comprimido), número de página y
  total.
- **Codificador propio en C** (~300 líneas, sin dependencias ni memoria dinámica): bits de datos, relleno
  `0xEC 0x11`, Reed-Solomon en GF(256) con el polinomio 0x11D, bloques intercalados, patrones fijos, zigzag
  y la máscara con menos penalización (las cuatro reglas de la norma). Se comprobó generando **los 160 QR**
  (versiones 1-40 × niveles L/M/Q/H, llenos al máximo) y leyéndolos con zxing-cpp: así se validan a la vez
  todas las tablas de bloques.
- **Pintar:** con la PVR apagada (`pvr_shutdown()` + `vid_set_mode(DM_640x480, PM_RGB565)`), rectángulos
  en `vram_s` **con escrituras de 16 bits** (ver la trampa del `memset` en el
  [mapa de memoria](../memoria/mapa-de-memoria.md)).
- **Leer en el PC:** `zxingcpp.read_barcodes(imagen)` (`py -m pip install zxing-cpp`) encuentra los QR de una
  foto o captura. Las páginas se juntan por id; si falta alguna, se dice cuál.

Tolerancia medida con capturas de Flycast degradadas a propósito (imagen de 800×600):

| Degradación | Módulo de 4 px (versión 21) | Módulo de 6 px (versión 11) |
|---|---|---|
| Desenfoque gaussiano hasta 2 px | se lee | se lee |
| Desenfoque de 2,5 a 3,5 px | **falla** | se lee |
| Mitad de resolución, JPEG calidad 30, perspectiva fuerte, rotación de 7°, reflejo | se lee | se lee |

Por si la imagen llega borrosa (vídeo compuesto), conviene ofrecer un modo de QR más grandes: los mismos
datos en el doble de páginas.

### Partida guardada en la VMU

`vmu_pkg_build` + `vmufs_write(dev, "NOMBRE", buf, tam, VMUFS_OVERWRITE)` (ver
[Maple, mando y VMU](../perifericos/maple-mando-y-vmu.md)). Un registro de 1,5 KB más la cabecera VMS y un
icono son 5 bloques; en Flycast tarda ~835 ms. Flycast guarda la VMU como imagen de 128 KB en
`data/<Game ID>_vmu_save_A1.bin`, que se lee desde el PC con unas pocas líneas.

## Notas / gotchas

- **Escribir en la VMU cambió las pruebas de rendimiento que venían después** (el peor tick de una de ellas,
  de 18,7 a 21,2 ms, igual en todas las ejecuciones). La causa era el estado en que queda la memoria, no la
  colocación del código: con el código presente pero sin la llamada volvió a 18,7. Las escrituras de este
  tipo van al final o fuera de lo que se mide.
- Un test de «la reproducción detecta una diferencia» tiene que cambiar algo que importe: cambiar un botón
  unos ticks cuando el personaje no tiene control no altera la partida, y la reproducción sale idéntica con
  razón.
- El registro no cubre un cuelgue: si la consola se cuelga, no se guarda nada. Para eso, el buzón o la
  pantalla de resultados.

## Referencias cruzadas

- [Flycast automatizado](flycast-automatizado.md) — buzón, entrada programada, capturas y la VMU del emulador
- [Maple, mando y VMU](../perifericos/maple-mando-y-vmu.md) — formato de la imagen de VMU y de la cabecera VMS
- [Coma flotante determinista](../cpu/coma-flotante-determinista.md)

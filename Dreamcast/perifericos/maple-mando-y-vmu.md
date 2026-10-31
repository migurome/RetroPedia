---
titulo: Bus Maple, mando y VMU — protocolo, lectura del mando con KOS y partidas guardadas
plataforma: [Dreamcast]
categoria: [perifericos]
tags: [maple, mando, controller, vmu, vms, partidas, vmu_pkg, puru-puru, input]
estado: borrador
fecha: 2026-10-30
fuentes: [https://mc.pp.se/dc/maplebus.html, https://mc.pp.se/dc/controller.html, https://mc.pp.se/dc/sw.html, https://www.dreamcast.wiki/Save/Load_file, cabeceras de KOS (dc/maple.h, dc/maple/controller.h, dc/vmu_pkg.h)]
---

# Bus Maple, mando y VMU

## Resumen

Todos los periféricos de la Dreamcast (mandos, VMU, *rumble*, teclado, ratón, pistolas) cuelgan del **bus Maple**: 4 puertos serie bidireccionales a 2 Mbit/s, en cada uno de los cuales hay un periférico principal y hasta 5 subperiféricos (las ranuras del mando). KOS lo gestiona solo: sondea los dispositivos por DMA cada frame y expone su estado ya decodificado. Esta entrada da el protocolo (para entender lo que hace KOS o depurar) y la forma práctica de leer el mando y guardar en la VMU.

## Detalle técnico

### Trama Maple

Una trama son palabras de 32 bits. La primera es la cabecera:

| Bits | Campo |
|---|---|
| 31–24 | comando o respuesta |
| 23–16 | dirección del destinatario |
| 15–8 | dirección del emisor |
| 7–0 | número de palabras adicionales |

Dirección de 8 bits: los bits 7–6 son el puerto (0 = A … 3 = D), el bit 5 el periférico principal y los bits 4–0 los subperiféricos 5…1. Con los bits 0–5 a cero, la dirección es la de la propia consola en ese puerto.

| Código | Comando | Respuesta esperada |
|---|---|---|
| 1 | pedir información del dispositivo | 5 (*device info*) |
| 2 | información extendida | 6 |
| 3 / 4 | reset / apagar | 7 (ACK) |
| 9 | *get condition* (estado: botones...) | 8 (datos) |
| 10 | información de memoria | 8 |
| 11 / 12 | leer / escribir bloque | 8 / 7 |
| 14 | *set condition* (p. ej. vibración) | 7 |
| -1 … -5 | sin respuesta, función no soportada, comando desconocido, reenviar, error de fichero | — |

Códigos de función (un dispositivo puede tener varios a la vez): `0x001` mando, `0x002` tarjeta de memoria, `0x004` LCD, `0x008` reloj, `0x010` micrófono, `0x020` AR-gun, `0x040` teclado, `0x080` pistola, `0x100` Puru Puru (vibración), `0x200` ratón. Una VMU es memoria + LCD + reloj. KOS los nombra `MAPLE_FUNC_CONTROLLER`, `MAPLE_FUNC_MEMCARD`, etc.

El DMA de Maple se controla con `0xA05F6C04` (puntero al buffer de descriptores, alineado a 32 bytes), `0xA05F6C18` (activar) y `0xA05F6C80` (*timeout* y velocidad). Si un dispositivo no responde, el hardware escribe `0xFFFFFFFF` en la dirección de resultado.

### El mando en crudo y en KOS

En el protocolo, el estado del mando son 16 bits de botones (**0 = pulsado**), dos gatillos de 0 a 255 y dos sticks analógicos de 0 a 255 (128 = centro). Orden de bits: C, B, A, START, arriba, abajo, izquierda, derecha, Z, Y, X, D y la segunda cruceta.

**KOS invierte y recentra estos valores**: en `cont_state_t`, un bit a **1 es pulsado**, `joyx`/`joyy` van de **-128 a 127 con 0 en el centro** y `ltrig`/`rtrig` de 0 a 255. Los bits siguen el mismo orden (`CONT_C`, `CONT_B`, `CONT_A`, `CONT_START`, `CONT_DPAD_UP`...).

```c
#include <kos.h>

/* Primer mando conectado (no tiene por qué estar en el puerto A) */
maple_device_t *dev = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
if(dev) {
    cont_state_t *st = (cont_state_t *)maple_dev_status(dev);
    if(st->buttons & CONT_A)  { /* A pulsado */ }
    if(st->dpad_left)         { /* también hay campos de bits */ }
    int x = st->joyx;         /* -128..127 */
}

/* O recorrer todos los mandos: */
MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
    if(st->buttons & CONT_START) salir = 1;
MAPLE_FOREACH_END()
```

- `maple_dev_status` devuelve el estado de la **última sonda**: leerlo dos veces en el mismo frame da lo mismo. Para detectar "recién pulsado" hay que guardar el estado del frame anterior.
- El mando no tiene botones C, D, Z ni segunda cruceta, aunque existan los bits. Las capacidades reales están en la información del dispositivo. Los mandos arcade y *twin-stick* sí usan más bits.
- Ejemplos de KOS: `examples/dreamcast/maple`, `rumble` (vibración con comandos en crudo), `keyboard`, `lightgun`.

### Guardar en la VMU

KOS monta las VMU como sistema de ficheros en `/vmu/<puerto><ranura>`: `/vmu/a1` es la primera ranura del mando del puerto A. Un fichero de partida válido para el gestor de la BIOS lleva una cabecera con descripciones, el id de la aplicación y un icono, que se construye con `vmu_pkg_t`:

```c
vmu_pkg_t pkg = {0};
strcpy(pkg.desc_short, "MI JUEGO");          /* 20 bytes */
strcpy(pkg.desc_long,  "Partida guardada");  /* 36 bytes */
strcpy(pkg.app_id,     "MIJUEGO");           /* 20 bytes */
pkg.icon_cnt = 1;
pkg.icon_anim_speed = 0;
memcpy(pkg.icon_pal, icono_paleta, 32);      /* 16 colores ARGB4444 */
pkg.icon_data = icono_pixeles;               /* 512 bytes por icono: 32x32 a 4 bpp */
pkg.eyecatch_type = VMUPKG_EC_NONE;
pkg.data = datos; pkg.data_len = tam;

uint8_t *out; int out_size;
vmu_pkg_build(&pkg, &out, &out_size);
fs_unlink("/vmu/a1/MIJUEGO.SAV");
file_t f = fs_open("/vmu/a1/MIJUEGO.SAV", O_WRONLY);
fs_write(f, out, out_size);
fs_close(f);
free(out);
```

Para leer, se carga el fichero entero y se llama a `vmu_pkg_parse(buf, tam, &pkg)`. `pkg.data` apunta **dentro de** `buf`, así que no hay que liberar `buf` mientras se use. El ejemplo de la wiki comprime los datos con zlib antes de empaquetarlos: una VMU tiene solo ~200 bloques de 512 bytes libres.

Gotchas del ejemplo de la wiki: no comprueba ningún valor de retorno, borra la partida antigua **antes** de escribir la nueva (si la escritura falla, no queda ninguna) y descomprime a un buffer fijo de 64 KB. En un juego real conviene escribir en un fichero temporal y renombrar, o al menos comprobar `fs_write`.

También se puede escribir un fichero de una vez con `vmufs_write(dev, "NOMBRE", buf, tam, VMUFS_OVERWRITE)`
(`dc/vmufs.h`, `dev` de `maple_enum_type(0, MAPLE_FUNC_MEMCARD)`); el nombre tiene hasta 12 caracteres. En
Flycast, 5 bloques tardaron ~835 ms.

### Leer la VMU desde el PC

Una VMU es una flash de 128 KB: 256 bloques de 512 bytes, en little endian. Flycast la guarda tal cual en un
archivo, `data/<Game ID>_vmu_save_A1.bin` si `PerGameVmu = yes` (si no, `data/vmu_save_A1.bin`). El Game ID
es el número de producto del IP.BIN. Para leer un fichero:

| Dónde | Qué |
|---|---|
| Bloque 255 (raíz), `+0x46` / `+0x48` | bloque y tamaño de la FAT (normalmente 254, 1) |
| Raíz, `+0x4A` / `+0x4C` | último bloque y tamaño del directorio (normalmente 253, 13): se recorre **hacia abajo** |
| FAT | un `uint16` por bloque: el siguiente del fichero (`0xFFFA` fin, `0xFFFC` libre) |
| Entrada de directorio (32 bytes) | `+0` tipo (`0x33` datos, `0xCC` juego de VMU, 0 vacío), `+2` primer bloque, `+4` nombre (12 bytes), `+0x10` fecha BCD, `+0x18` tamaño en bloques, `+0x1A` bloque de la cabecera (juegos) |

El fichero de datos empieza por la cabecera VMS que construye `vmu_pkg_build`:

| Desplazamiento | Campo |
|---|---|
| `0x00` / `0x10` / `0x30` | descripción corta (16), larga (32), id de la aplicación (16) |
| `0x40` / `0x42` / `0x44` | iconos, velocidad de la animación, tipo de *eyecatch* (`uint16`) |
| `0x46` / `0x48` | CRC (`uint16`), bytes de datos (`uint32`) |
| `0x60` | paleta del icono (16 × ARGB4444) |
| `0x80` | iconos (512 bytes cada uno), *eyecatch* (0, 8064, 4544 o 2048 bytes según el tipo) y los datos |

Así se lee en Python el registro de partida de [este proyecto](../herramientas/registro-de-partida-y-qr.md)
(`tools/replay.py`, funciones `vmu_files` y `vms_payload`).

### Programar la propia VMU

La VMU tiene su propia CPU (Sanyo LC86K, 8 bits) y puede ejecutar minijuegos. Recursos: la documentación de Marcus Comstedt (firmware, flash de 128 KB, cabecera de fichero, iconos `ICONDATA_VMS`), el ensamblador `aslc86k` y el emulador `softvms`, más los de [awesome-dreamcast](https://github.com/dreamcastdevs/awesome-dreamcast) (Waterbear, libevmu, tutoriales de Candle-Electric). Desde la Dreamcast, KOS permite dibujar en la pantalla de la VMU (`dc/vmu_fb.h`, ejemplo `vmu/vmu_lcd`) y hacerla pitar (`vmu_beep_raw`).

## Notas / gotchas

- `maple_enum_type(0, ...)` da el **primer mando encontrado**, no el del puerto A. Para un juego de 2 jugadores con puertos fijos, hay que buscar por puerto (`maple_enum_dev(puerto, unidad)`).
- El conector del mando: 1 datos, 2 +5 V, 3 GND, 4 *sense* (a GND dentro del mando), 5 datos.

## Referencias cruzadas

- [Mapa de memoria](../memoria/mapa-de-memoria.md) — registros en `0xA05F....`
- [Bibliotecas, motores y recursos](../herramientas/bibliotecas-motores-y-recursos.md) — herramientas de VMU

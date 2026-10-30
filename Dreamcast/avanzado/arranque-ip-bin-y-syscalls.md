---
titulo: Arranque — estructura de IP.BIN, carga de 1ST_READ.BIN y syscalls de la BIOS
plataforma: [Dreamcast]
categoria: [avanzado]
tags: [arranque, ip.bin, 1st_read.bin, bootstrap, scramble, region, syscalls, bios, gdrom, flashrom, romfont]
estado: borrador
fecha: 2026-10-30
fuentes: [https://mc.pp.se/dc/ip.bin.html, https://mc.pp.se/dc/sw.html, https://mc.pp.se/dc/syscalls.html]
---

# Arranque: IP.BIN, 1ST_READ.BIN y syscalls

## Resumen

La BIOS lee `IP.BIN` de los 16 primeros sectores de la última pista de datos y lo carga en `0x8C008000`. Ejecuta el código de la pantalla de licencia de SEGA y luego dos *bootstraps*, y salta al ejecutable indicado (normalmente `1ST_READ.BIN`), que se carga en `0x8C010000`. La entrada [imagen CDI](../herramientas/imagen-cdi.md) explica cómo construir el disco; esta explica qué hay dentro de `IP.BIN` y qué servicios deja la BIOS en RAM.

## Detalle técnico

### Estructura de IP.BIN (32 KB)

| Offset | Dirección de carga | Tamaño | Contenido |
|---|---|---|---|
| `0x0000` | `0x8C008000` | 256 B | metainformación (= `IP0000.BIN` de un GD; campos en [imagen CDI](../herramientas/imagen-cdi.md)) |
| `0x0100` | `0x8C008100` | 512 B | tabla de contenidos. Su uso es desconocido y vale con ceros |
| `0x0300` | `0x8C008300` | 13.312 B | código de la pantalla de licencia de SEGA |
| `0x3700` | `0x8C00B700` | 256 B | símbolos de protección de región |
| `0x3800` | `0x8C00B800` | 10.240 B | Bootstrap 1 |
| `0x6000` | `0x8C00E000` | 8.192 B | Bootstrap 2 |

- **Pantalla de licencia**: muestra el logo unos 6 s. La BIOS **la compara byte a byte con una copia en ROM** y no arranca si difiere: no se puede tocar.
- **Regiones**: 8 huecos, cada uno precedido de una instrucción de salto que lo esquiva. El hueco 0 es Japón, el 1 USA y el 2 Europa; los demás no tienen uso. El disco arranca en una región si su hueco contiene el nombre del área. Las plantillas libres traen las tres, y la región se elige en el campo "Áreas" de la metainformación (`JUE`).
- **Bootstrap 1**: modificable. Por defecto configura unos cuantos registros y pasa al 2.
- **Bootstrap 2**: modificable. Por defecto prepara la pila y el VBR, **desactiva la caché** y salta a `0x8C010000`. KOS vuelve a activarla al iniciarse.
- El checksum de *Device info* no hace falta que sea correcto para arrancar (`0000` vale). Aun así, las herramientas lo calculan, ver el CRC en [imagen CDI](../herramientas/imagen-cdi.md).

### 1ST_READ.BIN

- Binario plano sin cabeceras, enlazado para `0x8C010000`. A partir de un ELF: `objcopy -O binary`.
- En un **CD normal** (no GD-ROM) la BIOS lo carga desordenado, en un orden pseudoaleatorio, así que hay que pasarlo por `scramble` (con `-d` deshace el desordenado). En un GD no se desordena.
- Ejemplo mínimo sin KOS, de Marcus Comstedt:

```sh
sh-elf-gcc -ml -Wl,-Ttext,0x8c010000 -nostartfiles -nostdlib -e _start prog.c -o prog.elf -lgcc
sh-elf-objcopy -O binary prog.elf prog.bin
scramble prog.bin 1ST_READ.BIN
```

### Syscalls de la BIOS

La BIOS deja sus rutinas en RAM, en `0x8C000000`–`0x8C007FFF`. Se llaman a través de un **vector**: se lee un puntero de una dirección fija y se salta a él. En general `r7` elige la función y `r6` la "superfunción". Los argumentos van en `r4`–`r6`, y ROMFONT usa `r1` en lugar de `r7`.

| Vector | Grupo | Funciones |
|---|---|---|
| `0x8C0000B0` | SYSINFO | `r7=0` INIT (copia datos de la flash a `0x8C000068`–`7F`, llamar primero). `r7=2` ICON (lee el icono `r4` 0–9 a un buffer de 704 B). `r7=3` ID (puntero al **ID de 64 bits** de la consola) |
| `0x8C0000B4` | ROMFONT | `r1=0` dirección de la fuente de la ROM. `r1=1` LOCK (mutex, -1 si está ocupada). `r1=2` UNLOCK |
| `0x8C0000B8` | FLASHROM | `r7=0` INFO (inicio y tamaño de la partición `r4` 0–4). `r7=1` READ. `r7=2` WRITE (**solo pasa bits de 1 a 0**). `r7=3` DELETE (pone toda la partición a unos) |
| `0x8C0000BC` | MISC (`r6=-1`) | `r7=0` INIT (restaura vectores). `r7=1` SETVECTOR (instala una superfunción 0–7) |
| `0x8C0000BC` | GDROM (`r6=0`) | `r7=0` SEND_COMMAND (devuelve un id de petición). `1` CHECK_COMMAND (0 no existe, 1 en curso, 2 terminada, -1 error). `2` MAINLOOP (llamar periódicamente). `3` INIT. `4` CHECK_DRIVE (estado y tipo de disco). `8` ABORT_COMMAND. `9` RESET. `10` SECTOR_MODE |

Las superfunciones `r6=1`–`7` quedan para el usuario.

## Notas / gotchas

- Con KOS no hace falta llamar a estas syscalls a mano: `cdrom_*`, `flashrom_*` y la fuente BIOS (`bfont`) las envuelven. Conocerlas sirve para depurar o para programas sin KOS.
- Como la flash solo admite pasar bits de 1 a 0, una "sobrescritura" exige borrar la partición entera.
- Que el *checksum* de IP.BIN no se valide significa que un disco con un IP.BIN algo incorrecto puede arrancar en una consola y fallar en otra herramienta, como GDEMU o un emulador, que sí lo compruebe. Esto es una hipótesis sin comprobar: mejor generarlo bien.

## Referencias cruzadas

- [Imagen de disco autoarrancable (CDI)](../herramientas/imagen-cdi.md) — campos de la metainformación y CRC
- [Mapa de memoria](../memoria/mapa-de-memoria.md)

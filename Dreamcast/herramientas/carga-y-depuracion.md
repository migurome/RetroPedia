---
titulo: Carga en consola y depuración — dcload-ip/serial, dc-tool, GDB y emuladores
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [dcload-ip, dcload-serial, dc-tool, bba, coders-cable, gdb, emuladores, flycast, depuracion]
estado: borrador
fecha: 2026-10-30
fuentes: [https://www.dreamcast.wiki/Getting_Started_with_Dreamcast_development, https://www.dreamcast.wiki/Dcload-ip, https://github.com/sizious/dcload-ip, https://www.dreamcast.wiki/Romdisk_Swapping, https://www.dreamcast.wiki/Emulators, cabecera arch/gdb.h de KOS]
---

# Carga en consola y depuración

## Resumen

Para probar en una Dreamcast real sin grabar un CD en cada cambio se usa **dcload**: un programa que se arranca desde un CD-R y queda esperando ejecutables enviados desde el PC con **dc-tool**. Hay dos variantes según el enlace físico: **dcload-ip** (Broadband Adapter o LAN Adapter) y **dcload-serial** (*coder's cable*). Además de cargar el programa, dcload redirige su consola al PC, le da acceso a los ficheros del PC (`/pc`) y sirve de puente para GDB.

## Detalle técnico

### Enlaces de depuración

| Hardware | Programa | Velocidad máxima |
|---|---|---|
| Broadband Adapter (HIT-400/401) | dcload-ip | 100 Mbit/s (recomendado) |
| LAN Adapter (HIT-300) | dcload-ip | 10 Mbit/s |
| USB coder's cable | dcload-serial | 1.500 kbit/s |
| Serial coder's cable | dcload-serial | 120 kbit/s |
| Módem + DreamPi | — | no sirve para depurar (solo juego online) |
| Adaptador SD por el puerto serie | — | carga programas, pero sin enlace la depuración es mucho más difícil |

dcload-ip lo creó Andrew Kieschnick y hoy lo mantienen los desarrolladores de KOS y SiZiOUS (`github.com/sizious/dcload-ip`, con mejoras). La wiki ofrece imágenes DiscJuggler de **dcload-ip 2.0.1 y dcload-serial 1.0.6** (22-06-2023). También se puede lanzar desde la BIOS con utilidades como DreamDash. Acepta `.elf` y `.bin` (y `.srec` si se configura).

### dc-tool

```sh
dc-tool-ip  -t <IP de la Dreamcast> -x programa.elf
dc-tool-ser -t <puerto serie>       -x programa.elf
dc-tool-ip  -t dreamcast -c /ruta/al/proyecto -x programa.elf   # -c: /pc apunta a esa carpeta
dc-tool-ip  -t <IP> -g -x programa.elf                          # -g: abre sesión GDB
```

- Sin argumentos, `dc-tool-ip` y `dc-tool-ser` listan todas sus opciones.
- `-x` envía y ejecuta. `-n` desactiva la consola (sin ella no llegan al PC los volcados de excepción).
- **`/pc`**: con `-c`, el programa puede abrir ficheros del PC como `/pc/nivel1.img`. Es la forma de iterar con recursos sin regenerar el disco. En el disco final hay que cambiar esas rutas a `/cd/`.
- `make run` en un proyecto KOS ejecuta `$(KOS_LOADER) $(TARGET)`, y `KOS_LOADER` vale `dc-tool -x` por defecto en `environ.sh`.

### GDB

1. Llamar a `gdb_init()` (cabecera `arch/gdb.h`) al principio del programa, por ejemplo en `main()`.
2. Compilar con `-g` (`kos-cc -g ...`).
3. `dc-tool -g -x programa.elf`.
4. En `sh-elf-gdb`: `target remote :2159`. El puerto siempre es 2159.

El `make` del toolchain también compila `sh-elf-gdb`. La wiki enlaza guías para depurar desde CLion y Visual Studio Code.

### Red de dcload-ip

- La IP de la Dreamcast se fija en `Makefile.cfg` al compilar dcload-ip. `0.0.0.0` activa DHCP, y la versión de 2023 tiene mejor soporte de DHCP.
- Alternativa: asignar la IP con `arp` desde el PC (en Windows es más fiable `netsh`), usando una IP `169.254.x.x`, porque el rango `0.x.x.x` está reservado al descubrimiento DHCP.
- Pantalla roja con "NO ETHERNET ADAPTER DETECTED!" indica que no se detecta el adaptador. Si se queda en "link change...", probablemente es el cable.
- `EXCEPTION_SECONDS` (en `Makefile.cfg`) controla cuánto tiempo se muestra la información de una excepción (15 s por defecto).

### Emuladores

La wiki avisa de que los emuladores sirven para iterar rápido pero **no son fiables al 100%**: hay que probar en consola. Su tabla está desactualizada (no menciona Flycast, el que usa este proyecto). Lo útil para un desarrollador:

| Emulador | Estado | Para desarrollo |
|---|---|---|
| **Flycast** | activo | el usado en [Flycast automatizado](flycast-automatizado.md): consola serie, scripting Lua |
| **Redream** | activo | sin sonido en homebrew si se usa su BIOS HLE |
| **WashingtonDC** | activo | `printf` por puerto serie y **GDB remoto** |
| **lxdream** / lxdream-nitro | inactivo / mejorado por la comunidad | `printf` por su consola. nitro no tiene acceso a CD de KOS con la BIOS HLE |
| **Deecy** | activo, "muy experimental" | — |
| **MAME** | activo | compatibilidad y velocidad limitadas |
| **Demul** | inactivo | emula MMU y Naomi 2 |
| Reicast | "extremadamente inestable", en reescritura | evitar |

## Notas / gotchas

- Una cifra de rendimiento medida en emulador no vale como medida de consola. Ver la advertencia de [Flycast automatizado](flycast-automatizado.md).
- La wiki tiene sin escribir las páginas de GDB con dcload y de `dcprof` (el *profiler* compatible con gprof de `gitlab.com/simulant/dcprof`).

## Referencias cruzadas

- [Toolchain de KallistiOS](toolchain-kos.md)
- [Flycast automatizado](flycast-automatizado.md)
- [Imagen de disco autoarrancable (CDI)](imagen-cdi.md) — para grabar el CD de dcload o el juego

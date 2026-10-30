---
titulo: DreamSDK R4 en Windows — instalación y problemas resueltos
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [dreamsdk, kallistios, kos-ports, msys2, gcc, zlib, windows]
estado: revisado
fecha: 2026-10-30
fuentes: [DreamSDK R4 build 4.0.11.2508, KallistiOS master (27-10-2026), proyecto hyperdbz-dreamcast-port]
---

# DreamSDK R4 en Windows

## Resumen

[DreamSDK](https://dreamsdk.org) empaqueta KallistiOS (KOS), el toolchain SH-4 (GCC 13.2) y el ARM del AICA para Windows, sobre MSYS2. Se instala en `C:\DreamSDK` con un instalador Inno Setup (pide administrador). Funciona, pero tiene varias trampas si se compila desde scripts o desde fuera de su propia consola. Todo lo de esta entrada se comprobó con DreamSDK R4 (build 4.0.11.2508) y KOS de master.

## Detalle técnico

### Compilar desde un script

El shell de login de DreamSDK (`/c/DreamSDK/usr/bin/bash.exe -l`) llena la salida de errores `DreamSDK Home directory was not found` si no existe la variable `DREAMSDK_HOME`. Lo más limpio es no usar el perfil de login y cargar el entorno de KOS a mano:

```sh
DREAMSDK_HOME='C:\DreamSDK' /c/DreamSDK/usr/bin/bash.exe --noprofile --norc -c '
export PATH=/usr/bin:/bin:/opt/toolchains/dc/bin:/opt/toolchains/dc/sh-elf/bin:$PATH
. /opt/toolchains/dc/kos/environ.sh >/dev/null 2>&1
export PATH=$KOS_BASE/utils/build_wrappers:$PATH      # ver abajo
export DREAMSDK_HOME="C:\\DreamSDK"                    # environ.sh la pierde
export TMP="C:\\DreamSDK\\tmp" TEMP="C:\\DreamSDK\\tmp" TMPDIR=/tmp
make -C /home/proyecto'
```

### Problemas y soluciones

| Síntoma | Causa | Solución |
|---|---|---|
| `make` falla con `Error 203` | `kos-cc.exe` exige `DREAMSDK_HOME` con ruta de Windows | Exportarla **después** de `environ.sh` |
| `Cannot create temporary file in C:\Windows\` | Sin `TMP`/`TEMP` el compilador escribe en `C:\Windows` | Apuntarlas a `C:\DreamSDK\tmp` |
| Un port de kos-ports no encuentra su propia cabecera (`zlib.h`) | Los `kos-*.exe` de DreamSDK son lanzadores que recargan el entorno y **pierden los `KOS_CFLAGS` que añaden los makefiles** (`-I.`) | Poner primero en el `PATH` los scripts originales: `$KOS_BASE/utils/build_wrappers` |
| `makeip.exe` no arranca (código 127 o se queda colgado) | Falta una DLL; desde un script puede salir un diálogo invisible | Generar `IP.BIN` con un script propio (ver [imagen CDI](imagen-cdi.md)) |
| `WARNING: Toolchain does not support m4-single ABI` | El GCC 13.2 de DreamSDK se compiló sin ese ABI | Inofensivo: usa `m4-single-only` |
| Herramientas que se quedan esperando | Algunas `.exe` leen la entrada estándar | Lanzarlas con `</dev/null` |

### Rutas con espacios

Los makefiles de KOS dan problemas con espacios en la ruta del proyecto. En lugar de renombrar la carpeta, se puede crear un *junction* sin espacios (no necesita administrador):

```powershell
New-Item -ItemType Junction -Path "C:\DreamSDK\home\proyecto" -Target "C:\Users\...\carpeta con espacios"
```

y compilar con `make -C /home/proyecto`. En la práctica, con rutas relativas en el Makefile, compiló bien incluso cuando MSYS resolvía el junction a la ruta real.

### kos-ports

DreamSDK trae el árbol de kos-ports **sin compilar** (no existe `kos-ports/lib`). Cada port se instala con:

```sh
cd /opt/toolchains/dc/kos-ports/zlib && make install clean
```

Descarga el código fuente (zlib desde zlib.net) y deja `libz.a` en `kos-ports/lib` y las cabeceras en `kos-ports/include/zlib/`. En el Makefile del juego: `KOS_CFLAGS += -I$(KOS_PORTS)/include/zlib` y enlazar con `-L$(KOS_PORTS)/lib -lz`.

### Otros detalles

- Si se lanza el shell de DreamSDK desde una carpeta, puede dejar ahí un `dreamsdk.conf`; conviene ignorarlo en git.
- `kos-cc --version` informa de GCC 13.2.0. Utilidades de KOS ya compiladas: `scramble.exe`, `wav2adpcm.exe`, `cdi4dc`, `mkisofs`.

## Notas / gotchas

- `DREAMSDK_HOME` vacía sale como una cascada de "Critical Exception" que no impide compilar con los wrappers originales, pero ensucia los logs.
- Los makefiles de KOS **no siguen dependencias de cabeceras**: si un `.h` generado cambia (versión, tablas), hay que borrar los `.o` que lo usan.

## Referencias cruzadas

- [Dreamcast — overview](../README.md)
- [Imagen de disco autoarrancable (CDI)](imagen-cdi.md)
- [ngdevkit en Windows (MSYS2)](../../NeoGeo/herramientas/ngdevkit-windows.md) — el mismo tipo de problemas con MSYS2

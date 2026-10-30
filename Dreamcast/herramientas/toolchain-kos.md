---
titulo: Toolchain de KallistiOS en Linux, macOS y WSL — dc-chain, environ.sh, Makefile y romdisk
plataforma: [Dreamcast]
categoria: [herramientas]
tags: [kallistios, kos, dc-chain, kos-chain, environ.sh, kos-ports, makefile, romdisk, genromfs, mkdcdisc, gcc]
estado: borrador
fecha: 2026-10-30
fuentes: [https://www.dreamcast.wiki/Getting_Started_with_Dreamcast_development, https://kos-docs.dreamcast.wiki/md_doc_2README.html, https://github.com/KallistiOS/KallistiOS, https://github.com/KallistiOS/kos-ports, https://www.dreamcast.wiki/Romdisk_Swapping, KOS instalado con DreamSDK R4 (environ.sh, Makefile.rules, include/kos/init.h)]
---

# Toolchain de KallistiOS fuera de DreamSDK

## Resumen

En Windows lo cómodo es [DreamSDK](dreamsdk-windows.md). En Linux, macOS o WSL el toolchain se compila desde el código de KOS: primero el compilador cruzado SH-4 (más el ARM del AICA), luego KOS, luego kos-ports. Todo vive bajo `/opt/toolchains/dc`. Esta entrada resume la guía oficial de dreamcast.wiki y las convenciones de un proyecto KOS (Makefile, romdisk). La estructura de directorios y `environ.sh` coinciden con los de DreamSDK.

## Detalle técnico

### Estructura

```
/opt/toolchains/dc/
├── sh-elf/          # GCC + binutils + newlib para el SH-4 (KOS_CC_BASE)
├── arm-eabi/        # toolchain del ARM7 del AICA (DC_ARM_BASE)
├── bin/             # utilidades externas: dc-tool, etc. (DC_TOOLS_BASE)
├── kos/             # KallistiOS (KOS_BASE)
│   ├── environ.sh   # copia editada de doc/environ.sh.sample
│   ├── utils/dc-chain/   # scripts del toolchain (en master: kos-chain)
│   └── examples/dreamcast/
└── kos-ports/       # bibliotecas portadas (KOS_PORTS)
```

### Pasos

```sh
# dependencias (Debian/Ubuntu; la wiki da también las de Fedora, Arch, Alpine y Homebrew)
sudo apt install gawk patch bzip2 tar make libgmp-dev libmpfr-dev libmpc-dev gettext wget \
  libelf-dev texinfo bison flex sed git build-essential diffutils curl libjpeg-dev libpng-dev \
  python3 pkg-config cmake libisofs-dev meson ninja-build rake

sudo mkdir -p /opt/toolchains/dc
sudo chown -R $(id -u):$(id -g) /opt/toolchains/dc
git clone https://github.com/KallistiOS/KallistiOS.git -b v2.2.x /opt/toolchains/dc/kos   # o -b master

# 1. toolchain (tarda bastante)
cd /opt/toolchains/dc/kos/utils/dc-chain      # en master el directorio es kos-chain
cp Makefile.default.cfg Makefile.cfg          # en master: Makefile.dreamcast.cfg
make                                          # si falla, poner makejobs=1 en Makefile.cfg

# 2. KOS
cd /opt/toolchains/dc/kos
cp doc/environ.sh.sample environ.sh
. ./environ.sh                                # en CADA shell nuevo
make

# 3. kos-ports
git clone --recursive https://github.com/KallistiOS/kos-ports /opt/toolchains/dc/kos-ports
/opt/toolchains/dc/kos-ports/utils/build-all.sh     # o, port a port: cd kos-ports/<port> && make install clean

# 4. ejemplos
cd /opt/toolchains/dc/kos/examples && make
```

- En macOS con Apple Silicon hay que exportar `CPATH=/opt/homebrew/include` y `LIBRARY_PATH=/opt/homebrew/lib`.
- El perfil estable usa **GCC 13.2**, la misma versión que DreamSDK R4.
- Versiones: desde v2.1.0 cada serie estable tiene una rama `vX.Y.x` y los parches son *tags* de esa rama. `master` es la rama de desarrollo. Desde KOS 2.0.0 solo se soporta Dreamcast (más la subarquitectura NAOMI).

### Qué fija `environ.sh`

Variables (valores del `environ.sh` de DreamSDK R4):

| Variable | Valor | Uso |
|---|---|---|
| `KOS_BASE` | `/opt/toolchains/dc/kos` | raíz de KOS |
| `KOS_PORTS` | `${KOS_BASE}/../kos-ports` | cabeceras y bibliotecas de los ports |
| `KOS_CC_BASE` / `KOS_CC_PREFIX` | `/opt/toolchains/dc/sh-elf` / `sh-elf` | compilador SH-4 |
| `DC_ARM_BASE` / `DC_ARM_PREFIX` | `/opt/toolchains/dc/arm-eabi` / `arm-eabi` | compilador del AICA |
| `KOS_GENROMFS` | `${KOS_BASE}/utils/genromfs/genromfs` | genera imágenes romdisk |
| `KOS_LOADER` | `dc-tool -x` | lo que ejecuta `make run` |
| `KOS_CMAKE_TOOLCHAIN` | `${KOS_BASE}/utils/cmake/kallistios.toolchain.cmake` | para proyectos CMake |

Flags por defecto: `-O2 -fno-PIC -fno-PIE -fomit-frame-pointer` y `KOS_SH4_PRECISION="-m4-single"`. La plantilla explica que PIC/PIE y los *frame pointers* no son fiables en SH-4, y deja comentadas otras opciones (LTO, `-ffast-math`, protección de pila, quitar `assert` en release...). **Si se cambia el ABI de coma flotante (`-m4-single`) hay que recompilar KOS y todos los ports.**

### Makefile de un proyecto

Patrón de los ejemplos de KOS (`examples/dreamcast/png/Makefile`):

```make
TARGET = example.elf
OBJS = example.o romdisk.o
KOS_ROMDISK_DIR = romdisk          # activa las reglas romdisk.img / romdisk.o de Makefile.rules

all: rm-elf $(TARGET)

include $(KOS_BASE)/Makefile.rules

clean: rm-elf
	-rm -f $(OBJS)

rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET): $(OBJS)
	kos-cc -o $(TARGET) $(OBJS) -lpng -lz      # bibliotecas de kos-ports

run: $(TARGET)
	$(KOS_LOADER) $(TARGET)

dist: $(TARGET)
	$(KOS_STRIP) $(TARGET)
```

- `kos-cc` es un *wrapper* de `sh-elf-gcc` con las rutas y flags de KOS. Las rutas de `kos-ports/include` y `kos-ports/lib` se añaden solas.
- Un binario cualquiera se convierte en objeto enlazable con `$(KOS_BASE)/utils/bin2o/bin2o fichero.bin simbolo fichero.o`.
- En el código, `KOS_INIT_FLAGS(...)` elige qué subsistemas arrancan. Si no se pone, equivale a `INIT_DEFAULT`.

### Romdisk

Un romdisk es una imagen de sistema de ficheros de solo lectura (genromfs) que se monta en memoria.

- **Romdisk incrustado:** con `KOS_ROMDISK_DIR = carpeta`, `Makefile.rules` genera `romdisk.img` y `romdisk.o`, y KOS lo monta en `/rd`. **`KOS_INIT_ROMDISK(romdisk)` está obsoleto** en el KOS actual (la cabecera `kos/init.h` lo marca como "deprecated and not useful anymore"). Mucho código antiguo, wiki incluida, todavía lo usa.
- **Romdisks intercambiables:** para no tener todos los recursos en RAM a la vez, se genera un romdisk por nivel (`$(KOS_GENROMFS) -f level1.img -d ruta/level1 -v`) y se carga en tiempo de ejecución:

```c
/* leer level1.img entero a un buffer (fs_open/fs_read o fs_load) y luego: */
fs_romdisk_mount("/level1", buffer, true);   /* true: KOS libera el buffer al desmontar */
/* ... /level1/spritesheet.png ... */
fs_romdisk_unmount("/level1");
```

  Firma real: `int fs_romdisk_mount(const char *mountpoint, const uint8_t *img, bool own_buffer)`. El punto de montaje **debe empezar por `/`**. Durante el desarrollo con dc-tool la imagen se lee de `/pc/...`; en el disco final, de `/cd/...`.
- El código de ejemplo de la wiki para esto está roto (llama a `gzread` sin abrir el fichero). Además, la propia página dice que la compresión gz se quitó porque daba problemas con nombres de más de ~16 caracteres.

### Imagen de disco con mkdcdisc

Alternativa de una sola orden al proceso manual de [imagen CDI](imagen-cdi.md):

```sh
git clone https://gitlab.com/simulant/mkdcdisc.git && cd mkdcdisc
meson setup builddir && meson compile -C builddir
./builddir/mkdcdisc -e MiJuego.elf -o MiJuego.cdi
```

El CDI se graba con DiscJuggler (Windows o WINE), ImgBurn con el plugin CDI, o un script `cdiburn` en *nix.

### Ejemplos de KOS como referencia

`kos/examples/dreamcast/` es la mejor documentación práctica de la API. Los más útiles para un juego:

| Carpeta | Qué enseña |
|---|---|
| `pvr/` | `palette`, `strided_texture`, `modifier_volume*`, `texture_render` (render a textura), `pvrmark_strips_direct` (envío directo al TA), `fb_tex` |
| `gldc/` | `2D_tex_quad`, `nehe`, `benchmarks` |
| `sound/` | `sfx`, `sfxbuf`, `cdda`, `hello-mp3`, `hello-ogg`, `hello-opus`, `multi-stream` |
| `vmu/` | `vmu_pkg` (partidas guardadas), `vmu_lcd`, `vmu_beep` |
| `basic/` | `threading`, `mmu`, `dma`, `fpu`, `gdb_breaking`, `stacktrace` |
| `profiling/` | `gprof`, `gcov` |
| `parallax/`, `tsunami/` | bibliotecas 2D de alto nivel de KOS |
| `raylib/`, `sdl/`, `lua/`, `micropython/`, `mruby/` | otros lenguajes y bibliotecas |

## Notas / gotchas

- `environ.sh` hay que cargarlo **con `.` o `source`** en cada terminal, no ejecutarlo.
- La guía de la wiki es incoherente con el nombre del directorio del toolchain: en v2.2.x es `utils/dc-chain` y en master `utils/kos-chain`.
- Los makefiles de KOS no siguen dependencias de cabeceras (ver [DreamSDK en Windows](dreamsdk-windows.md)).

## Referencias cruzadas

- [DreamSDK R4 en Windows](dreamsdk-windows.md) — el mismo árbol, empaquetado para Windows
- [Carga en consola y depuración](carga-y-depuracion.md) — `make run` con dc-tool
- [Bibliotecas, motores y recursos](bibliotecas-motores-y-recursos.md) — lista completa de kos-ports

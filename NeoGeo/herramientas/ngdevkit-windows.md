---
titulo: ngdevkit en Windows (MSYS2) — instalación, build de la ROM y GnGeo
plataforma: [NeoGeo]
categoria: [herramientas]
tags: [ngdevkit, msys2, ucrt64, gcc, sdcc, gngeo, romtool, nullbios, nullsound, pruebas]
estado: revisado
fecha: 2026-10-23
fuentes: [README y README-msys2 de ngdevkit, repositorio https://dciabrin.net/msys2-ngdevkit/, ngdevkit-examples (00-template, 03-sprite-animation), código de GnGeo (rama ngdevkit), proyecto RickAndMorty/neogeo]
---

# ngdevkit en Windows (MSYS2)

## Resumen

[ngdevkit](https://github.com/dciabrin/ngdevkit) es un SDK libre en C para Neo Geo. Incluye GCC para 68000, SDCC para Z80, GDB, una BIOS libre (*nullbios*), un driver de sonido vacío (*nullsound*), herramientas Python para gráficos y ROM, y una versión de GnGeo con depuración remota. En Windows funciona bajo MSYS2, entorno **UCRT64**.

Todo lo de esta entrada se ejecutó con ngdevkit 0.5+202610171524, GCC 15.3.0 y GnGeo 0.8.1.

## Detalle técnico

### Instalación sin compilar

- El repositorio de paquetes precompilados es `https://dciabrin.net/msys2-ngdevkit/x86_64/`, y su índice es `ngdevkit.db`, un tar.gz con un `desc` por paquete.
- Paquetes necesarios:
  - `mingw-w64-ucrt-x86_64-ngdevkit-toolchain`: 148 MB de descarga y 641 MB instalado.
  - `mingw-w64-ucrt-x86_64-ngdevkit`.
  - `mingw-w64-ucrt-x86_64-ngdevkit-gngeo`.
- El README propone añadir el repositorio a `pacman.conf` con `SigLevel = Optional TrustAll`. **Alternativa más prudente**: descargar los `.pkg.tar.zst`, comprobar su SHA-256 contra el campo `%SHA256SUM%` del `desc` e instalarlos con `pacman -U` (que resuelve las dependencias desde los repositorios oficiales de MSYS2). Así no queda ningún repositorio sin verificación de firmas.
- Dependencias que arrastra, todas nuevas en un UCRT64 vacío:
  - Python 3.14 con `pillow` y `ruamel-yaml`.
  - `pkgconf`, `gmp`, `mpfr`, `mpc`, `isl`, `expat`, `gettext`, `ncurses`, `zlib` y `flex`.
  - SDL2 y glew para GnGeo.

  Antes conviene simular con `pacman -S --needed --print` para confirmar que no se actualiza ningún paquete ya instalado.
- Qué se instala:
  - `m68k-neogeo-elf-*` (gcc, objcopy, gdb…) y `z80-neogeo-ihx-*` (sdcc, sdasz80, sdldz80, sdobjcopy…).
  - En `/ucrt64/bin`: `tiletool.py`, `paltool.py`, `romtool.py`, `adpcmtool.py`, `vromtool.py`, `soundtool.py`, `furtool.py` y `nsstool.py`.
  - En `/ucrt64/share/ngdevkit`: `neogeo.zip` y `aes.zip` (nullbios) y `nullsound_driver.ihx`.
- Flags: `pkg-config --cflags/--libs ngdevkit` da `-specs ngdevkit -lngdevkit`. Las variables `sharedir` y `bindir` dan rutas al estilo Windows (`C:/msys64/ucrt64/...`).

### Arranque del programa (crt0 y linkscript)

- La BIOS llama a `main()`.
- Las opciones del cartucho son **símbolos de enlace**, no variables: `-Wl,--defsym,rom_eye_catcher_mode=2`.
  - `rom_eye_catcher_mode`: 0 = la BIOS muestra su logo (necesita los tiles del logo en la ROM C), 1 = lo hace el juego, 2 = sin logo.
  - `rom_NGH_ID`: 1337 por defecto.
- El VBlank por defecto confirma la interrupción, alimenta el *watchdog*, llama a `rom_callback_VBlank` y después a `SYSTEM_IO`, la rutina de la BIOS que lee los mandos. Por eso, al volver de `ng_wait_vblank()`, `bios_p1current` y `bios_p2current` ya están actualizados.
- **`ng_wait_vblank()` usa el propio `rom_callback_VBlank` de libngdevkit.** Si el juego define otro, el enlace o la espera dejan de funcionar. Para hacer trabajo en el VBlank hay que ponerlo después de `ng_wait_vblank()`.

### De las fuentes a la ROM (Makefile propio, sin `configure`)

1. **P1**: `objcopy -O binary -S -R .text2 --gap-fill 0xff --pad-to 1048576 rom.elf p1` y luego `dd conv=notrunc,swab`, porque la ROM P va con los bytes de cada palabra intercambiados.
2. **C1/C2 y S1**: se concatenan los tiles y se rellena con `truncate -s` hasta el tamaño de la ROM.
3. **M1**: `z80-neogeo-ihx-sdobjcopy -I ihex -O binary nullsound_driver.ihx m1 --pad-to 131072`.
4. **V1**: sin samples basta un fichero de ceros (`truncate -s 524288`).
5. **Cartucho**: `romtool.py -b cartridge -f zip -p P1 -c C1 C2 -s S1 -m M1 -v V1 -n NOMBRE -o NOMBRE.zip`.
6. **GnGeo** necesita la descripción del cartucho dentro de **su** `gngeo_data.zip`: `romtool.py -b hash -f gngeo ... -l "Título" -x gngeo.data=/ucrt64/share/ngdevkit-gngeo/gngeo_data.zip -o gngeo_data.zip`. El nombre del juego admite hasta 31 caracteres.
7. Junto a la ROM se copian `neogeo.zip` y `aes.zip` (nullbios).
8. Ejecutar: `ngdevkit-gngeo --scale 3 --no-resize --system home -i build/rom -d build/rom/gngeo_data.zip NOMBRE`.

Los ejemplos oficiales generan la fuente de la capa fix y el logo con ImageMagick (`convert`), que no viene con ngdevkit. Un script en Python con Pillow lo sustituye sin problemas (ver [formatos de las ROM C y S](../graficos/rom-c-s-y-paletas.md)).

### Mandos en GnGeo

- GnGeo SDL2 lee las teclas como `K<código>`, donde el código es `keysym.sym & ~SDLK_SCANCODE_MASK`: las flechas son 79–82 (derecha, izquierda, abajo, arriba) y las letras van en ASCII minúscula.
- Los valores por defecto del código fuente usan códigos de SDL1 (`UP=K273`…) y el jugador 2 viene vacío. Por eso conviene pasar los dos explícitamente:
  - `--p1control=UP=K82,DOWN=K81,LEFT=K80,RIGHT=K79,A=K122,B=K120,C=K99,D=K118,START=K49,COIN=K53`
  - `--p2control=UP=K119,DOWN=K115,LEFT=K97,RIGHT=K100,A=K106,B=K107,C=K108,D=K105,START=K50,COIN=K54`
- GnGeo no se pausa al perder el foco.

### Pruebas automáticas sin tocar el escritorio

- **Capturar:** `PrintWindow(hwnd, hdc, PW_CLIENTONLY | PW_RENDERFULLCONTENT)` sobre la ventana de GnGeo. Funciona aunque la ventana esté tapada.
- **Pulsar:** `PostMessage(hwnd, WM_KEYDOWN/WM_KEYUP, vk, lParam)`, con el *scancode* en los bits 16-23 y el bit 24 de tecla extendida para las flechas. SDL2 lo trata como una pulsación normal y no afecta a otras ventanas.
- **No usar `keybd_event`/`SendInput` ni `CopyFromScreen`**. Windows suele negar `SetForegroundWindow` a un proceso en segundo plano: las teclas irían a la ventana activa del usuario y la captura mostraría otra ventana. Ocurrió en la primera prueba.
- El script `tools/probar_gngeo.ps1` del proyecto implementa este método, con pasos `wait`, `press`, `release`, `hold` y `shot`.

## Notas / gotchas

- `pacman -S` puede fallar al descargar algún fichero de un mirror ("no se pudieron recibir algunos archivos"). `pacman -U` sobre los paquetes locales volvió a resolver e instaló todas las dependencias. Hay que comprobarlo después con `pacman -Q` y `pacman -Dk`.
- La captura F2 de GnGeo está en código desactivado de esa versión: no sirve para pruebas.
- MAME exige su BIOS original de Neo Geo, así que con nullbios solo vale GnGeo.

## Referencias cruzadas

- [Sprites: SCB1-SCB4, cadenas, espejo y orden](../graficos/sprites-scb.md)
- [Formatos de la ROM C, la ROM S y las paletas](../graficos/rom-c-s-y-paletas.md)
- [Caso práctico: SimuLockdown en Neo Geo](caso-practico-simulockdown.md)

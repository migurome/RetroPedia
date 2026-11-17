#!/bin/sh
# Genera out/sprite.cdi, listo para Flycast. Uso: sh construir.sh   (bash de Git; DreamSDK en C:\DreamSDK)
# La ruta del snippet no debe tener espacios: los makefiles de KOS no los admiten.
set -e
cd "$(dirname "$0")"
mkdir -p out/cd
py -I png2argb.py robot.png sprite_tex.h robot
py -I make_ip.py /c/DreamSDK/opt/toolchains/dc/kos/utils/makeip/rsrc/templates/ip.tmpl out/IP.BIN "SNIPPET SPRITE" V0.001
RUTA="$(pwd)" exec /c/DreamSDK/usr/bin/bash.exe --noprofile --norc -c '
set -e
export PATH=/usr/bin:/bin:/opt/toolchains/dc/bin:/opt/toolchains/dc/sh-elf/bin:$PATH
# environ.sh pregunta la versión con kos-cc, que a veces se cuelga: mientras carga, contesta sh-elf-gcc
kos-cc() { sh-elf-gcc "$@"; }
. /opt/toolchains/dc/kos/environ.sh >/dev/null 2>&1
unset -f kos-cc
export PATH=$KOS_BASE/utils/build_wrappers:$PATH
export DREAMSDK_HOME="C:\\DreamSDK" TMP="C:\\DreamSDK\\tmp" TEMP="C:\\DreamSDK\\tmp" TMPDIR=/tmp
cd "$RUTA"
make </dev/null 2>&1 | grep -v m4-single
sh-elf-objcopy -R .stack -O binary sprite.elf out/sprite.bin
"$KOS_BASE/utils/scramble/scramble.exe" out/sprite.bin out/cd/1ST_READ.BIN </dev/null
mkisofs -C 0,11702 -V SPRITE -G out/IP.BIN -r -J -l -quiet -o out/sprite.iso out/cd </dev/null
cdi4dc out/sprite.iso out/sprite.cdi </dev/null >/dev/null
rm -f out/sprite.iso out/sprite.bin
ls -la out/sprite.cdi
'

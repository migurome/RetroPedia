#!/bin/sh
# Genera out/rom.bin. Uso: sh construir.sh   (bash de Git; SGDK 2.11 en C:\sgdk)
# El make de SGDK no admite espacios en la ruta del kit.
set -e
cd "$(dirname "$0")"
GDK=C:/sgdk /c/sgdk/bin/make.exe -f C:/sgdk/makefile.gen
ls -la out/rom.bin

---
titulo: Bumbershoot Software — modernizando el toolchain de ASMX en 2025 (INCBIN, RORG/REND, smdfix)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [bumbershoot, asmx, z80, incbin, makefile, smdfix, toolchain, migracion]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2025/05/31/polishing-the-improvised-genesis-toolchain/]
---

# Bumbershoot Software — modernizando el toolchain de ASMX en 2025 (INCBIN, RORG/REND, smdfix)

## Resumen

Siete años después del toolchain improvisado de 2018 (ver `bumbershoot-toolchain-improvisado-2018.md`), el autor revisa y reescribe casi por completo su flujo de build para Genesis, aprovechando que el fork de ASMX activo en GitHub (el mismo que ya se menciona de pasada en `bumbershoot-comparativa-toolchains.md` como "la continuación moderna") alcanzó la versión 2.0, con soporte nativo de `INCBIN` y ensamblado multi-CPU en un mismo fichero fuente.

## Detalle técnico

### Qué fallaba en el toolchain original de 2018

- ASMX 1.8.2 no tenía `INCBIN`, obligando al truco de convertir binarios a texto con `sed`/`od` (documentado en la entrada de 2018).
- Solo podía emitir S-Record, no binario nativo; además había un bug en los S-Record de 24 bits que forzaba usar el modo de 32 bits como workaround: `objcopy -Isrec -Obinary romimage.s9 romimage.bin`.
- ASMX no tenía ningún conocimiento incorporado de la plataforma Genesis/Mega Drive (checksum, tamaño de ROM, etc. corrían todos por cuenta de scripts externos).
- El ensamblador Z80 usado entonces, `z88dk-z80asm`, generaba de forma incontrolable ficheros `.obj`, `.sym` y `.map` no deseados, y necesitaba el mismo pipeline de conversión binario-a-texto que el lado 68000 para poder incluir datos/gráficos.

### El nuevo toolchain: ASMX 2.0+

El autor migra al fork de ASMX mantenido activamente en GitHub, que resuelve de raíz las carencias anteriores:

- Un único binario del ensamblador soporta múltiples CPUs objetivo (antes hacía falta un binario distinto por procesador).
- `INCBIN` nativo y salida binaria directa, eliminando por completo los workarounds de `sed`/`objcopy`.
- Ensamblado con desplazamiento ("offset assembly") mediante las directivas `RORG`/`REND`, que permite mezclar código de distintas CPUs en el mismo fichero fuente.

### Migración del lado Z80

Cambios de sintaxis necesarios al pasar de `z88dk-z80asm` a ASMX 2.0 para el código Z80:

- El prefijo hexadecimal `$FF` se sustituye por `0xFF` (una regresión intencional de la 2.0, no un descuido).
- La directiva `BINARY` pasa a llamarse `INCBIN`.
- `DEFC` desaparece; se usa `EQU` en su lugar.
- El relleno de `DEFS` cambia: ASMX rellena con `0xFF`, frente a `0x00` en z88dk (importante si algún código depende del valor de relleno).
- La pseudo-etiqueta `ASMPC` (dirección actual) se sustituye por `$` o `*`.
- Se eliminan especificaciones redundantes de acumulador en instrucciones Z80 (formas con un operando `A` de más que ya no hacen falta).
- Se borran unos 15 KB de directivas `DEFB` en `fm_mus.asm` (datos de música volcados como bytes) y se sustituyen por un único `INCBIN`.

Nuevo comando de ensamblado Z80:

```
asmx -C z80 -w -e -b -o $@ $<
```

que sustituye al antiguo `z80asm -b $<`.

### Migración del lado 68000

Los cambios aquí son mínimos:

- El prefijo de etiqueta local cambia de `@label` a `.label` (estilo NASM, cambio intencional y no retrocompatible de la 2.0).
- Todos los `include` que apuntaban a los ficheros de texto hexadecimal generados por el truco de 2018 se sustituyen por `incbin` directamente sobre los `.bin` originales.

Nuevo comando de ensamblado 68000:

```
asmx -C 68000 -w -e -b -o $@ $<
```

### `smdfix`: sustituto de `srec2smd.py`

Se sustituye el script Python `srec2smd.py` de 2018 por una utilidad nueva en C, `smdfix`, que:

- Trabaja directamente sobre el binario plano ya ensamblado (ya no hace falta el paso intermedio por S-Record).
- Corrige/actualiza los campos de la cabecera de ROM.
- Expande el binario al tamaño que espera el emulador.
- Procesa el fichero en bloques de 16 KB.

### Makefile reescrito por completo

```makefile
BINARIES = hello.bin bumberlogo.bin sprcont.bin cca.bin
Z80OBJS  = 8k_dac.bin fm_mus.bin psg80.bin
TOOLS    = smdfix

all: $(BINARIES)

$(BINARIES): %.bin: %.s smdfix
	asmx -C 68000 -w -e -b -o $@ $< && ./smdfix $@

$(Z80OBJS): %.bin: %.asm
	asmx -C z80 -w -e -b -o $@ $<

smdfix: tools/smdfix.c
	gcc -O2 -o $@ $<

clean:
	rm -f $(BINARIES) $(Z80OBJS) $(TOOLS)

tidy:
	rm -f $(Z80OBJS) $(TOOLS)
```

Respecto al Makefile de 2018 (implícito, no reescrito antes en detalle): desaparecen las reglas intermedias de conversión `.s80`/`.hex` y todo el procesado de S-Record; el número de tipos de `.bin` generados se consolida declarando los nombres de fichero explícitamente.

### Técnica avanzada: stub de inicio Z80 embebido en fuente 68000

En 2018, el arranque del Z80 se inyectaba como bytes crudos vía `hex` (p. ej. `hex f3 ed 56 21 06 00 e9 00`). Con ASMX 2.0 y su ensamblado multi-CPU con offset, el mismo stub se escribe como Z80 real, embebido dentro del fichero fuente 68000:

```asm
rorg    0
cpu     z80

di
im      1
ld      hl, .z80lp
.z80lp: jp      (hl)

cpu     68000
rend
even
```

### Hallazgo de compatibilidad

El código Z80 migrado también ensambla correctamente — y produce salida binaria idéntica byte a byte al ASMX 1.8.2 original — con versiones recientes de z88dk y con vasm (`vasmz80_oldstyle`). Es decir, la migración de sintaxis no ata el proyecto a ASMX en exclusiva.

### Qué se mantiene sin cambios desde 2018

- Sintaxis de estilo Motorola (frente a la de `gas`/GNU).
- Ausencia de un paso de linkado obligatorio o de un mapa de memoria predeclarado.
- Un sistema de segmentación propio para intercalar código de ROM y declaraciones de variables en RAM.
- Soporte de macros y de ensamblado con offset.

## Notas / gotchas

- El prefijo `0x` en vez de `$` para hexadecimal en Z80 es un cambio de sintaxis deliberado de ASMX 2.0, no un bug — cualquier fuente Z80 antigua necesita pasar por esta migración de sintaxis.
- `smdfix` ya no necesita Python ni pasar por S-Record: es un cambio de arquitectura, no solo un refactor del script anterior.
- Esta entrada asume conocido el toolchain de 2018 documentado en `bumbershoot-toolchain-improvisado-2018.md`; léase esa primero si no se tiene contexto de qué problema resuelve cada cambio.
- El ASMX 2.0 aquí descrito es el mismo proyecto ya mencionado en `bumbershoot-comparativa-toolchains.md` ("la continuación moderna permite mezclar código m68k y Z80 en el mismo fichero fuente") — esta entrada aporta el detalle práctico de esa frase genérica.

## Referencias cruzadas

- [Bumbershoot Software — el toolchain improvisado de 2018 para Genesis (ASMX + conversión manual)](bumbershoot-toolchain-improvisado-2018.md) — el toolchain original que esta entrada sustituye pieza por pieza.
- [Comparativa de toolchains de ensamblador para Mega Drive](bumbershoot-comparativa-toolchains.md) — menciona de pasada la existencia del fork moderno de ASMX; esta entrada detalla en qué consiste.
- [Bumbershoot Software — probando ASMotor para Mega Drive (2026)](bumbershoot-asmotor-mega-drive.md) — un año después el autor evalúa ASMotor como alternativa a este mismo ASMX 2.0 y decide quedarse con ASMX.
- [Build y debug de ASM puro para Mega Drive (sin SGDK)](build-debug-asm-puro.md) — comparativa general de ensambladores/depuradores modernos.

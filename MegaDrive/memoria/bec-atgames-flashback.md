---
titulo: Depurando el AtGames Flashback — bug del emulador de Z80 en IYH (TANGLEWOOD)
plataforma: [MegaDrive]
categoria: [memoria]
tags: [bigevilcorporation, tanglewood, atgames, flashback, z80, echo, debugging]
estado: borrador
fecha: 2026-07-21
fuentes: [https://blog.bigevilcorporation.co.uk/2018/04/22/taming-the-atgames-flashback/]
---

# Depurando el AtGames Flashback — bug del emulador de Z80 en IYH (TANGLEWOOD)

## Resumen

Artículo de Matt (Big Evil Corporation) publicado el 22/04/2018, "Taming the AtGames Flashback". Segunda entrega de su investigación de compatibilidad de TANGLEWOOD (juego comercial publicado) con clones AtGames: en el modelo Flashback, el juego se quedaba sin música FM ni PCM (el PSG sí funcionaba), a causa de un bug concreto en la implementación del Z80 emulado por ese clon al cargar el byte alto del registro índice IY (`IYH`).

## Detalle técnico

### Síntoma

En hardware AtGames Flashback, TANGLEWOOD perdía por completo la síntesis FM y el PCM, mientras que el PSG seguía sonando con normalidad.

### Causa raíz

El driver de sonido usado por el juego, Echo (de Sik), mantiene el registro índice `IY` del Z80 cargado con `$4000` durante buena parte de su ejecución, para acceder de forma eficiente a los puertos del YM2612 usando direccionamiento indexado. Para ello inicializaba solo el byte alto del registro con `LD IYH, $40`. El emulador de Z80 del AtGames Flashback no implementa correctamente esta forma de carga de `IYH`: mientras que el byte bajo (`IYL`) se comporta con normalidad, escribir o leer `IYH` de forma aislada produce resultados incorrectos en este clon. Como consecuencia, Echo nunca llegaba a inicializar bien el acceso al puerto `$4000` del YM2612, dejando la síntesis FM en silencio.

### Fragmento de diagnóstico

Código usado para aislar el problema, comprobando si `IYH` retiene realmente el valor `$40` tras cargarlo:

```
ex af, af'      ; Backup A+F
ld c,iyh        ; Load top byte of IY
ld a,c          ; To register A
cp $40          ; Check it's $40
jp z,.ok        ; If correct, bail
.piano:
call Piano      ; IY is not $40XX
.ok:
ex af, af'
```

Inicialización original (problemática) en Echo:

```
ld iyh, $40
```

Solución aplicada — cargar el registro `IY` completo de una sola vez en lugar de solo su mitad alta:

```
ld iy, $4000
```

Comprobación adicional propuesta para detectar hardware con este defecto en tiempo de ejecución (usada también para decidir si desactivar `SSG-EG`, ver nota más abajo):

```
ld iy, $1234
ld iyh, $40
ld a, iyh
cp $40
jp nz, .disableSSGEG
```

### Registros y direcciones relevantes

- Puertos del YM2612 usados por Echo vía direccionamiento indexado con `IY`: `$4000`/`$4001` (canales 1-3) y `$4002`/`$4003` (canales 4-6).
- `IY` es un registro de 16 bits del Z80; `IYH` es su byte alto, `IYL` su byte bajo.
- Valor esperado en `IYH` tras la inicialización de Echo: `$40`.

## Notas / gotchas

- El bug es específico de la implementación de `LD IYH,nn` (carga inmediata al byte alto de un registro índice) en el emulador de Z80 del Flashback; no afecta a `IYL` ni a la carga del registro `IY` completo de 16 bits, que es justamente la solución adoptada.
- Solución práctica: sustituir cualquier `LD IYH, nn` por `LD IY, nnXX` (cargar el registro completo) evita el bug sin necesidad de detectar el hardware.
- El propio autor señala que el Flashback comparte con el Firecore el problema de `SSG-EG` no emulado correctamente en el YM2612 (ver artículo hermano sobre el Firecore).
- Es un ejemplo de bug de *emulación* de CPU en hardware clon comercial, distinto en naturaleza al problema de registros de reloj no estándar del Firecore, aunque motivado por la misma necesidad de compatibilidad amplia de un juego comercial ya publicado.

## Referencias cruzadas

- [`hardware-issues.md`](hardware-issues.md) (fuente plutiedev.com) — ya recoge una nota breve: "El clon Genesis Flashback carece de implementación de `LD IYH,nn`... las soluciones alternativas sustituían por `LD IY,4000h`". Esta entrada es el relato técnico completo de primera mano detrás de esa nota, incluyendo el código de diagnóstico y el porqué del fallo en el driver Echo.
- [`bec-atgames-firecore.md`](bec-atgames-firecore.md) — primer artículo de la misma investigación del autor sobre clones AtGames, esta vez centrado en un problema de registros de reloj no estándar (no de emulación de CPU) en el modelo Firecore.
- [`../cpu/using-the-z80.md`](../cpu/using-the-z80.md) — contexto general de programación del Z80 en Mega Drive, útil para entender por qué un driver de sonido como Echo se apoya en registros índice del Z80.

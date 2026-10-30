---
titulo: Simulación idéntica en PC y Dreamcast — libm, fmac y -ffp-contract
plataforma: [Dreamcast]
categoria: [cpu, herramientas]
tags: [sh4, float, ieee754, determinismo, fmac, ffp-contract, libm, newlib, mingw, qa, huella, mugen]
estado: borrador
fecha: 2026-10-30
fuentes: [proyecto hyperdbz-dreamcast-port (dc/proto/src/logic.c, tools/qa/), medido en Flycast 2.7 y MinGW-w64 de DreamSDK R4]
---

# Simulación idéntica en PC y Dreamcast

## Resumen

Compilar la lógica del juego también para PC permite depurarla en segundos y hacer QA automática: se compara
una **huella** (hash del estado de todos los actores, tick a tick) con la de referencia, y con la que imprime
Dreamcast. Para que la huella coincida **bit a bit** en x86 y en el SH-4 hacen falta dos cosas:
1. no usar las funciones de la libm (`sinf`, `atanf`, `powf`...);
2. compilar sin contracción de multiplicación-suma (`-ffp-contract=off`).

## Detalle técnico

### Qué es exacto y qué no

- `+ - * /` y `sqrtf` en `float` son operaciones IEEE 754 correctamente redondeadas en las dos máquinas
  (SSE en x86-64; `fadd`/`fmul`/`fdiv`/`fsqrt` del SH-4). `floorf`, `ceilf`, `fabsf`, `frexpf` y
  `ldexpf` también son exactas.
- **La libm no:** `atanf`, `sinf`, `expf`, `powf`... no están obligadas a redondear bien, y newlib (Dreamcast)
  y la CRT de MinGW dan a veces resultados que difieren en el último bit. Basta un bit para que la
  simulación diverja tiempo después.
- **Caso real:** en Hyper DBZ, un helper de Goku calcula su ángulo con
  `fvar(4) = (180/pi)*atan(-vel y / vel x)`. En el tick 61 del combate, `fvar(4)` valía `0x429834A4` en
  PC y `0x429834A5` en Dreamcast.

### Funciones propias

Se implementan solo con operaciones exactas, y el resultado es el mismo en las dos máquinas:
- **`sin`:** reducción a [−π, π] con 2π en dos partes (Cody-Waite) y simetría a [−π/2, π/2], con polinomio
  de Taylor de grado 11. `cos(x) = sin(x + π/2)`.
- **`atan`:** si |x| > 1, `π/2 − atan(1/x)`; si x > tan(π/8), `π/4 + atan((x−1)/(x+1))`, con serie hasta
  x¹⁵. Con ella, `asin(x) = atan(x / sqrt(1 − x²))`.
- **`exp`:** `x = k·ln2 + r`, polinomio de grado 7 en r y `ldexpf(p, k)`.
- **`ln`:** `frexpf` y `2·atanh(s)` con `s = (m−1)/(m+1)`.
- **`pow`:**
  - con exponente entero, multiplicaciones (cuadrado y multiplicación);
  - con exponente fraccionario, `exp(b·ln a)`;
  - entero elevado a entero, en enteros.

El error relativo queda por debajo de 2·10⁻⁶ (el peor caso, en `cos` con argumentos grandes), de sobra en un
juego que trabaja en píxeles.

### `-ffp-contract=off`

GCC, en modo GNU C, puede fusionar `a*b + c` en una sola instrucción (`fmac` en el SH-4, FMA en x86 con
`-mfma`), que no redondea el producto intermedio, y el resultado cambia. Hay que compilar la lógica con
`-ffp-contract=off` en **los dos** compiladores: en KOS, en `KOS_CFLAGS` del Makefile; en el arnés de PC, en
su línea de `gcc`.

### Cómo encontrar una divergencia

1. Imprimir la huella encadenada cada 300 ticks en los dos (`chain = chain·31 + hash`) para acotar el tramo.
2. Imprimir la huella sin encadenar de cada tick de ese tramo para encontrar el primero que difiere.
3. En ese tick, volcar cada actor (estado, posición en hexadecimal, variables) y comparar con `diff`.

En Hyper DBZ salieron tres pasadas de Flycast de un minuto. Se escriben los floats en hexadecimal porque
`%f` esconde el último bit.

## Notas / gotchas

- Sin la libm también se gana velocidad en el SH-4: la `atanf` de newlib es lenta.
- `fsin`/`fcos` de KOS (instrucción FSCA) son aproximadas y solo existen en Dreamcast. Se pueden usar para
  dibujar (no afectan a la simulación), pero nunca en la lógica que se compara.
- La conversión `(int)float` trunca igual en los dos, pero `(int)` de un valor fuera de rango es indefinida:
  limitar antes.
- Que la huella coincida en Flycast no garantiza que coincida en la consola si el emulador calcula algo
  distinto. Se comprobará en hardware real.

## Referencias cruzadas

- [Optimización del SH-4](sh4-optimizacion.md)
- [Flycast automatizado — buzón en RAM con Lua y capturas](../herramientas/flycast-automatizado.md)
- [Compilar la lógica CNS de MUGEN](../../MOTORES/mugen/cns-compilacion.md)

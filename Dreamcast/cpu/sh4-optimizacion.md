---
titulo: Optimización del SH-4 — caché, store queues, alineación, ABI, FIPR/FTRV y GCC
plataforma: [Dreamcast]
categoria: [cpu]
tags: [sh4, optimizacion, cache, store-queues, pref, movca, alineacion, abi, fpu, fipr, ftrv, fsrra, sh4zam, gcc]
estado: borrador
fecha: 2026-10-30
fuentes: [https://mc.pp.se/dc/cpu.html, https://www.dreamcast.wiki/Useful_programming_tips, https://www.dreamcast.wiki/GCC-SH4_tips, https://www.dreamcast.wiki/Fast_SH4_Vertex_Processing, https://www.dreamcast.wiki/SH4_FTRV_Optimizations, https://www.dreamcast.wiki/SH4_FIPR_Optimizations, https://www.dreamcast.wiki/Efficient_usage_of_the_Dreamcast_RAM, environ.sh de KOS]
---

# Optimización del SH-4

## Resumen

El SH-4 de la Dreamcast (SH7091, casi idéntico a un SH7750) es un RISC de 32 bits a 200 MHz, con instrucciones de 16 bits, *pipeline* de 5 etapas, **doble emisión** y una FPU con operaciones vectoriales de 4 floats. No tiene predicción de saltos ni ejecución especulativa, y funciona siempre en *little endian*. En la práctica, el rendimiento depende de tres cosas: la **alineación de los datos**, el **uso de la caché** y no **bloquear el pipeline** esperando resultados de la FPU.

## Detalle técnico

### Datos básicos

- Caché de instrucciones de 8 KB y caché de datos de 16 KB, **de correspondencia directa**. KOS usa *write-back*, el modo necesario para que la caché ayude en escrituras.
- 16 registros enteros de 32 bits y 32 registros de coma flotante de 32 bits, en dos bancos de 16. El banco trasero guarda la matriz `XMTRX`.
- Bus a 100 MHz: la RAM se lee y escribe en ráfagas de 32 bytes (ver [mapa de memoria](../memoria/mapa-de-memoria.md)).

### Alineación

El SH-4 **no admite accesos desalineados**: un `int` en una dirección que no es múltiplo de 4 hace que el programa casque. Si una estructura binaria leída de disco se declara `packed` sin más, GCC genera cada `uint32_t` con cuatro `mov.b`, extensiones, desplazamientos y OR. Declarando también la alineación se evita:

```c
typedef struct __attribute__((packed, aligned(4))) {
    unsigned char id[4];
    unsigned int  address;
    unsigned int  size;
    unsigned char data[];
} command_t;
```

### Caché

- **Correspondencia directa**: dos direcciones separadas por un múltiplo de 16 KB caen en la misma línea. Copiar de `buf` a `buf + 16 KB` hace que cada acceso eche al anterior de la caché (*thrashing*). Hay que desplazar uno de los dos buffers.
- **OCRAM**: la mitad de la caché de datos (8 KB) puede usarse como RAM rápida de trabajo. A cambio, la caché normal queda en 8 KB y el *aliasing* pasa a ser cada 8 KB.
- Instrucciones de control:

| Instrucción | Efecto |
|---|---|
| `pref @Rn` | trae la línea por adelantado. Se pone al principio del bucle para el elemento siguiente |
| `movca.l R0,@Rn` | reserva la línea **sin leerla de memoria**. Útil si se va a sobrescribir entera; mal usada corrompe datos |
| `ocbwb @Rn` | escribe la línea en memoria y la mantiene en caché |
| `ocbp @Rn` | escribe la línea y la invalida |
| `ocbi @Rn` | invalida sin escribir (tras un DMA o una escritura por store queue a esa zona) |

### Store queues

- Dos colas de 32 bytes independientes: mientras una se vacía, se puede ir llenando la otra. El bit 5 de la dirección (valor 32) elige la cola: es lo que hace `pvr_dr_target()` en KOS, que alterna con `pvr_dr_addr ^= 32`. La wiki habla del "bit 6", probablemente contando desde 1.
- Se saltan la caché: si se escribe por SQ en una zona que también está en caché, hay que invalidarla después.
- Cada vaciado espera a que termine el anterior de **esa misma** cola, así que conviene intercalar trabajo entre vaciados.
- Son la vía rápida para mandar datos al Tile Accelerator y a la VRAM. En KOS: `sq_cpy`, `sq_lock`/`sq_unlock` (cabecera `dc/sq.h`), y por debajo `pvr_txr_load` y el *direct rendering* del PVR (`pvr_dr_target` / `pvr_dr_commit`, ver [PowerVR](../graficos/powervr-pipeline.md)). En el KOS actual `pvr_dr_init` y `pvr_dr_finish` están obsoletos.

### ABI de C

- Argumentos: enteros en `r4`–`r7` y floats en `fr4`–`fr11`. Valor de retorno en `r0` / `fr0`.
- Las variables globales cuestan una carga extra de la dirección (`mov.l L2,r1` + `mov.l @r1,r1`). Las locales y los parámetros son más baratos.
- GCC genera mal código cuando se queda sin registros (*spills*): hay que preferir funciones pequeñas. La wiki sugiere partir las de más de ~100 líneas.
- Copiar una `struct` entera de una vez permite a GCC programar mejor las instrucciones que copiar campo a campo.

### Pipeline y doble emisión

- Se pueden emitir dos instrucciones por ciclo si pertenecen a **grupos distintos** y no dependen una de otra. Las del grupo CO nunca van en pareja; las MT pueden ir con otras MT si no hay dependencia.
- Para pasar de 200 MIPS hace falta paralelismo a nivel de instrucción. En C de esto se encarga GCC; importa sobre todo en ensamblador.
- `fdiv` cuesta ~13 ciclos y `fmul` ~3: dividir por una constante es mejor hacerlo multiplicando por el inverso (`x * 0.5f`). Con enteros, GCC ya convierte las divisiones por potencias de dos en desplazamientos.
- **Usar `float`, no `double`**: la FPU está pensada para precisión simple, y KOS compila con `-m4-single`. Cuidado con las constantes sin `f` (`0.5` es `double`).

### Instrucciones vectoriales de la FPU

| Instrucción | Calcula | Latencia |
|---|---|---|
| `fipr FVm,FVn` | producto escalar de dos vectores de 4 floats → `FR[n+3]` | 4/5 ciclos |
| `ftrv XMTRX,FVn` | matriz 4×4 del banco trasero × vector de 4 floats → `FVn` | 5/8 ciclos |
| `fsrra FRn` | aproximación de `1/sqrt(x)` (normalizar vectores) | — |
| `fmac` | `a*b + c` al coste de una operación | — |

- `ftrv` compensa cuando hay **tres o más productos escalares seguidos con un vector fijo**: transformar vértices, o probar una esfera contra 4 planos a la vez (se cargan los 4 planos como filas de `XMTRX` y se pasa el centro con `w = -1` para que reste la distancia de cada plano).
- `fipr` se emite en un ciclo, pero **leer su resultado enseguida bloquea** 4–5 ciclos. La optimización de la wiki para el sintetizador de audio de pl_mpeg intercala la carga de operandos del siguiente `fipr` mientras termina el anterior, usando el banco trasero (`frchg`) para no pisar registros.
- `fmov` de 64 bits (modo `fschg`) mueve parejas de registros y llega al banco trasero, pero los datos deben estar **alineados a 8 bytes** y no conviene cambiar de modo a menudo.
- **SH4ZAM** (kos-ports) envuelve todo esto en C (`shz_vec3_dot`, `shz_xmtrx_load_4x4_rows`, `shz_xmtrx_trans_vec4`...): es preferible a escribir el ensamblador a mano.

### Bucle de transformación e iluminación

En Dreamcast la CPU hace la transformación y la iluminación, y suele ser el cuello de botella en 3D. La receta de la wiki:

1. Medir antes de optimizar.
2. Dividir el bucle grande por vértice en bucles pequeños independientes (difusa, especular, recorte y empaquetado, transformación con división de perspectiva). Así hay menos presión de registros y el pipeline va mejor.
3. Procesarlos por bloques de **~32 vértices**: con menos pesa la sobrecarga del bucle y con más aumentan los fallos de caché.

## Notas / gotchas

- Cambiar `-m4-single` (por ejemplo a `-m4-single-only`, como hace DreamSDK R4) exige que KOS y todos los ports se compilen igual (ver [DreamSDK en Windows](../herramientas/dreamsdk-windows.md)).
- La página "GCC-SH4 tips" de la wiki no trata *flags* de compilación: las opciones (LTO, `-ffast-math`, etc.) están comentadas en `environ.sh`.
- Las latencias vienen de la documentación del SH-4 citada por la wiki, no de mediciones de este proyecto.
- **Perfilar el C portable en PC (Windows):** con el GCC de MinGW, `gprof` (`-pg`) genera `gmon.out` pero sin muestras. Y `-finstrument-functions` infla las funciones pequeñas que se llaman millones de veces: el reparto sale distorsionado. Lo que sí funcionó es un **perfilador por muestreo** de ~40 líneas, enlazado con el programa. Un hilo creado en un `__attribute__((constructor))` hace `Sleep(1)` (con `timeBeginPeriod(1)`), `SuspendThread` del hilo principal, `GetThreadContext` (`CONTEXT_CONTROL`) para leer `Rip`, y `ResumeThread`. Al salir se vuelcan las direcciones. Se reparten por función con la salida de `nm` (búsqueda binaria) y por línea con `addr2line` (compilar con `-g -no-pie`). Ojo: con *computed goto*, GCC en x86 junta el salto indirecto del despacho en un solo punto, que `addr2line` atribuye a la llave de cierre de la función. En el SH-4 el código es otro: el PC sirve para saber *qué* es caro, no *cuánto* lo será en la consola.

## Referencias cruzadas

- [Mapa de memoria](../memoria/mapa-de-memoria.md) — SDRAM, ráfagas y reglas de alineación
- [PowerVR: pipeline y listas](../graficos/powervr-pipeline.md) — envío de vértices por store queues
- [Punto fijo](../../GLOSARIO/fixed-point.md) — en Dreamcast normalmente **no** hace falta: la FPU de precisión simple es rápida
- [Optimizar C con GCC para 68000](../../MegaDrive/cpu/optimizar-c-gcc-68000.md) — la misma idea en otra CPU

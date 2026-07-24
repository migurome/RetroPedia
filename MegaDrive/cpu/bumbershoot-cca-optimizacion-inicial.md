---
titulo: Optimización del bucle principal del CCA en 68000 (2018)
plataforma: [MegaDrive]
categoria: [cpu]
tags: [cca, optimizacion, 68000, variables-de-induccion, punteros, benchmark, bucle]
estado: borrador
fecha: 2026-07-24
fuentes: [https://bumbershootsoft.wordpress.com/2018/04/21/optimizing-the-cca/]
---

# Optimización del bucle principal del CCA en 68000 (2018)

## Resumen

Continuación directa de la primera implementación del CCA (ver referencia cruzada): el autor aplica tres optimizaciones sucesivas al bucle principal en 68000, cada una justificada primero en pseudocódigo C y luego traducida a ensamblador. En conjunto casi quintuplican el rendimiento, de 124 a 27 frames por generación (0.48 → 2.22 "generaciones/segundo" de la simulación).

## Detalle técnico

Punto de partida conceptual (equivalente en C a la primera versión en ASM de la entrada anterior), con una macro `CHECK` que compara un vecino con el estado objetivo y una función `full_check` que evalúa los 4 vecinos de una celda:

```c
#define INDEX(x, y) ((((y) & 0x7f) << 7) | ((x) & 0x7f))
#define CHECK(x, y) if (in[INDEX(x, y)] == target) val = target

void full_check(const char *in, char *out, int x, int y)
{
    int val, target;
    val = in[INDEX(x, y)];
    target = (val + 1) & 0x0f;
    CHECK(x-1, y);
    CHECK(x+1, y);
    CHECK(x, y-1);
    CHECK(x, y+1);
    out[INDEX(x, y)] = val;
}

void cca_step_0(const char *in, char *out)
{
    int x, y;
    for (y = 0; y < 128; ++y) {
        for (x = 0; x < 128; ++x) {
            full_check(in, out, x, y);
        }
    }
}
```

### Optimización 1: separar el núcleo de los bordes

Idea: procesar aparte las filas/columnas 1-126 (que nunca necesitan wraparound) del resto de celdas de borde (que sí lo necesitan), para no pagar en el 98% de las celdas el coste de las máscaras de wraparound (`& 0x7f`) que solo hacen falta en el 2% restante:

```c
void cca_step_1(const char *in, char *out)
{
    int x, y, index, val, target;
    for (y = 1; y < 127; ++y) {
        for (x = 1; x < 127; ++x) {
            index = INDEX(x, y);
            val = in[index];
            target = (val + 1) & 0x0f;
            if (in[index - 128] == target ||
                in[index -   1] == target ||
                in[index +   1] == target ||
                in[index + 128] == target) {
                val = target;
            }
            out[index] = val;
        }
    }
    for (y = 0; y < 128; ++y) {
        full_check(in, out, 0, y);
        full_check(in, out, y, 0);
        full_check(in, out, y, 127);
        full_check(in, out, 127, y);
    }
}
```

Traducción a ASM 68000, aprovechando que el direccionamiento indexado con desplazamiento de 8 bits con signo del 68000 (rango -128 a +127) cubre exactamente los 4 vecinos ortogonales sin necesitar un registro de dirección por vecino:

```asm
        move.w  #125, d2
@lp:    move.w  #125, d3
@lp2:   bsr.s   @index
        add.w   #129, d0
        move.b  (a0, d0), d4
        move.b  d4, d5
        addq    #1, d5
        and.b   #$0f, d5
        move.w  d0, d6
        cmp.b   -128(a0, d0), d5
        beq.s   @eat
        cmp.b   -1(a0, d0), d5
        beq.s   @eat
        cmp.b   1(a0, d0), d5
        beq.s   @eat
        addq    #1, d0
        cmp.b   127(a0, d0), d5
        bne.s   @next
@eat:   move.b  d5, d4
@next:  move.b  d4, (a1, d6)
        dbra    d3, @lp2
        dbra    d2, @lp
```

Resultado: de **124 a 40 frames por generación** (0.48 → 1.5 "gen/s"), ~3.1× más rápido. Es la optimización de mayor impacto de las tres.

### Optimización 2: variables de inducción en vez de recalcular el índice

En vez de llamar a `@index` (que recalcula el offset desde x,y con desplazamientos y máscaras) en cada iteración, se mantienen dos punteros (`in_ptr`/`out_ptr` en C; `a2`/`a3` en ASM) que simplemente se incrementan en cada paso, saltando 2 posiciones extra al llegar al final de cada fila de 128 para saltar los 2 bytes de borde:

```c
void cca_step_2(const char *in, char *out)
{
    const char *in_ptr = in + 129;
    const char *in_end = in_ptr + (126*128);
    char *out_ptr = out + 129;
    int i, val, target;
    while (in_ptr < in_end) {
        val = *in_ptr;
        target = (val + 1) & 0x0f;
        if (*(in_ptr - 128) == target ||
            *(in_ptr -   1) == target ||
            *(in_ptr +   1) == target ||
            *(in_ptr + 128) == target) {
            val = target;
        }
        *out_ptr = val;
        ++in_ptr;
        ++out_ptr;
        if (((in_ptr - in) & 0x7f) == 0x7f) {
            in_ptr += 2;
            out_ptr += 2;
        }
    }
    for (i = 0; i < 128; ++i) {
        full_check(in, out, 0, i);
        full_check(in, out, i, 0);
        full_check(in, out, i, 127);
        full_check(in, out, 127, i);
    }
}
```

```asm
        move.w  #(126*126-1), d2
        movea.l a0, a2
        add.w   #129, a2
        movea.l a1, a3
        add.w   #129, a3
@lp:    move.b  (a2), d4
        move.b  d4, d5
        addq    #1, d5
        and.b   #$0f, d5
        cmp.b   -128(a2), d5
        beq.s   @eat
        cmp.b   -1(a2), d5
        beq.s   @eat
        cmp.b   1(a2), d5
        beq.s   @eat
        cmp.b   128(a2), d5
        bne.s   @next
@eat:   move.b  d5, d4
@next:  move.b  d4, (a3)
        addq    #1, a2
        addq    #1, a3
        move.l  a2, d0
        and.w   #$7f, d0
        cmp.w   #$7f, d0
        bne.s   @lpend
        addq    #2, a2
        addq    #2, a3
@lpend: dbra    d2, @lp
```

Resultado: de **40 a 30 frames por generación** (1.5 → 2.0 "gen/s"). El coste de la comprobación de fin de fila (6 instrucciones extra) es menor que el de recalcular el índice completo cada vez.

### Optimización 3: "hacer trampa" quitando la comprobación de fin de fila

El bloque final de la optimización 2 (comprobar si el puntero llegó al final de una fila de 128 celdas y saltar 2 posiciones de borde) se ejecuta en cada una de las 15876 celdas del núcleo, pero solo importa su resultado en 126 de esas iteraciones (una por fila). El autor razona que sale más barato dejar que el puntero se "cuele" por los 2 bytes de borde de cada fila —procesándolos con el cálculo normal, con resultado incorrecto para esas 2 celdas— porque el segundo bucle (el que trata los bordes por separado) sobrescribe correctamente esas mismas celdas justo después. Se elimina así por completo el bloque de comprobación:

```asm
        move.l  a2, d0
        and.w   #$7f, d0
        cmp.w   #$7f, d0
        bne.s   @lpend
        addq    #2, a2
        addq    #2, a3
```

Resultado: de **30 a 27 frames por generación** (2.0 → 2.22 "gen/s").

## Notas / gotchas

- Patrón común a las tres optimizaciones: cuanto más veces se ejecuta una instrucción (aquí hasta 16384 veces por generación), más rentable es eliminarla del camino caliente, aunque complique la lógica de los bordes.
- La optimización 3 es un ejemplo de "dejar que ocurra un resultado temporalmente incorrecto a propósito": es válido procesar de más con un resultado erróneo si un paso posterior garantiza sobrescribirlo antes de que se use en ningún otro sitio.
- Separar núcleo de bordes (optimización 1) fue la de mayor impacto (~3.1×) porque elimina el coste de las máscaras de wraparound en el 98% de las celdas (126×126 de 128×128); las optimizaciones 2 y 3 dan ganancias más modestas porque atacan un bucle ya reducido a lo esencial.
- El direccionamiento indexado con desplazamiento de 8 bits con signo del 68000 (offsets de -128 a +127) encaja justo con las distancias a los 4 vecinos ortogonales de esta rejilla, evitando calcular direcciones aparte para cada uno.

## Referencias cruzadas

- [Primer diseño e implementación del CCA en Genesis](bumbershoot-cca-primera-implementacion.md) — punto de partida de este bucle (124 frames/generación) y contexto completo del proyecto.
- [Re-optimización del CCA en Genesis en 2025](bumbershoot-cca-reoptimizacion-2025.md) — retoma exactamente este mismo bucle (ya reducido a 27 frames/generación) siete años después, con contabilidad de ciclos y técnicas adicionales (LEA, macros inline, offsets constantes en esquinas).
- [Motorola 68000 — instrucciones y modos de direccionamiento (base)](68000-instrucciones-modos-direccionamiento.md) — referencia de los modos de direccionamiento (indexado con desplazamiento, `dbra`) usados en las tres optimizaciones.

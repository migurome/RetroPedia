---
titulo: SGDK — Tutorial de Matemáticas (fix16/fix32)
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, tutorial, matematicas, punto-fijo, fix16, fix32, trigonometria]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/Tuto-Maths]
---

# SGDK — Tutorial de Matemáticas (fix16/fix32)

## Resumen

SGDK ofrece dos tipos de punto fijo virtuales (`fix16`, `fix32`) que simulan operaciones en coma flotante usando almacenamiento entero, evitando la penalización de rendimiento de emular floats nativos en el 68000. También incluye funciones trigonométricas y de raíz/logaritmo basadas en tablas.

## Detalle técnico

### Tipos de punto fijo

| Tipo | Bit de signo | Bits enteros | Bits fraccionarios | Rango |
|---|---|---|---|---|
| fix32 | 1 | 21 | 10 | -2097152.000 a 2097151.000 |
| fix16 | 1 | 9 | 6 | -512.00 a 511.00 |

Internamente usan `s16` (fix16) y `s32` (fix32) respectivamente.

### Declaración y asignación

Declaración de variable:
```c
fix16 value_f16;
fix32 value_f32;
```

Desde constantes:
```c
value_f16 = FIX16(10.5);
value_f32 = FIX32(3456.234);
```

Desde enteros:
```c
value_f16 = FIX16(value_s16);
value_f32 = FIX32(value_s32);
```

### Conversión de tipo

```c
// A entero
value_s16 = F16_toInt(value_f16);
value_s32 = F32_toInt(value_f32);

// Conversión redondeada
value_s16 = F16_toRoundedInt(value_f16);
value_s32 = F32_toRoundedInt(value_f32);

// Entre tipos fix
value_f16 = F32_toFix16(value_f32);
value_f32 = F16_toFix32(value_f16);
```

### Operaciones matemáticas básicas

```c
// Suma y resta (los operadores estándar funcionan)
f = f1 + f2;
f = f1 - f2;

// Multiplicación y división (requieren llamada a función)
f = F16_mul(f1, f2);
f = F16_div(f1, f2);

// Extracción de componentes
f = F16_frac(f1);  // parte fraccionaria
f = F16_int(f1);   // parte entera
f = F16_round(f1); // redondeado (salida fix16)
i = F16_toRoundedInt(f1); // redondeado (salida entera)
```

Nota: existen funciones equivalentes para fix32 (`F32_mul`, `F32_div`, etc.).

### Operaciones matemáticas avanzadas

Disponibles solo para fix16, por requerir tablas de lookup:
```c
f = F16_log2(f1);
f = F16_log10(f1);
f = F16_sqrt(f1);
```

### Funciones trigonométricas

```c
// Versiones fix16
F16_sin(v)
F16_cos(v)
F16_tan(v)
F16_atan(v)
F16_atan2(y, x)

// Versiones fix32 (limitadas)
F32_sin(v)
F32_cos(v)
```

Todas estas utilidades matemáticas residen en el módulo "maths" (archivos `maths.h` y `maths.c`).

## Notas / gotchas

- Comparar con el concepto genérico de punto fijo y las tablas de seno/coseno precalculadas descritas en [`../../../GLOSARIO/fixed-point.md`](../../../GLOSARIO/fixed-point.md) y [`../../../GLOSARIO/angle-math.md`](../../../GLOSARIO/angle-math.md) (fuente plutiedev.com) — SGDK ya trae esto implementado como librería, sin necesidad de escribirlo a mano en ASM.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Tutorial: Input](tuto-input.md)

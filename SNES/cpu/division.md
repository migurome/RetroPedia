---
titulo: División en SNES (hardware de división del 5A22)
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, division, hardware, registros]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Division]
---

# División en SNES (hardware de división del 5A22)

## Resumen
El 65816 carece de instrucción nativa de división, pero el chip 5A22 de la SNES incluye un divisor por hardware de 16 bits entre 8 bits accesible mediante registros mapeados en memoria. Es la vía habitual para dividir sin implementar rutinas de software.

## Detalle técnico

**Registros de entrada (escritura):**

| Registro | Dirección | Propósito |
|----------|-----------|-----------|
| WRDIVL | $4204 | Byte bajo del dividendo (16 bits) |
| WRDIVH | $4205 | Byte alto del dividendo (16 bits) |
| WRDIVB | $4206 | Divisor (8 bits sin signo); escribir aquí inicia la operación |

**Registros de salida (lectura):**

| Registro | Dirección | Propósito |
|----------|-----------|-----------|
| RDDIVL | $4214 | Byte bajo del cociente (16 bits) |
| RDDIVH | $4215 | Byte alto del cociente (16 bits) |
| RDMPYL | $4216 | Byte bajo del residuo (16 bits) |
| RDMPYH | $4217 | Byte alto del residuo (16 bits) |

La escritura en WRDIVB dispara el proceso de división. Esta requiere 16 ciclos de CPU para completarse, sin importar si el sistema funciona a 2.68 MHz o a 3.58 MHz. Igual que con la multiplicación, el tiempo que consume la propia instrucción de lectura del resultado cuenta hacia esa espera: un `LDA absolute` consume 3 ciclos antes de leer, y un `LDA long` consume 4.

**Limitaciones críticas:**
- División por cero: el resultado del cociente es $FFFF y el residuo queda igual al dividendo original.
- Los registros de multiplicación sin signo comparten hardware con la división: escribir en WRMPYB reemplaza el valor en RDDIVL, y pone a cero RDDIVH.
- No se puede iniciar una nueva división mientras otra está en curso; hacerlo corrompe el resultado.

La página fuente menciona la existencia de técnicas de "multiplicación y división de 16 bits" por software para operandos más grandes que los que soporta el hardware (16÷8), pero no incluye código de ejemplo para ese caso.

## Notas / gotchas
- El hardware de división solo admite dividendo de 16 bits y divisor de 8 bits; para divisiones de 16÷16 o mayores hace falta una rutina de software (no detallada en esta página fuente).
- Los 16 ciclos de latencia son fijos independientemente de la velocidad de reloj de acceso a memoria (2.68 MHz vs 3.58 MHz), a diferencia de otros timings de la consola que sí varían con eso.
- Cuidado con el registro compartido: si un código escribe WRMPYB para multiplicar justo después de haber iniciado una división (o viceversa), los resultados de ambas operaciones se corrompen porque usan el mismo bloque de hardware/registros de resultado.
- Recordar restar el propio coste de la instrucción de lectura (LDA absolute = 3 ciclos, LDA long = 4 ciclos) al calcular cuántos ciclos de NOP/trabajo útil hacen falta para cubrir la latencia total de 16 ciclos.

## Referencias cruzadas
- (pendiente de enlazar) — ver también la entrada de multiplicación (comparten registros RDMPYL/RDMPYH).

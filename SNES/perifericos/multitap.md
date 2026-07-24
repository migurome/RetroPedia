---
titulo: Multitap de SNES
plataforma: [SNES]
categoria: [perifericos]
tags: [multitap, mando, multijugador, io, protocolo]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Multitap]
---

# Multitap de SNES

## Resumen

El Multitap oficial de SNES es un accesorio que amplía la consola a 5 jugadores: se conecta al puerto 2 y ofrece 4 puertos adicionales, mientras el puerto 1 sigue aceptando un mando estándar normal. Incluye un interruptor 2/5 jugadores que, en posición "2", lo convierte en un simple pasante para un segundo mando. Esta entrada documenta cómo el sistema selecciona y lee los mandos adicionales.

## Detalle técnico

### Restricción oficial de uso combinado

El manual de desarrollo de SNES desaconseja combinar el multitap con un ratón, un Super Scope o un segundo multitap en la misma sesión, debido al consumo eléctrico excesivo y a incompatibilidades de interfaz entre esos periféricos.

### Selección de puertos

El bit alto de WRIO ($4201) controla qué par de puertos del multitap queda activo:

- `$4201.7 = 1`: selecciona los mandos de los puertos 2 y 3.
- `$4201.7 = 0`: selecciona los mandos de los puertos 4 y 5.

Se recomienda mantener `$4201.7 = 1` la mayor parte del tiempo, de forma que la lectura automática estándar del hardware pueda usarse directamente para los mandos 2 y 3.

### Distribución de las lecturas

| Mando | Registro | Bits | Método |
|-------|----------|------|--------|
| 1 | $4218/$4219 (JOY1) | 0-15 | Automática |
| 2 | $4017 D0 | 0-15 | Automática (JOY2) |
| 3 | $4017 D1 | 0-15 | Automática (JOY4) |
| 4 | $4017 D0 | 17-32 | Serie manual |
| 5 | $4017 D1 | 17-32 | Serie manual |

El bit 17 de cada reporte de 32 bits indica si hay un mando físicamente conectado en ese puerto (1 = conectado, 0 = desconectado).

### Secuencia de lectura recomendada por cuadro

1. Configurar `$4201.7 = 1` (selecciona puertos 2/3).
2. Dejar que la lectura automática de hardware complete los primeros 16 bits.
3. Esperar a que termine, comprobando el bit 0 de HVBJOY.
4. (Opcional) Leer el bit 17 para detectar si el mando 2/3 está conectado.
5. Configurar `$4201.7 = 0` (selecciona puertos 4/5).
6. Leer manualmente el reporte de 16 bits correspondiente.
7. (Opcional) Leer el bit 17 de esos mandos.
8. Restaurar `$4201.7 = 1` antes del siguiente cuadro, para que la lectura automática vuelva a funcionar sobre los puertos 2/3.

### Procedimiento oficial de detección del multitap

1. Escribir 1 en el bit 0 de $4016.
2. Leer el bit 1 de $4017 ocho veces; debe devolver `$FF` (ocho unos) mientras el strobe está activo.
3. Escribir 0 en el bit 0 de $4016.
4. Leer el bit 1 de $4017 ocho veces de nuevo; tras liberar el strobe debe devolver cualquier valor de 8 bits salvo `$FF`.

## Notas / gotchas

- No combinar el multitap con ratón, Super Scope o un segundo multitap: el manual oficial lo prohíbe explícitamente por consumo de corriente e incompatibilidad de interfaz.
- Algunos multitaps de terceros presentan una transición lenta al pasar `$4201.7` de 0 a 1, por lo que conviene leer primero los mandos de los puertos 2/3 (donde se aprovecha la lectura automática) y dejar el cambio de bit para el final del ciclo de lectura.
- El bit 17 es la única forma fiable de saber si hay un mando realmente enchufado en un puerto del multitap, ya que un puerto vacío no necesariamente reporta todo en cero.

## Referencias cruzadas

- (pendiente de enlazar)

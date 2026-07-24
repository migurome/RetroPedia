---
titulo: Coma flotante explicada visualmente
plataforma: [General]
categoria: [arquitectura]
tags: [coma-flotante, ieee754, punto-fijo, precision-numerica, representacion-numeros]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/floating_point_visually_explained/index.html]
---

# Coma flotante explicada visualmente

## Resumen

El autor, escribiendo originalmente sobre Wolfenstein 3D, buscaba explicar por qué trabajar sin coma flotante era una desventaja real para los desarrolladores de la época. Al no encontrarse cómodo con las explicaciones canónicas de IEEE 754 (que dependen de notación matemática poco intuitiva), propone una explicación visual alternativa basada en "ventanas" y "desplazamientos" en vez de exponente/mantisa. Relevante para decidir, en el diseño de un motor, cuándo usar coma flotante frente a punto fijo.

## Detalle técnico

### La explicación tradicional (y por qué el autor la rechaza)

Un float de 32 bits (IEEE 754) se divide en tres secciones:

- **1 bit S**: signo
- **8 bits E**: exponente
- **23 bits**: mantisa (fracción)

La fórmula tradicional para reconstruir el valor es:

```
(-1)^S * 1.M * 2^(E-127)
```

El autor reconoce explícitamente que "esta forma de explicar punto flotante deja a algunos completamente sin entender", y de ahí su motivación para buscar una vía visual alternativa.

### El enfoque visual alternativo: ventanas y desplazamientos

En lugar de pensar en términos de exponente y mantisa por separado, el autor propone dos conceptos:

- **Ventana**: el rango de valores entre dos potencias consecutivas de 2. Ejemplos de ventanas: [0.5, 1], [1, 2], [2, 4], [4, 8]... hasta [2^127, 2^128].
- **Desplazamiento (offset)**: dentro de cada ventana, esta se subdivide en **2^23 = 8.388.608** segmentos iguales. El desplazamiento indica en qué segmento, dentro de la ventana activa, cae el número representado.

Esta forma de pensarlo hace explícitas dos propiedades del formato:

- **Protección frente a desbordamiento**: cuando se alcanza el valor máximo de una ventana, el número simplemente "flota" a la ventana siguiente (de ahí el nombre "punto flotante").
- **Coste de esa protección**: una pequeña pérdida de precisión, porque la ventana siguiente es el doble de ancha que la anterior, por lo que el mismo número de segmentos (2^23) debe cubrir un rango dos veces mayor.

### Análisis de precisión según la ventana

La precisión (el tamaño de cada segmento) depende directamente del ancho de la ventana activa:

- **Ventana [1, 2]** (ancho = 1): precisión = (2-1)/8.388.608 = **0,00000011920929**
- **Ventana [2048, 4096]** (ancho = 2048): precisión = 2048/8.388.608 = **0,0002**

Es decir, la precisión absoluta disponible decrece (empeora) de forma proporcional al tamaño de la ventana en la que cae el número — la razón por la que números grandes en coma flotante pierden precisión decimal frente a números pequeños, mientras que la precisión *relativa* se mantiene aproximadamente constante en todas las ventanas.

### Ejemplo introductorio: representar 6,1

- La ventana debe empezar en 4 y llegar hasta 8 (la ventana [4, 8], ya que 6,1 cae dentro de ese rango).
- El desplazamiento correspondiente cae aproximadamente a mitad de esa ventana.

### Ejemplo completo y detallado: representar π (3,14)

Cálculo paso a paso dado por el autor:

1. **Signo**: 3,14 es positivo → S = 0.

2. **Ventana**: 3,14 está entre 2^1 = 2 y 2^2 = 4 → esto corresponde a un exponente almacenado E = 128 (usando la fórmula inversa: 2^(E-127) = 2^(128-127) = 2^1 = 2, el inicio de la ventana).

3. **Desplazamiento dentro de la ventana [2, 4]**:
   - Posición relativa dentro de la ventana = (3,14 - 2) / (4 - 2) = 1,14 / 2 = **0,57**
   - Mantisa M = 2^23 × 0,57 = **4.781.507**

**Representación binaria resultante:**

- S = 0 = `0b`
- E = 128 = `10000000b`
- M = 4.781.507 = `10010001111010111000011b`

**Valor reconstruido aproximado**: 3,1400001049041748046875 (el error de redondeo inherente a que 0,57 no cae exactamente en un múltiplo entero de 1/8.388.608).

**Verificación con la fórmula tradicional:**

```
(-1)^0 × 1.57 × 2^(128-127) = 3.14  (aproximado)
```

Ambos caminos — el visual (ventana + desplazamiento) y el tradicional (signo/exponente/mantisa) — llegan al mismo resultado numérico; la aportación del autor es puramente de intuición pedagógica, no un formato nuevo.

## Notas / gotchas

- El artículo no es una crítica al formato IEEE 754 en sí, sino a la forma habitual de *explicarlo*: el autor considera que pensar en "ventanas entre potencias de 2, subdivididas en 2^23 segmentos" es más intuitivo que memorizar la fórmula `(-1)^S · 1.M · 2^(E-127)`, aunque ambos describen exactamente el mismo mecanismo.
- La pérdida de precisión al pasar a una ventana más ancha (por ejemplo, de [1,2] a [2048,4096]) es la razón intuitiva, en este marco visual, de por qué sumar un número muy pequeño a uno muy grande en coma flotante puede no tener ningún efecto: el pequeño puede caer muy por debajo de la resolución del segmento en la ventana del número grande.
- Relevancia directa para diseño de motor: en una plataforma sin FPU (como los 68000/Z80 de Mega Drive), esta explicación visual es un argumento intuitivo para preferir aritmética de punto fijo en rutinas críticas (física, posiciones, cámaras) — con punto fijo el "tamaño de ventana" es constante en todo el rango representable, evitando la pérdida de precisión variable inherente a la coma flotante, a cambio de un rango dinámico menor.
- El artículo no profundiza en casos especiales de IEEE 754 (infinito, NaN, denormales, cero con signo) — el foco del artículo extraído es exclusivamente la intuición de ventana/desplazamiento y el ejemplo numérico de π; no se documentan aquí porque no aparecen desarrollados en el contenido fuente disponible.

## Referencias cruzadas

- (pendiente de enlazar)

---
titulo: FizzleFade — la transición de pantalla de Wolfenstein 3D
plataforma: [General]
categoria: [efectos]
tags: [wolfenstein3d, fizzlefade, lfsr, transiciones-pantalla, pseudoaleatorio]
estado: borrador
fecha: 2026-07-21
fuentes: [https://fabiensanglard.net/fizzlefade/index.php]
---

# FizzleFade — la transición de pantalla de Wolfenstein 3D

## Resumen

El "FizzleFade" es el efecto de transición de Wolfenstein 3D que convierte progresivamente la pantalla a un color sólido, píxel a píxel, en un orden que parece aleatorio pero que en realidad es determinista. Se usa en dos momentos del juego: al morir el jugador y al matar a un jefe final. El artículo explica por qué un generador aleatorio convencional no sirve para esto y cómo un LFSR (Linear Feedback Shift Register) resuelve el problema con un coste mínimo — relevante para cualquier motor que necesite efectos de transición deterministas y baratos en CPU.

## Detalle técnico

### El problema: por qué "aleatorio puro" no funciona

Un enfoque ingenuo (generar coordenadas X,Y con un generador pseudoaleatorio normal) presenta dos problemas serios:

1. **No determinismo respecto a la duración**: "would make the fade non-deterministic with regard to duration" — no hay garantía de cuántas iteraciones hacen falta para cubrir toda la pantalla, porque un generador aleatorio convencional puede repetir la misma coordenada muchas veces antes de cubrir todas.
2. **Desperdicio de ciclos de CPU**: "would also waste CPU cycles since the same pixel coordinates (X,Y) could come up several times" — cada repetición de una coordenada ya pintada es trabajo perdido.

Lo que se necesita es una secuencia que visite **cada coordenada de pantalla exactamente una vez**, en un orden que parezca aleatorio al ojo humano, y que termine en un número de pasos exactamente igual al número de píxeles.

### La solución: LFSR (Linear Feedback Shift Register)

Un LFSR de longitud máxima con los "taps" (posiciones de retroalimentación) correctamente elegidos recorre **todos** los valores distintos posibles (excepto el cero) antes de repetirse, sin repetir ninguno por el camino. Eso es exactamente la propiedad que hace falta: determinismo total + cobertura completa sin repeticiones.

El artículo distingue dos formas de implementar un LFSR:

- **Configuración Fibonacci (conceptual)**: se muestran los "taps" en posiciones específicas del registro; para calcular el nuevo bit hacen falta varias operaciones XOR encadenadas entre los bits de esas posiciones.
- **Configuración Galois (la implementación práctica)**: "requires only one XOR regardless of the number of taps" — solo hace falta una operación XOR (con una máscara constante) independientemente de cuántos taps tenga el polinomio. Esta es la estrategia que usa Wolfenstein 3D.

### Código fuente (equivalente en C)

```c
boolean fizzlefade(void)
{
  uint32_t rndval = 1;
  uint16_t x,y; 
  do
  {
     y = rndval & 0x000FF;      /* Y = low 8 bits */
     x = (rndval & 0x1FF00) >> 8; /* X = High 9 bits */
     unsigned lsb = rndval & 1;   /* Get output bit */
     rndval >>= 1;                /* Shift register */
     if (lsb) {
          rndval ^= 0x00012000;
     }
     if (x < 320 && y < 200)
       fizzle_pixel(x , y) ;
  } while (rndval != 1);
  return 0;
}
```

### Parámetros técnicos exactos

| Parámetro | Valor |
|-----------|-------|
| Resolución objetivo | 320×200 píxeles = 64.000 píxeles totales |
| Longitud del registro | 17 bits |
| Valor inicial (`rndval`) | 1 |
| Máscara para Y | `0x000FF` (8 bits bajos) |
| Extracción de X | `(rndval & 0x1FF00) >> 8` (9 bits) |
| Máscara XOR (taps Galois) | `0x00012000` |
| Condición de parada | `rndval != 1` (el ciclo termina al volver al valor inicial) |
| Límite X | 320 |
| Límite Y | 200 |

### Cómo funciona la iteración, paso a paso

En cada vuelta del bucle `do...while`:

1. Se extraen los 8 bits inferiores de `rndval` → coordenada **Y**.
2. Se extraen los bits 8 a 16 (9 bits) de `rndval`, desplazados → coordenada **X**.
3. Se obtiene el bit menos significativo (LSB) de `rndval`, que será el "bit de salida" del registro.
4. Se desplaza `rndval` un bit a la derecha (`>>= 1`).
5. Si el LSB obtenido en el paso 3 era 1, se aplica un XOR con la máscara `0x00012000` sobre `rndval` — este es el paso de retroalimentación (feedback) del LFSR en su forma Galois.
6. Se comprueba que `(x,y)` esté dentro de los límites de pantalla (320×200); si lo está, se pinta ese píxel.
7. El bucle continúa hasta que `rndval` vuelve a valer 1 (el valor inicial), momento en que se ha completado el ciclo máximo del registro.

Como el registro es de 17 bits, genera **131.072 valores** distintos antes de repetirse (2^17 - 1, descontando el cero que es un estado absorbente en LFSR). Dado que 320×200 = 64.000 (menos de la mitad de 131.072), muchos de esos valores generados caen fuera de los límites de pantalla y simplemente se descartan (paso 6) sin pintar nada — pero el recorrido determinista garantiza que, al final, cada una de las 64.000 coordenadas válidas se habrá visitado exactamente una vez.

### Por qué 17 bits y no 16

El artículo señala que, dado que 320×200 = 64.000 y 2^16 = 65.536, técnicamente **un registro de 16 bits ya habría bastado**: "it could have been implemented with a 16 bits Maximum-length register". El autor especula sobre por qué id Software usó 17 bits en su lugar: "My assumption is that LFSR literature was hard to come across in 1991/1992" — es decir, probablemente los desarrolladores no tenían fácil acceso en esa época a las tablas de polinomios de longitud máxima para 16 bits concretamente, y usaron una configuración de 17 bits que sí tenían documentada.

### Ubicación en el código original y dificultad de portabilidad

- La función original se llama `FizzleFade`, ubicada en `id_vh.cpp` del código fuente de Wolfenstein 3D.
- Su propósito es un fade determinista que escribe cada píxel exactamente una vez.
- El artículo señala que fue difícil de replicar en ports posteriores: "None of the ports managed to replicate the fizzlefade except Wolf4SDL, which found a LFSR taps configuration to reach resolution higher than 320x200" — es decir, casi ningún port logró reproducir fielmente el efecto original; Wolf4SDL fue la excepción, encontrando una configuración de taps de LFSR distinta que permite cubrir resoluciones mayores a 320×200 manteniendo la misma propiedad de cobertura completa sin repeticiones.

## Notas / gotchas

- La clave del truco no es "generar números aleatorios" sino generar una **permutación determinista de todas las coordenadas posibles**, usando aritmética de registro de desplazamiento con retroalimentación (XOR) en vez de un PRNG convencional.
- La forma Galois del LFSR es preferible a la forma Fibonacci precisamente porque solo requiere una operación XOR por iteración, sin importar cuántos "taps" tenga el polinomio — importante en hardware limitado en ciclos.
- Elegir un registro más grande de lo estrictamente necesario (17 bits para cubrir 64.000 valores, cuando 16 bits ya habrían bastado) no es gratis: se generan más valores "fuera de rango" que se descartan sin pintar nada, pero simplifica el problema si no se tiene a mano la tabla de polinomios exacta para el tamaño mínimo.
- Portar este efecto a otra resolución o relación de aspecto requiere encontrar una nueva configuración de taps (polinomio) que sea de longitud máxima para ese nuevo espacio de valores — no es trivial, como demuestra que solo Wolf4SDL lo consiguiera entre los ports mencionados.
- Contexto histórico: la técnica proviene de una época (1991-1992) en la que la documentación sobre LFSR no era tan accesible como hoy, lo que según el autor explica algunas de las decisiones aparentemente subóptimas (como el tamaño de registro).

## Referencias cruzadas

- (pendiente de enlazar)

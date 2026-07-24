---
titulo: Números en punto fijo (Fixed Point)
plataforma: [General]
categoria: [fundamentos]
tags: [matematicas, punto-fijo, fixed-point, 68000, precision]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/fixed-point]
---

# Números en punto fijo (Fixed Point)

## Resumen

El punto fijo es una técnica aritmética usada como alternativa a la coma flotante en procesadores como el 68000, que no tienen FPU. Permite representar valores fraccionarios usando únicamente aritmética entera, mejorando el rendimiento en hardware retro.

## Detalle técnico

### Concepto central

La idea fundamental es: en lugar de que 1 equivalga a una unidad, se usa un número más grande para representar una unidad, y los números más pequeños se refieren a cantidades de sub-unidad. Por ejemplo, si 1000 representa una unidad, entonces 250 equivale a 0.25 unidades.

### Formato estándar 16.16

El formato más habitual en el 68000 usa un componente entero de 16 bits y un componente fraccionario de 16 bits. En esta configuración, una unidad equivale a 65536 (en hexadecimal: `$10000`). El término "punto fijo" viene de que la posición del punto decimal permanece constante, a diferencia de la coma flotante, donde la precisión fraccionaria varía según la magnitud.

### Operaciones soportadas

Estas operaciones funcionan igual tanto para enteros como para valores en punto fijo:
- Copia de valores
- Suma y resta
- Operaciones de comparación
- Operaciones binarias (AND, OR, desplazamientos de bits)
- Implementaciones con y sin signo

### Conversión a entero

Para extraer la parte entera, se divide por el valor de una unidad — típicamente mediante un desplazamiento de bits a la derecha cuando se usan potencias de dos. Con el formato 16.16, leer 16 bits de memoria da la parte entera; leer 32 bits da el valor completo en punto fijo.

### Mecanismo de redondeo

La conversión estándar redondea hacia abajo. Para redondeo al entero más cercano, se suma 0.5 unidades antes de truncar la parte fraccionaria. Usando específicamente el formato 16.16: se suma 32768, y luego se descartan los 16 bits inferiores.

### Advertencias sobre multiplicación y división

Estas operaciones desplazan la posición del punto decimal, generando problemas críticos ("gotchas"):

- **Multiplicación**: realizar la operación primero, y luego dividir por el valor de una unidad; hay que preservar los resultados intermedios para evitar truncamientos.
- **División**: multiplicar el primer operando por el valor de una unidad antes de dividir; el segundo y tercer operando quedan sin cambios.
- Un desplazamiento de bits a la derecha de 16 posiciones divide por el valor de una unidad en formato 16.16.
- Los segundos operandos que son enteros no necesitan conversión de unidad, tanto en multiplicación como en división.

El autor enfatiza: evitar estas operaciones (multiplicación/división) cuando sea posible, debido a las limitaciones de rendimiento del 68000.

## Notas / gotchas

- Multiplicar o dividir dos valores en punto fijo directamente sin ajustar el punto decimal produce resultados incorrectos: hay que recordar dividir tras multiplicar, y multiplicar antes de dividir.
- El 68000 es lento para multiplicación/división, así que el autor recomienda evitarlas cuando sea posible en cálculos de punto fijo.
- Preservar los resultados intermedios (no truncar demasiado pronto) es importante para no perder precisión.

## Referencias cruzadas

- (pendiente de enlazar)

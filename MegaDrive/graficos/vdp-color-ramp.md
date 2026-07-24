---
titulo: Rampa de color del VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, referencia, cram, paletas, color]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/vdp-color-ramp]
---

# Rampa de color del VDP

## Resumen

Documenta la curva real de salida de color del VDP de Mega Drive, que no es lineal ni logarítmica, sino una curva más aplanada cerca del centro que en los extremos. No tener esto en cuenta puede hacer que imágenes oscuras parezcan más brillantes de lo previsto y viceversa. Incluye las tablas de niveles de salida para modo 5 (Mega Drive) y modo 4 (Master System / compatibilidad).

## Detalle técnico

### Rampa en modo 5

En una escala de 0 a 255, los valores aproximados son: 0, 29, 52, 70, 87, 101, 116, 130, 144, 158, 172, 187, 206, 228, 255. Los colores normales son cada dos valores, los colores shadow son los primeros ocho valores, y los colores highlight son los últimos ocho valores.

Nivel de salida para colores del VDP ("Output level for VDP colors"):

| CRAM value | Normal | Shadow | Highlight |
|---|---|---|---|
| 0 | 0 | 0 | 130 |
| 2 | 52 | 29 | 144 |
| 4 | 87 | 52 | 158 |
| 6 | 116 | 70 | 172 |
| 8 | 144 | 87 | 187 |
| A | 172 | 101 | 206 |
| C | 206 | 116 | 228 |
| E | 255 | 130 | 255 |

En hardware, estos valores representan voltajes de 0V a 1V (donde 255 = 1V).

### Rampa en modo 4

**Advertencia del autor:** las rampas pueden ser ligeramente distintas por componente, y esto además difiere por revisión de VDP, así que esta información debe considerarse desactualizada. Aun así estas rampas son mucho más precisas que rampas lineales, por lo que se pueden seguir usando como referencia mientras tanto.

La rampa del modo 4 es mucho más cercana a lineal, con 1/3 siendo ligeramente más brillante y 2/3 siendo casi perfecto (en una Master System). Los niveles exactos varían ligeramente entre revisiones de Master System y la Mega Drive.

- **Master System (early):** 0, 90, 173, 255
- **Master System (later):** 0, 89, 174, 255
- **Mega Drive:** 0, 99, 162, 255

La Mega Drive tiene el mismo "aplastamiento" que tiene en modo 5, pero no es ni de lejos tan pronunciado en este caso.

### Explicación técnica

Esta curva extraña parece haber sido no intencional: mirar el die del VDP de Mega Drive muestra que los tres DAC son resistencias con múltiples tomas (una por cada valor posible), y que las tomas están espaciadas de forma uniforme. Parece que no tuvieron en cuenta efectos analógicos que distorsionan los valores que salen de ahí.

El modo 4 también tiene dos tomas dedicadas para sus niveles de salida de 1/3 y 2/3, lo que probablemente agravó el efecto mencionado.

## Notas / gotchas

- La rampa de color NO es lineal ni logarítmica; usar una interpolación lineal para fades o degradados producirá resultados visualmente incorrectos respecto al hardware real.
- Los valores de la rampa de modo 4 varían por revisión de hardware (Master System temprana, Master System tardía, Mega Drive) y el propio autor los marca como "información desactualizada", aunque siguen siendo más precisos que asumir una rampa lineal.
- El efecto de "aplastamiento" de la curva existe tanto en modo 5 como en modo 4 en Mega Drive, aunque mucho menos pronunciado en modo 4.
- La causa es un efecto analógico no intencional en el diseño del DAC (resistencias con tomas espaciadas uniformemente que no tuvieron en cuenta la distorsión resultante).

## Referencias cruzadas

- (pendiente de enlazar)

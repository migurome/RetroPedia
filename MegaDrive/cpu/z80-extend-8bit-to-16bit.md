---
titulo: Extender un valor de 8 bits a 16 bits en Z80
plataforma: [MegaDrive]
categoria: [cpu]
tags: [z80, ensamblador, aritmetica, signo, 8-bit, 16-bit]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/z80-extend-8bit-to-16bit]
---

# Extender un valor de 8 bits a 16 bits en Z80

## Resumen

El Z80 no tiene una instrucción nativa para extender un valor de 8 bits a 16 bits. Esta entrada muestra dos técnicas en ensamblador para lograrlo: una para valores sin signo (unsigned) y otra para valores con signo (signed), ambas aprovechando particularidades del set de instrucciones del Z80.

## Detalle técnico

### Extensión sin signo (unsigned)

El procedimiento es directo: se almacena el valor en el byte bajo y se fuerza a cero el byte alto.

```
ld  l, a  ; Store low byte
ld  h, 0  ; High byte always 0
```

### Extensión con signo (signed)

Este método es más complejo pero igualmente accesible. El byte bajo se copia sin cambios. Luego se desplaza el bit de signo hacia el acarreo (carry) mediante `add a, a`, y se explota la instrucción `sbc a` para obtener 0 o -1 (realizando A - A - acarreo), valor que se almacena en el byte alto.

```
ld  l, a  ; Store low byte
add a, a  ; Push sign into carry
sbc a     ; Turn it into 0 or -1
ld  h, a  ; Store high byte
```

Ambas técnicas aprovechan características específicas del procesador Z80 para lograr resultados eficientes.

## Notas / gotchas

- No hay advertencias adicionales indicadas por el autor más allá de las propias particularidades de cada técnica descritas arriba.

## Referencias cruzadas

- (pendiente de enlazar)

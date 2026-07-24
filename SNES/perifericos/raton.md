---
titulo: Ratón de SNES (Super NES Mouse)
plataforma: [SNES]
categoria: [perifericos]
tags: [raton, mouse, io, protocolo, mario-paint]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Mouse]
---

# Ratón de SNES (Super NES Mouse)

## Resumen

El Super NES Mouse (modelo SNS-016) es un periférico apuntador que se distribuyó originalmente junto con *Mario Paint*. A diferencia del mando estándar (16 bits), el ratón entrega un paquete de 32 bits que incluye botones, sensibilidad y desplazamiento relativo en X/Y codificado en signo-magnitud. También existe un clon posterior, el "Hyper Click Retro Style Mouse" de Hyperkin, mayormente compatible pero con requisitos de temporización más estrictos.

## Detalle técnico

### Formato del paquete (32 bits, 4 bytes, MSB primero)

| Byte | Bits | Contenido |
|------|------|-----------|
| 1 | 7-0 | Siempre cero: `00000000` |
| 2 | 3-0 | Firma: `0001` |
| 2 | 5-4 | Sensibilidad (0=baja, 1=media, 2=alta) |
| 2 | 6 | Botón izquierdo (1=presionado) |
| 2 | 7 | Botón derecho (1=presionado) |
| 3 | 6-0 | Desplazamiento vertical (magnitud) |
| 3 | 7 | Dirección vertical (1=arriba, 0=abajo) |
| 4 | 6-0 | Desplazamiento horizontal (magnitud) |
| 4 | 7 | Dirección horizontal (1=izquierda, 0=derecha) |

Tras el paquete de 32 bits, el ratón original emite únicamente unos (`1`); el clon Hyperkin emite un `1` seguido de ceros.

### Movimiento en signo-magnitud

El desplazamiento no usa complemento a dos, sino signo-magnitud: por ejemplo, `$05` son cinco "mickeys" en una dirección y `$85` son cinco mickeys en la dirección contraria. Conversión a complemento a dos:

```
lda tercer_byte
bpl :+
eor #$7F
inc
:
sta velocidad_y
```

Cuando la magnitud de movimiento reportada es 0, el signo repite el último valor de signo usado para esa coordenada (no hay un signo "neutro" fiable en reposo).

### Configuración de sensibilidad (ratón original)

Se ajusta manteniendo el latch activo (bit 0 de $4016) y leyendo el puerto serie correspondiente ($4016/$4017):

```
lda #1
sta $4016
lda $4016
stz $4016
```

Tabla de sensibilidad (valor interno vs. configuración seleccionada):

| Valor | Configuración 0 | Configuración 1 | Configuración 2 |
|-------|-----------------|-----------------|-----------------|
| 0 | 0 | 0 | 0 |
| 1 | 1 | 1 | 1 |
| 2 | 2 | 2 | 4 |
| 3 | 3 | 3 | 9 |
| 4 | 4 | 8 | 12 |
| 5 | 5 | 10 | 20 |
| 6 | 6 | 12 | 24 |
| 7 | 7 | 21 | 28 |
| 8+ | lineal | 21 | 28 |

La configuración 0 corresponde a 50 cuentas por pulgada de forma lineal, con valores posibles de 0 a 63 (por encima de 25 resulta muy difícil de usar en la práctica). Este ajuste no puede realizarse mientras hay una lectura automática activa.

### Particularidades del clon Hyperkin

- Requiere un mínimo de 170 ciclos de reloj maestro entre lecturas de bit.
- Requiere un mínimo de 336 ciclos de reloj maestro entre el byte 2 y el byte 3 (bits 16-17).
- Tras los 32 bits del paquete, emite un `1` seguido de ceros.
- Ignora cualquier intento de configuración por software: la sensibilidad reportada siempre es 0.
- La sensibilidad se cambia con un botón físico en el propio ratón (rango bajo 0-31, rango alto 0-63), y escala linealmente con la velocidad del movimiento, no con la distancia recorrida.
- Velocidad máxima de seguimiento de aproximadamente 8 pulgadas por segundo; necesita una superficie con textura para funcionar correctamente (es un ratón óptico).

### Ejemplo de lectura manual (con temporizado para el clon Hyperkin)

```
.a8
.i8
:
lda a:$4212
and #1
bne :-
; Esperar 336 ciclos (Hyperkin)
ldy #16
:
lda a:$4017
lsr
rol z:mouseread_x
rol z:mouseread_y
nop ; 170+ ciclos total en loop (188 en FastROM)
nop
nop
nop
dey
bne :-
```

Direcciones involucradas: $4016 (puerto 1), $4017 (puerto 2).

## Notas / gotchas

- El ratón original tolera cualquier velocidad de lectura razonable; el clon Hyperkin exige los tiempos mínimos indicados arriba o los datos leídos son inválidos.
- Al no usar complemento a dos, hay que aplicar la conversión de signo-magnitud manualmente; una extensión de signo ingenua produciría resultados incorrectos.
- Con magnitud de movimiento en 0, el bit de signo puede no ser significativo: conviene ignorarlo o mantener el último signo conocido.
- Algunas revisiones del microcontrolador del ratón original devuelven valores inútiles hasta que se cambia explícitamente la sensibilidad al menos una vez.
- No debe intentarse reconfigurar la sensibilidad mientras hay una lectura automática de mandos en curso.

## Referencias cruzadas

- (pendiente de enlazar)

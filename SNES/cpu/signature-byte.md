---
titulo: Signature byte en BRK, COP y WDM
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, brk, cop, wdm, interrupciones]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Signature_byte]
---

# Signature byte en BRK, COP y WDM

## Resumen
El "signature byte" es el byte que sigue al opcode de las instrucciones BRK, COP y WDM del 65816. El hardware avanza el contador de programa 2 bytes en estos casos aunque, en el caso de BRK/COP, ese segundo byte no es leído ni usado directamente por la circuitería de la CPU; puede aprovecharse por software como un código identificador.

## Detalle técnico

Un signature byte es el operando que acompaña a las instrucciones BRK, COP y WDM. El contador de programa avanza 2 bytes tras ejecutarlas, aunque el hardware de la CPU no utiliza directamente ese segundo byte para BRK/COP.

### Instrucciones afectadas

**BRK y COP** generan interrupciones por software, cada una con su propio vector:
- Vector de COP: $FFE4
- Vector de BRK: $FFE6

**Estado de la pila tras la interrupción** (offsets relativos al stack pointer tras la interrupción):
- $00,S: vacío
- $01,S: registro P (estado del procesador)
- $02,S: byte bajo de la dirección de retorno (PC + 2)
- $03,S: byte alto de la dirección de retorno
- $04,S: banco K (program bank) en modo nativo

**WDM** está reservada sin uso funcional; se comporta como un NOP de 2 bytes.

### Usos prácticos del signature byte
- Sin usar el signature: la rutina manejadora puede decrementar en 1 la dirección de retorno guardada en la pila antes de ejecutar `RTI`, simulando que BRK fue una instrucción de 1 byte.
- Usando el signature: la rutina manejadora puede inspeccionar ese segundo byte como código de error o como selector de una llamada al sistema (una especie de "syscall" por software).

### Comportamiento según ensamblador
No existe un estándar universal sobre cómo tratan los ensambladores el signature byte de BRK/COP/WDM:
- **ca65**: el signature es opcional para BRK y COP (admite 1 o 2 bytes); para WDM siempre exige el signature.
- **wla-dx**: siempre emite 2 bytes; el signature por defecto es 0 si no se especifica.
- **asar**: siempre emite 2 bytes; el signature por defecto es 0 si no se especifica.

## Notas / gotchas
- Como no hay convención universal entre ensambladores, el mismo código fuente (`BRK` sin operando, por ejemplo) puede generar 1 o 2 bytes según el ensamblador usado; esto importa para calcular offsets y para el truco de decrementar la dirección de retorno.
- Si el manejador de BRK/COP asume signature de 2 bytes pero el ensamblador solo generó 1 (o viceversa), el ajuste de la dirección de retorno en la pila antes de `RTI` queda desalineado.
- WDM siempre requiere el signature byte en ca65, a diferencia de BRK/COP donde es opcional; es fácil olvidar este caso especial al portar macros entre ensambladores.

## Referencias cruzadas
- (pendiente de enlazar) — relacionado con la entrada de vectores de CPU (vectores de COP y BRK).

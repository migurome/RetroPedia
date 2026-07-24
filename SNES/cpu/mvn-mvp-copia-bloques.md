---
titulo: MVN y MVP — copia de bloques de memoria en 65816
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, mvn, mvp, copia-memoria, ensamblador]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/MVN_and_MVP_block_copy]
---

# MVN y MVP — copia de bloques de memoria en 65816

## Resumen
MVN y MVP son instrucciones del 65816 diseñadas para copiar bloques de memoria byte a byte entre bancos, usando A como contador, X como puntero origen e Y como puntero destino. MVN copia en orden ascendente y MVP en orden descendente, lo que determina cuál usar según si el destino se solapa con el origen.

## Detalle técnico

### Sintaxis y opcodes

| Instrucción | Opcode | Operando 1 | Operando 2 |
|---|---|---|---|
| MVN src, dest | $54 | dest (banco) | src (banco) |
| MVP src, dest | $44 | dest (banco) | src (banco) |

Nota importante: en el código máquina, el operando de banco destino se codifica primero, aunque la sintaxis tradicional de ensamblador escribe el origen a la izquierda (`MVN src, dest`).

### Registros involucrados
- **A**: cantidad de bytes a copiar, menos 1 (16 bits).
- **X**: dirección origen (16 bits, dentro del banco).
- **Y**: dirección destino (16 bits, dentro del banco).
- Los dos bytes de operando inmediato especifican los bancos de origen y destino.

### Diferencia entre MVN y MVP
- **MVN (Move Memory Negative)**: copia en orden ascendente de direcciones. Se usa cuando destino < origen, para evitar que la copia se sobrescriba a sí misma.
- **MVP (Move Memory Positive)**: copia en orden descendente de direcciones. Se usa cuando destino > origen, por la misma razón.

### Estado tras la ejecución
- Los datos quedan copiados correctamente.
- El registro A termina en $FFFF (por el conteo hacia atrás).
- X/Y quedan incrementados (MVN) o decrementados (MVP) según la cantidad de bytes copiados.
- El registro DB (data bank) queda igual al banco destino especificado en la instrucción.
- Las banderas no se modifican.

### Duración
Cada byte copiado consume 7 ciclos de CPU del 65C816, aunque el timing real en la SNES es algo más complejo que esa cifra base.

### Limitaciones
Las direcciones no cruzan límites de banco: si X o Y se desbordan durante la copia, la dirección se ajusta dentro del mismo banco en vez de pasar al siguiente.

### Ejemplos de código

**MVN (copia hacia adelante):**
```
REP #$30
LDX #.loword(source)
LDY #.loword(dest)
LDA #(size-1)
MVN #^source, #^dest
```

**MVP (copia hacia atrás):**
```
REP #$30
LDX #.loword(source+size-1)
LDY #.loword(dest+size-1)
LDA #(size-1)
MVP #^source, #^dest
```

### Sintaxis en ca65 (v2.18+)
Son válidos dos formatos:
- `MVP #^source, #^dest` (especifica el banco directamente con el operador `^`)
- `MVP source, dest` (ca65 extrae automáticamente el byte de banco)

Advertencia: si se omite el `#` sin usar `^`, ca65 descarta silenciosamente los 2 bytes inferiores de la dirección, lo que puede seleccionar accidentalmente el banco $00 sin ningún aviso.

## Notas / gotchas
- Elegir MVN o MVP incorrectamente cuando origen y destino se solapan corrompe los datos copiados; la regla es: destino < origen → MVN, destino > origen → MVP.
- El registro DB (data bank) queda modificado tras la instrucción (igual al banco destino), lo cual puede afectar a código posterior que asuma un DB distinto.
- El error de sintaxis en ca65 al omitir `#`/`^` es silencioso: no lanza error de ensamblado, simplemente ensambla mal el banco (típicamente $00), y el bug solo se manifiesta en tiempo de ejecución.
- MVN/MVP no cruzan límites de banco automáticamente: para copiar bloques que abarcan más de 64 KB hace falta gestionar el cambio de banco manualmente entre llamadas.
- A debe cargarse con "tamaño - 1", no con el tamaño exacto del bloque; es un desfase fácil de olvidar.

## Referencias cruzadas
- (pendiente de enlazar)

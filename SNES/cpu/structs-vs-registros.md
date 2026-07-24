---
titulo: Structs vs. registros — dónde guardar el "this" en 65816
plataforma: [SNES]
categoria: [cpu]
tags: [cpu, optimizacion, structs, registros, arquitectura]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/Struct_register_tradeoffs]
---

# Structs vs. registros — dónde guardar el "this" en 65816

## Resumen
Al programar un motor de juego en 65816, es habitual necesitar un puntero ("this") a la estructura del objeto actual (por ejemplo, un actor) mientras se procesa su lógica. La elección de qué registro usar como ese puntero —X, Y, o incluso el registro de direct page— tiene consecuencias arquitectónicas profundas, porque cada registro tiene un conjunto distinto de instrucciones y modos de direccionamiento compatibles.

## Detalle técnico

### Asimetrías entre X e Y en el 65816
Varias instrucciones y modos de direccionamiento no están disponibles simétricamente para ambos registros índice:
- `ASL`, `ROL`, `LSR`, `ROR`, `INC`, `DEC`, `BIT`, `STZ` solo admiten indexación por X, no por Y.
- `direct page,y` solo existe para `LDX`/`STX`.
- `absolute long,y` no existe; solo `absolute long,x`.
- Los modos indirectos (`(direct page),y`, `[direct page],y`) son casi exclusivos de Y; X no tiene equivalentes directos para esos modos.
- Para tablas de salto, `JMP (absolute,x)` existe, pero no hay equivalente con Y.

### Opción 1: X como "this"
**Ventajas:**
- Permite operar directamente sobre campos de la estructura con instrucciones tipo `ASL`, `DEC`, etc. (solo disponibles indexadas por X).
- `direct page,x` es 1 ciclo más rápido que `absolute,x` cuando los registros son de 16 bits.
- Deja Y libre para direccionamiento indirecto (`[direct page],y`).
- El acceso a la estructura vía direct page es independiente del registro data bank (siempre banco 0).

**Desventajas:**
- Usar `absolute long,x` para acceder a datos en otros bancos obliga a guardar y restaurar X; la alternativa de usar `absolute,y` fuerza a sincronizar el data bank.
- Incompatible con tablas de salto indexadas por X (`JMP`/`JSR (absolute,x)`) sin recurrir a trucos como el "RTS trick".
- Para combinar structs y tablas de salto indexadas por X sin conflicto, el código tiende a quedar limitado a los bancos $80-$BF.

### Opción 2: Y como "this"
**Ventajas:**
- X queda libre para `absolute long,x`, permitiendo acceso arbitrario a cualquier banco sin coste extra de ciclos en modo 16 bits.
- El data bank puede fijarse en $7E (los 64 KiB de RAM principal) sin necesidad de gestionarlo dinámicamente.
- Es trivial pasar el "this" a código mediante `JMP`/`JSR (absolute,x)`, ya que X queda libre para indexar la tabla de salto.
- X también queda disponible para tablas alternativas de bit-fields, etc.

**Desventajas:**
- Los campos de la estructura solo pueden accederse vía `absolute,y`, ya que Y no soporta direct page indexado de forma completa.
- Es 1 ciclo más lento que `direct page,x` cuando Y está en modo 16 bits.
- `(direct page),y` y `[direct page],y` requieren una gestión de guardado/restauración similar a la de `absolute long,x` con X.

### Opción 3: Direct page como "this"
Consiste en mover el propio registro de direct page (DP) al inicio de la estructura del objeto actual, de modo que el direccionamiento "directo" pase a ser relativo a esa estructura.

**Ventajas:**
- X e Y quedan completamente libres para usarse como contadores o punteros simultáneos.
- El acceso a campos cuesta 4 ciclos (igual que `absolute,x` en modo 8 bits, y mejor que en modo 16 bits).
- Están disponibles todas las operaciones sobre memoria: `ASL`, `TRB`, `TSB`, `PEI`, `CPX`, `CPY`.
- Es trivial pasar el "this" a tablas de salto.

**Desventajas:**
- Muy pocas herramientas de ensamblado soportan bien un direct page distinto de cero; de los ensambladores mencionados, solo 64tass lo maneja correctamente.
- Las variables globales quedan forzadas a direccionamiento absoluto (4 ciclos), ya que la direct page está ocupada por la estructura.
- Límite de 8 KiB en el banco 0 para ubicar las estructuras (y menos aún si hay que alojar varias).
- Un direct page no alineado añade 1 ciclo extra a todas las operaciones de direct page.
- Con direct page alineado a 256 bytes se logra la velocidad teórica máxima (3 ciclos), pero consume 256 bytes por estructura o exige un layout de memoria más elaborado.

### Resumen comparativo de ciclos

| Contexto | X como "this" | Y como "this" | DP como "this" |
|----------|------------|-----------|-------------|
| direct page (16 bits) | 3 ciclos | N/A | 4 ciclos |
| absolute,x/y (16 bits) | 4 ciclos | 4 ciclos | N/A |
| absolute long,x | Requiere guardar X | 4 ciclos | N/A |

La página fuente no incluye ejemplos de código ensamblador explícitos, es un análisis puramente conceptual de tradeoffs.

## Notas / gotchas
- La elección de registro para "this" es una decisión arquitectónica temprana en un motor de juego: cambiarla más adelante implica reescribir buena parte del código de acceso a structs.
- Usar X como "this" y necesitar además tablas de salto indexadas por X simultáneamente es una combinación conflictiva; requiere trucos (RTS trick) o restringir el código a ciertos bancos.
- La opción de direct page como "this" es la más rápida en teoría, pero depende de soporte específico del ensamblador (64tass) — no es portable a cualquier toolchain SGDK/ca65/asar sin verificar antes.
- Con Y como "this", cualquier operación bit a bit sobre campos de la estructura (ASL, INC, etc., que solo existen indexadas por X) obliga a mover el valor a otro sitio primero.

## Referencias cruzadas
- (pendiente de enlazar) — relacionado con la entrada de usar X como puntero.

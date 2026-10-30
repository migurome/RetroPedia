---
titulo: Compilar la lógica CNS de MUGEN — semántica real y trampas
plataforma: [General]
categoria: [arquitectura, cpu]
tags: [mugen, cns, cmd, triggers, bytecode, vm, compilador, statedef]
estado: borrador
fecha: 2026-10-30
fuentes: [documentación de Elecbyte (cns, exp, sctrls), proyecto hyperdbz-dreamcast-port (tools/cns/, compilado con Goku y Krillin de Hyper DBZ 6.0c)]
---

# Compilar la lógica CNS de MUGEN

## Resumen

La lógica de un personaje de MUGEN son **estados** (`[Statedef N]`) con **controladores** (`[State ...]`) que se ejecutan si sus **triggers** son ciertos. Interpretar ese texto en cada tick es inviable en una consola: Goku de Hyper DBZ tiene 529 estados, 6.396 controladores y 35.000 expresiones. Hay que compilarlo a bytecode o a código nativo. Esta entrada recoge las reglas que no están claras en la documentación y que se descubrieron compilando código real. Está en `borrador` porque falta contrastar la VM con MUGEN.

## Detalle técnico

### Qué se ejecuta en cada tick

Por cada jugador: estado especial **-3** (salvo en un estado ajeno), **-2** (siempre), **-1** (comandos) y el estado actual. Los helpers solo ejecutan -1 si tienen `keyctrl = 1`. Dentro de un controlador:
- todas las líneas `triggerall` deben ser ciertas;
- después basta con que lo sea un grupo `trigger1`, `trigger2`...: todas las líneas de ese grupo.

Los grupos deben ser consecutivos desde 1: sin `trigger1`, el controlador no se ejecuta nunca.

`persistent = 0` hace que el controlador se ejecute una sola vez por entrada al estado; `persistent = N` lo ejecuta cada N veces que sus triggers son ciertos.

### Reglas descubiertas en código real (Hyper DBZ)

1. **`[State ...]` no necesita número.** `[State varset]` o `[State instant charge during intros]` son controladores válidos. Si el lector exige un número, pierde en silencio cientos de controladores (en Goku, 205 de los 322 del estado -2).
2. **`&&` y `||` no cortocircuitan.** El idioma `trigger1 = 1 || var(34) := 16 || var(33) := 45` (muy usado) solo funciona si se evalúan todos los operandos. Entre *líneas* de trigger sí se corta: la primera falsa detiene el grupo.
3. **`:=` se aplica a la variable inmediata** y su lado derecho llega hasta el siguiente `||`/`^^`. Admite `var`, `fvar`, `sysvar` y `sysfvar`.
4. **`command = "x"` liga más que la aritmética:** `(command = "holda" + command = "holdb" + command = "holdc") >= 2` cuenta botones pulsados. Lo mismo vale para `name = "..."` y similares.
5. **Formas especiales tras `=`:**
   - intervalos `[a, b]`, `(a, b]`...;
   - `AnimElem = n, >= m` → `AnimElemTime(n) >= m` (sin comparación, `= 0`);
   - `TimeMod = m, r` → `Time % m = r`;
   - símbolos: `statetype = A`, `hitdefattr = SC, NA, SA`, `teammode = simul`.
6. **Redirecciones:** `enemy, pos x`, `helper(1500), var(3)` o `root, stateno` afectan solo al trigger siguiente. Con una comparación (`enemynear, statetype = L`), la comparación va dentro de la redirección.
7. **Triggers con subclave:** `pos x`, `vel y`, `p2bodydist x`, `screenpos y`, `parentdist x`... `Const(...)`, `GetHitVar(...)` y `StageVar(...)` reciben un nombre con puntos (`Const(movement.air.gethit.groundlevel)`).
8. **Constantes por defecto:** si el personaje no define una constante de `[Movement]`, MUGEN usa la suya (por ejemplo, `air.gethit.groundlevel = 25`). `velocity.jump.y` se deriva de `jump.neu`.
9. **Precedencia de archivos:** gana la primera definición de cada estado en este orden: `cmd`, `cns`, `st`, `st1`... y al final `stcommon` (`common1.cns`).
10. **Parámetros que no son expresiones:** `attr = S, NA`, `postype = p1`, `flag = noshadow`, `sound = S1,0`, `anim = F2010`, `sparkno = S ifelse(...)` (la `S` delante indica sprite propio). Se reconocen porque solo tienen nombres y comas, y al menos un nombre no es un trigger.

### Diseño del bytecode usado

- Palabras de 16 bits (el SH-4 no admite lecturas desalineadas de 32 bits) y constantes de 32 bits en dos palabras.
- Máquina de pila con valores int/float etiquetados (MUGEN distingue enteros, por ejemplo en la división).
- Opcodes específicos: `TRIG id`, `TRIGA id` (con argumento), `CMD`, `STREQ` (compara hashes FNV-1a), `SYMEQ` (máscara de bits), `INTERVAL`, `REDIR tipo salto`... `REDIREND`, `ASSIGN` y `FUNC`.
- **Deduplicar expresiones** compensa mucho: en Goku, 35.035 expresiones se quedan en 5.727 únicas.
- Resultado: banco de lógica de 738 KB (Goku) y 535 KB (Krillin), con el 100 % de las expresiones compiladas.

## Notas / gotchas

- Cifras de un personaje de Hyper DBZ: -2 tiene unos 320 controladores que se evalúan **en cada tick**, y -1 unos 150. Los triggers más usados son `time`, `enemy`, `stateno`, `NumExplod`, `name`, `anim`, `command`, `AnimElemTime`, `var` y `AILevel`.
- Si una expresión no compila, mejor tratarla como falsa y registrarla que abortar: así se mide la cobertura.

## Referencias cruzadas

- [Formatos de MUGEN — SFF, AIR y SND](formatos-sff-air-snd.md)
- [Another World — VM y bytecode](../arquitectura/another-world-overview.md) — otro motor basado en bytecode

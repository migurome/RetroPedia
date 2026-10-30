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

### Lo que hace el motor y no está en el CNS

Una VM que solo ejecute los controladores se queda atascada. Hay comportamientos que MUGEN aplica por su
cuenta y que el código de los personajes da por hechos (descubiertos con una simulación que se bloqueaba):

- **Aterrizaje:** con `physics = A`, cuando el personaje cae (`vel y > 0`) y llega a `pos y >= 0`, el motor
  lo pasa al **estado 52**. El estado 50 (salto) no tiene ningún controlador de aterrizaje.
- **Suelo:** solo `physics` S y C se pegan al suelo con rozamiento. `physics = N` no se toca: los estados de
  caída tras un golpe (5050, 5953...) aplican su propia gravedad con `GetHitVar(yaccel)` y comparan `pos y`
  con `Const(movement.air.gethit.groundlevel)` (25 por defecto). Si se pega N al suelo, nunca llegan.
- **Giro automático:** solo con control en los estados 0, 11 y 20. Hyper DBZ gira en el resto con su propia
  animación y `Turn`. Si el motor también gira, se pelean y la animación se reinicia sin fin.
- **`ChangeAnim` con `elem`:** `elem = AnimElemNo(0)` cambia de animación conservando el fotograma.
  Sin él, los estados que esperan `AnimTime = 0` nunca acaban.
- **`GetHitVar`:** el golpe debe rellenar al menos `hittime`, `slidetime`, `hitshaketime`, `animtype`, `xvel`,
  `yvel`, `yaccel` y `fall.*`, o los estados de recibir daño no avanzan.

### Coste medido en Dreamcast (Flycast)

Dos personajes de Hyper DBZ, por tick: unos **1.320 controladores**, **2.100–2.500 expresiones** y 12.000–18.000
opcodes. Lo dominan los estados especiales (-2 y -1), que se evalúan siempre. El par más pesado del juego,
Shin Golden Freeza contra Jiren (625 y 509 controladores en -2), cuesta un 40 % más.

Tiempo por tick en Flycast:

| Prueba | VM sin optimizar | VM optimizada |
|---|---|---|
| Goku-Krillin sin IA | 7,09 ms | 4,01 ms |
| Goku-Krillin IA 8 | 5,22 ms | 2,66 ms |
| Freeza-Jiren sin IA | ~9 ms (estimado) | 5,47 ms (peor tick 8,0) |

Con la VM optimizada (ver el apartado anterior) la lógica cabe holgada en un frame de 16,7 ms.

Truco útil: compilar la misma VM para PC (MinGW, con un `kos.h` mínimo que traduce `fs_open` y el temporizador)
y fijar la semilla aleatoria. Los resultados son idénticos a los de la consola y cada prueba tarda segundos en
lugar de minutos.

Ese arnés de PC sirve también como red de seguridad para optimizar:
- **Huella del mundo:** un hash FNV-1a de estados, tiempos, animaciones, posiciones y variables de todos los
  actores, encadenado tick a tick y comparado cada 300 ticks con una referencia guardada. Una optimización que
  cambie un solo bit de la simulación se detecta, y se sabe en qué tramo empezó a divergir.
- **Personaje sintético:** un `.cns` cuyos controladores dejan en variables el resultado de cada regla de este
  artículo (`:=`, cortocircuito, intervalos, `persistent`, grupos de triggers, guardas...), comparado con los
  valores esperados.

### Optimizar la VM: qué funcionó

Medido con una batería de QA que compara una huella del estado del mundo tick a tick: todas las optimizaciones
dejan la simulación idéntica bit a bit. Por orden de impacto en Hyper DBZ:

1. **Guardas constantes por actor.** Muchos controladores de -2 y -1 llevan condiciones que no cambian en todo
   el combate para un actor dado: `AILevel`, `IsHelper`, `IsHelper(id)`, `TeamSide`, `Name`, `Const(...)`. El
   compilador extrae las partes de un `&&` que solo dependen de ellas (las *guardas*). La VM las evalúa una vez
   al crear el actor y monta la lista de controladores vivos de -3, -2 y -1, compartida entre actores
   equivalentes. Con IA se descarta entre el 20 y el 55 % del -2 y la mitad del -1.
   - **Trampa:** solo es seguro si esa línea de triggers y las anteriores no tienen efectos (`:=` o `Random`).
     Una guarda falsa detiene la evaluación en su línea, así que las de después nunca se habrían evaluado; las
     de antes, sí.
2. **Cortocircuito seguro.** MUGEN no cortocircuita `&&` ni `||`, pero si el lado derecho no tiene efectos, el
   resultado es el mismo saltándoselo: `a && b` pasa a ser `a ? bool(b) : 0`. Expresiones como
   `NumExplod(x) && Anim = y && AnimElemTime(...) >= z` dejan de evaluarse enteras cuando `NumExplod` vale 0.
3. **Superinstrucciones.** El 25 % de los opcodes eran constantes que se empujaban a la pila para un trigger o
   una comparación. `TRIGK` lleva el argumento dentro (de **32 bits**: Hyper DBZ usa ids de explod y de helper
   como 90000005) y `EQK`, `LTK`... comparan con una constante de 16 bits.
4. **Triggers caros.**
   - `NumExplod(id)`: Hyper DBZ usa los explods como banderas y lo consulta miles de veces por tick. Un filtro
     de 32 bits por actor (bit `id & 31`) responde 0 sin recorrer la lista casi siempre.
   - `NumHelper(id)` y `helper(id)`: una lista compacta de helpers vivos (slot, root, id) en vez de recorrer
     los 64 actores, cada uno en una línea de caché distinta.
5. **Detalles de la VM.** Despacho por tabla de saltos (*computed goto* de GCC), triggers que escriben
   directamente en la pila en vez de devolver una estructura de 8 bytes, y contadores de coste solo en la
   versión de PC.

Los helpers **no** ejecutan los estados -3 ni -2, y el -1 solo con `keyctrl = 1`
([documentación de Elecbyte](https://www.elecbyte.com/mugendocs/cns.html)). Un helper que ejecutara el -2
completo de su personaje duplicaría el coste, porque son los estados más grandes.

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

---
titulo: Optimizar C compilado con gcc para el 68000 (lecciones de un port con SGDK)
plataforma: [MegaDrive]
categoria: [cpu]
tags: [gcc, sgdk, optimizacion, perfil, 68000, md-engine]
estado: revisado
fecha: 2026-10-23
fuentes: ["Port de Super Smash Land (B044): medido con el perfilador por funciones del emulador de MD Engine"]
---

# Optimizar C compilado con gcc para el 68000

## Resumen

Al pasar un juego de 2 a 4 luchadores (SGDK y MD Engine), el núcleo en C pasó a gastar más de un fotograma por paso. Lo que más rindió no fue escribir ensamblador, sino **evitar trabajo** y **no dejar que gcc genere código caro**: divisiones de 32 bits, copias de structs e índices recalculados. Antes de tocar nada hay que **medir por funciones**, porque las intuiciones fallaron varias veces.

## Detalle técnico

- **Mide antes de optimizar.** Escribir un número en una variable `volatile` al entrar y al salir de cada función permite que el emulador de MD Engine cuente ciclos totales, propios y llamadas (`_tbce_set_prof_script_addrs(start, end, count)`); ver [depuración y generación](../herramientas/md-engine/depuracion-y-generacion.md). En este port, tres hipótesis «obvias» resultaron falsas: el coste de `random()`, el de las cajas de los proyectiles y el de las llamadas a SGDK de las piezas.
- **`__divsi3`, `__modsi3` y `__mulsi3` son rutinas por software lentísimas.** En SGDK, la división viene del código de Sozobon: un bucle bit a bit de cientos a más de mil ciclos. `s32 / constante` y `s32 * s32` las llaman.
  - División entre una constante con |v| < 65536: recíproco `k = ceil(65536 / d)` y un `mulu.w`, más una corrección de ±1.
  - Para cocientes que caben en 16 bits, `divu` del 68000 (unos 140 ciclos).
  - `rnd(n) == 0` (azar de 0 a n-1, con `r * n >> 16`) es lo mismo que `r < ceil(65536 / n)`: con `n` constante, una comparación.
  - Busca `jsr __divsi3` / `__mulsi3` en el desensamblado (`objdump -d rom.out`) y mira cuáles caen en el camino de cada fotograma.
- **Copiar un struct grande cuesta.** `AttackDef a = tabla[i];` (~50 bytes) en una comprobación de golpe que se hace decenas de veces por paso. Mejor un puntero, y copiar solo cuando hace falta modificarlo.
  - Además, comparar la dirección de esa copia (`a == &tabla[6]`) nunca es cierto: era un fallo latente.
- **Índices a arrays de structs.** `&array[index]` con un tamaño que no es potencia de 2 es una multiplicación o una cadena de desplazamientos. Con pocos registros, gcc la **recalcula en cada acceso** en vez de guardarla.
  - Si el dato pertenece a algo que ya tienes en un registro (el jugador, `p`), mételo en ese struct y usa `&p->campo`.
  - Ojo: `&ssb_players[index].ai` sigue recalculando aunque valga lo mismo que `&p->ai`; gcc no lo sabe.
- **Evita el trabajo repetido entre pasos** (mayor ganancia):
  - Listas de sólidos cercanos por personaje, hechas con una caja mayor y reutilizadas mientras la del paso quepa dentro.
  - En movimientos de píxel en píxel, si la caja barrida por todo el recorrido está libre, se avanza de una vez con el mismo resultado.
  - Descarte por distancia (alcance máximo calculado al arrancar) antes de las pruebas de golpe por parejas.
  - Piezas que no han cambiado en pantalla (coordenadas de juego / 4): no llamar a la función de dibujo.
- **Generador de azar en línea**: un xorshift de 16 bits (`x ^= x << 7; x ^= x >> 9; x ^= x << 8`) sembrado con `random()` no necesita llamada.
- Con LTO, gcc ya pone en línea muchas funciones pequeñas: reescribir a mano lo que ya estaba en línea no cambia nada (comprobado con `proj_solid`).

- **gcc no siempre usa `muls.w` aunque los dos factores sean `s16`.** Tras cargar el valor en un registro de 32 bits pierde el dato de que cabe en 16 y llama a `__mulsi3`. Pasaba con `distancia * seno` y con `velocidad * seno`. Con ensamblador en línea se fuerza:

  ```c
  static inline __attribute__((always_inline)) s32 muls16(s16 a, s16 b)
  {
      s32 r = a;
      __asm__("muls.w %1,%0" : "+d"(r) : "dmi"(b) : "cc");
      return r;
  }
  ```

  Igual con `divu.w` (cociente de 16 bits; quien llama comprueba que cabe, porque si no el 68000 activa V y no divide). SGDK no trae versiones en línea: su `divu()` es una función.
- **Tablas precalculadas por el generador** cuando el dominio es pequeño. Por ejemplo, las 720 posiciones de una pieza que gira en medios grados, calculadas con la misma aritmética entera que el C (truncar hacia 0, desplazamiento aritmético) para que el resultado sea idéntico.
- **Descartes por alcance precisos**: un alcance máximo global (el de todos los ataques) apenas descartaba nada cuando todos pelean juntos; por personaje y ataque, la mitad de las comprobaciones desaparecen.
- **Mide con una métrica exacta y reproducible.** En el banco, la partida con CPU es determinista, así que «pasos completados en 900 fotogramas» compara dos versiones sin ruido. Una traza de posiciones paso a paso confirma además que la optimización no cambia el juego. Varias ideas razonables salieron **más lentas**:
  - memoria de la última consulta, porque los aciertos eran pocos;
  - separar el camino rápido de una función con `always_inline`, porque subía la presión de registros en la función que llama;
  - `#pragma GCC optimize` (incluso `"O3"`: el propio pragma costaba), y `-O2`, `-Os`, `-fgcse` y `-fweb`;
  - rellenar un struct a potencia de 2, porque los bucles ya avanzaban con punteros.

### Una rutina entera en ensamblador, llamada por registros (Super Smash Land MD, B046)

La consulta de colisión (¿toca esta caja algún sólido?) se hacía 5-7 veces por luchador y paso. En C, cada llamada guardaba 11 registros, recibía los argumentos por la pila y, en cada rampa, llamaba a otra función que repetía lo mismo: ~760 ciclos por consulta. Reescrita a mano bajó a ~200-400 y, con 4 CPU, los fotogramas perdidos del peor escenario bajaron del 10 % al 4 %. Cómo se hizo:

- **La rutina, en un `__asm__` de nivel superior** del `.c` (sintaxis de GNU as con `%` en los registros: `movea.l (%a1)+,%a0`; los saltos sin tamaño se ajustan solos). Así no hace falta un `.s` aparte, que la herramienta que compila el proyecto quizá no admita.
- **Argumentos en los registros que gcc no conserva** (d0, d1, a0, a1). La rutina solo guarda los que usa de más (`movem.l %d2-%d4/%a2,-(%sp)`). El envoltorio en C, con `always_inline`:

  ```c
  register s32 r0 __asm__("d0") = x1; /* ... d1, a0, a1 */
  __asm__ volatile("jsr ssb_solids_hit" : "+d"(r0), "+d"(r1), "+a"(r2), "+a"(r3) : : "cc", "memory");
  return r0;
  ```

  El 68000 compara con signo una palabra contra un registro de direcciones (`cmpa.w %d1,%a2`). Así a0 y a1 sirven para coordenadas, y `movea.w` + `adda.w` dan `y + h` en 32 bits, como el `int` del C.
- **Los datos por nombre, en una estructura global no `static`** con `__attribute__((used, externally_visible))`. Con LTO, gcc renombra los `static` (`.lto_priv.0`) y el ensamblador no los encontraría. Los desplazamientos de los campos se escriben a mano en el ensamblador y se fijan con `_Static_assert(__builtin_offsetof(...) == n)`.
- **El ensamblador puede aprovechar lo que el C no sabe.** Si solo un borde de la rampa va en tabla, el otro vale en todas las filas, y como el de la tabla es monótono basta mirar las dos filas extremas: sin búsqueda binaria. Que el generador de datos compruebe con `assert` lo que la rutina da por hecho.
- **Comprobar que es exacta:** traza paso a paso de partidas deterministas en todos los escenarios y, aparte, comparar ensamblador y C en decenas de miles de cajas alrededor de cada borde inclinado. El C se conserva bajo `#ifndef __m68k__` como referencia.
- **Lo que no salió a cuenta en ensamblador:** el xorshift de 16 bits (7, 9, 8) cuesta ~130 ciclos con desplazamientos, con tablas de 256 o con bytes; gcc ya lo hace igual. Y la lógica de estados (decenas de `if` sobre campos) no tiene un núcleo que reescribir.

## Notas / gotchas

- **Fija la semilla del azar antes de medir.** Si la IA se siembra con `random()` al empezar la partida, la semilla depende de cuántos fotogramas se pasaron en los menús. En Super Smash Land MD, cambiar el cursor inicial de un menú cambió la partida medida, y las pérdidas con 4 CPU pasaron del 1 % al 14 % sin tocar el código del combate. Las pruebas de rendimiento escriben ahora una semilla fija en la RAM al empezar el combate.
  - **Que la escritura no dependa del tiempo de carga.** Escribirla en la variable del azar unos fotogramas después de elegir escenario dejó de valer cuando la sala tardó más en cargar (fondos comprimidos): el código que la inicializa corría después y la pisaba. Con el mismo coste del núcleo, las pérdidas de Clock Town pasaron del 9,5 % al 13 %. Es mejor una variable aparte (`ssb_test_seed`) que el juego lee al empezar el combate si no es 0, escrita antes de elegir.

- Lo que **no** aplica al 68000 de las listas de «técnicas de optimización»: no hay caché, ni predictor de saltos, ni SIMD, ni renombrado de registros, y el *scheduling* apenas importa. Las que sí aplican son *strength reduction*, *code motion*, CSE, *inlining* y, sobre todo, cambios de algoritmo.
- Una función pequeña puede costar mucho solo por la llamada. Guardar 9-11 registros con `movem` y apilar 8 argumentos son 200-300 ciclos.
- Cuando el coste queda repartido (decenas de funciones de 1-3 % cada una), el C ya no da para mucho más: queda el ensamblador a mano en 2-3 rutinas o reducir trabajo de diseño.
- Cambiar el generador de azar cambia las jugadas de la IA: las pruebas automáticas que dependan de secuencias concretas pueden necesitar ajuste.

## Referencias cruzadas

- [Multiplicación y división en el 68000](multiplication-division.md)
- [Instrucciones y modos de direccionamiento](68000-instrucciones-modos-direccionamiento.md)
- [MD Engine: depuración y generación](../herramientas/md-engine/depuracion-y-generacion.md)

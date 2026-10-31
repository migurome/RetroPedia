---
titulo: Física y golpes del motor de MUGEN 1.0 — lo que el CNS no hace por sí solo
plataforma: [General]
categoria: [motores]
tags: [mugen, cns, hitdef, clsn, hitpause, hitshake, guard, custom-state, targetstate, playerpush, screenbound, camara, hitvelset, hitfallvel, hitfalldamage, ikemen]
estado: borrador
fecha: 2026-10-31
fuentes: [documentación de Elecbyte (cns.html, sctrls.html, trigger.html), código de Ikemen GO (src/char.go, src/bytecode.go, src/compiler_functions.go), common1.cns de MUGEN 1.0, CNS de Hyper DBZ Wicked Ambition, proyecto hyperdbz-dreamcast-port (dc/proto/src/logic.c)]
---

# Física y golpes del motor de MUGEN 1.0

## Resumen

Con ejecutar los estados CNS no basta para tener un combate. El motor de MUGEN hace por su cuenta, cada tick:
- la física de cada `physics`;
- el empuje entre luchadores;
- los límites de pantalla y la cámara;
- las colisiones Clsn1/Clsn2 y lo que pasa al golpear o bloquear.

Esta entrada recoge cómo se implementó para el port de Hyper DBZ y las trampas que aparecieron. Al pasar de
golpes simplificados a los reales, Goku visitó 59 estados en un minuto, frente a 25.

## Detalle técnico

### Orden de un tick

1. **Por cada jugador:** si mantiene atrás con control y hay un ataque enemigo a menos de su `guard.dist`, el
   motor lo pasa al estado 120.
2. **Por cada actor (jugadores y helpers):**
   1. los estados −3, −2, −1 y el actual;
   2. si no está en hitpause ni en hitshake, la física;
   3. avanzan `Time` y la animación.
3. **`TargetBind` / `BindToTarget`.**
4. **Colisiones:**
   1. se recogen primero todos los contactos y después se aplican, para que dos golpes del mismo tick se
      intercambien;
   2. después, cada `HitDef` que ha golpeado queda desactivado.
5. **Empuje entre jugadores y límites; luego la cámara.**
6. **Fin de tick:** bajan los contadores de `NotHitBy` y `HitOverride`, y se quitan los objetivos que ya
   no son `movetype = H`.

### Física (MUGEN 1.0)

- **`physics = S` o `C`:** `vel x *= stand.friction` o `crouch.friction`. **El umbral
  (`stand.friction.threshold`) no lo aplica el motor**: en MUGEN 1.0 lo hace `common1.cns` con un `VelSet`.
- **`physics = A`:** `vel y += yaccel`. Al llegar al suelo bajando, el motor lo lleva al estado 52.
- **`physics = N`:** nada. Los estados de caída tras un golpe llevan su propia gravedad.
- **`PlayerPush`, `PosFreeze`, `ScreenBound` y `Width`** duran **un tick**: los personajes los llaman con
  `trigger1 = 1`.

### Empuje, límites y cámara

- **Cuerpo:** `size.ground.front`/`back` (o `air.*` si `statetype = A`) y `size.height`. Dos jugadores se
  empujan si sus anchos se solapan y sus alturas también.
- **Reparto del empuje:** cada uno se aparta la mitad; si uno está contra el borde, el otro se aparta lo que
  falte.
- **Límites:**
  - los jugadores, a `screenleft`/`screenright` del borde de la cámara y dentro de
    `leftbound`/`rightbound` del escenario;
  - **los helpers no, salvo que llamen a `ScreenBound value = 1`.** Los de Hyper DBZ lo hacen cuando lo
    necesitan, y un proyectil limitado nunca saldría de la pantalla para destruirse.
- **Cámara:**
  - en horizontal se mueve cuando un jugador se acerca a menos de `tension` del borde;
  - en vertical, `cam_y = (y_más_alto + floortension) · verticalfollow` cuando el más alto sube más de
    `floortension`;
  - siempre dentro de `boundleft`, `boundright`, `boundhigh` y `boundlow`.
- **Triggers que miran la cámara real:** `BackEdgeDist`, `FrontEdgeBodyDist`, `ScreenPos`, `CameraPos`,
  `LeftEdge`... `P2BodyDist X` resta el ancho delantero propio y el del lado del rival que mira hacia él.

### HitDef

- **Cuándo se evalúa:** sus parámetros, al ejecutarse el controlador. Queda activo hasta que golpea o el actor
  cambia de estado (salvo `hitdefpersist`).
- **Colisión:** cualquier Clsn1 del frame actual del atacante con cualquier Clsn2 del defensor. Las cajas
  pueden venir con x1 > x2 y se reflejan con `facing`.
- **Filtros, por orden:**
  1. `NotHitBy`/`HitBy`: `attr` como máscara SCA × {N, S, H}{A, T, P};
  2. `hitflag`: H de pie, L agachado, A aire, F cayendo, D tumbado, `+` solo si ya recibe, `-` solo si no;
  3. si bloquea (`guardflag` con su postura, y en 120–155 o con control manteniendo atrás) → estados
     150/152/154;
  4. `HitOverride` → su estado, sin daño.
- **Al golpear:**
  - daño × `attack` del atacante / `defence` del defensor; si cae, se reduce con `fall.defence_up`;
  - `pausetime = p1, p2`: el atacante entra en **hitpause** y el defensor en **hitshake**;
  - `GetHitVar` se rellena (tipo, `animtype`, velocidades, `fall.*`...);
  - estado 5000/5010/5020/5080 según la postura, o `p2stateno`;
  - `p1stateno`, `p1facing`/`p2facing`, `p1sprpriority` (1)/`p2sprpriority` (0);
  - se añade a la lista de objetivos con el `id`.
- **Valores por defecto que importan:**
  - `hitflag = MAF`, `guardflag` vacío (no se puede bloquear), `air.hittime = 20`, `yaccel = 0.35`,
    `fall.yvelocity = -4.5`, `fall.recovertime = 4`, `p2getp1state = 1`;
  - `guard.*`, `air.*` y `down.*` heredan del valor de suelo;
  - `airguard.velocity = air.velocity × (1.5, 0.5)`.
- **Energía:** `getpower`/`givepower` salen de `Default.Attack.LifeToPowerMul` del `mugen.cfg`. **En Hyper DBZ
  es 0**: la energía la gestiona cada personaje.

### El golpe no mueve al defensor: lo hacen sus estados

El motor **no** aplica la velocidad de `ground.velocity`, `air.velocity` ni la de caída al golpear. Solo la
guarda en `GetHitVar`. Son los estados de recibir de `common1.cns` los que la aplican con cuatro
controladores (5001, 5011, 5030, 5071, 5081, 151, 153, 155, 5100...). Si faltan, el combate «funciona»,
pero nadie sale despedido: en el port de Hyper DBZ estuvieron sin implementar durante toda la Fase 1 y solo
lo destapó un test de QA con un muñeco sintético.

Semántica, comprobada en el código de Ikemen GO (`char.go` y `bytecode.go`):

- **`HitVelSet` (`x`, `y`, `z`):** con el flag distinto de 0, esa componente pasa a ser la del golpe; **si
  falta el parámetro, no se toca** (no hay valor por defecto). Ikemen guarda la x como `hitdef.vel × −facing
  del atacante` y al aplicarla la multiplica por el `facing` del defensor: el empujón aleja siempre del que
  golpea, aunque el defensor mire hacia otro lado.
- **`HitFallVel`:** solo si `movetype = H`. `vel y = fall.yvelocity`; `vel x = fall.xvelocity` **solo si el
  HitDef la traía** (si no, la x no cambia).
- **`HitFallSet` (`value`, `xvel`, `yvel`):** `value` −1 por defecto (no cambia el flag de caída); `xvel` e
  `yvel` solo si se dan.
- **`HitFallDamage`:** solo si `movetype = H`. Resta `fall.damage` con la defensa del personaje (y
  `fall.defence_up` si está cayendo), respeta `fall.kill` y lo deja a 0 para que no se aplique dos veces.

### Hitpause y hitshake no son lo mismo

- **Hitpause (atacante):** solo se ejecutan los controladores con `ignorehitpause = 1`; no hay física y no
  avanzan `Time` ni la animación.
- **Hitshake (defensor):** sus estados **sí** se ejecutan (el 5000 espera a `HitShakeOver`), pero no se
  mueve. `HitOver` llega cuando, acabado el hitshake, se agota el `hittime`.

### Estados ajenos (custom states)

- `TargetState` y `p2stateno` (con `p2getp1state = 1`) hacen que el defensor ejecute **los estados del
  atacante**. Es la base de todos los agarres.
- **En un estado ajeno:**
  - se siguen ejecutando el −2 y el −1 **propios**, pero no el −3;
  - `ChangeAnim` usa las animaciones propias y `ChangeAnim2`, la **animación** del dueño del estado (frames,
    tiempos, desplazamientos y cajas), pero **los sprites del propio personaje**, buscados por grupo y número
    en su SFF: así el lanzado se ve con su cara. Si se toman también los sprites del dueño, en los agarres
    salen dos copias del atacante;
  - `Const()` y `Name` siguen siendo los propios.
- **Se vuelve con `SelfState` o al recibir un golpe normal.**
- **Implementación:** cada actor guarda su banco (`own`) y el del dueño del estado actual (`lb`). La memoria de
  `persistent` del estado actual debe dimensionarse con el estado más grande **de los dos** personajes.

### Helpers: límite y binds

- **`HelperMax` (56 en el `mugen.cfg` de Hyper DBZ) es por jugador**, no global. En Ikemen GO, al superarlo,
  el helper no se crea («Reached limit of 56 helpers»).
- **`AnimElemTime` en una animación con `LoopStart`:** al dar la vuelta, el tiempo de la animación vuelve al
  del inicio del bucle, así que `AnimElemTime(n) = 0` se cumple **en cada vuelta**. Un `Helper` disparado así
  se crea una y otra vez: el aura de carga de Jiren crea uno cada 4 ticks y llega al límite. Es el
  comportamiento original, no un fallo del port.
- **`BindToParent` / `BindToRoot`:**
  - `time` es 1 por defecto, 0 suelta y −1 es para siempre (solo se descuenta si es mayor que 0);
  - `pos` va con el `facing` de aquel al que se ata;
  - `facing` 1 o −1 copia su orientación (o la contraria), y 0 no la toca;
  - en un jugador (sin padre) no hacen nada.

### SuperPause y Pause

Según el código de Ikemen GO:

- **Arranque:** el controlador deja la pausa en un búfer; **empieza en el tick siguiente** y dura `time` (30 por
  defecto). Si hay otra pendiente, la sustituye solo si es más larga o del mismo jugador.
- **Quién queda congelado:** mientras dura, un personaje congelado **no ejecuta nada**: ni sus estados −3, −2 y
  −1 ni el actual, ni física, ni `Time`.
- **Quién se libra:**
  - el que la lanza, durante su `movetime` (0 por defecto; se descuenta un tick por tick);
  - los helpers creados con `supermovetime` o `pausemovetime`.
- **Solo la `SuperPause`:**
  - `poweradd`;
  - `unhittable` (1 por defecto): el que la lanza no puede ser golpeado durante `time + 1` ticks;
  - `p2defmul` (`Super.TargetDefenceMul`, 1,5): multiplica la defensa de los rivales **que ya están
    recibiendo** y se quita cuando dejan de recibir.
- **Mientras dura cualquiera de las dos:** el reloj del asalto no avanza.
- **Visual y sonido:** `darken` oscurece la pantalla, `anim` lanza una chispa (`fightfx` por defecto, la 100) y
  `sound` suena.

### Proyectiles

Según el código de Ikemen GO (`char.go`: `Projectile.update`, `tick`, `tradeDetection`,
`hitDetectionProjectile`, y su compilador):

- **Son del jugador**, aunque los lance un helper. `NumProj`, `NumProjID` y `ProjHit` se miran en el
  jugador. Un helper siempre tiene `NumProj` 0 y `ProjHitTime` -1.
- **Orden del tick:** estados → movimiento de los proyectiles → choques entre proyectiles → golpes →
  final de su tick. Un proyectil se mueve ya en el tick en que nace.
- **Movimiento:**
  - `pos += velocity × facing`; luego `velocity = (velocity + accel) × velmul`;
  - si `velocity` x es negativa, se da la vuelta (`facing`) y se queda positiva.
- **Se quita** (con el proyectil sin pausar y sin hitpause) cuando:
  - `projremovetime` llega a 0;
  - o sale del borde de la pantalla más `projedgebound` (40);
  - o sale del escenario más `projstagebound` (40), según hacia dónde va;
  - o sale de `projheightbound` (-240, 1).

  Entonces pasa a la animación de quitar (golpe, quitar o cancelar), con `remvelocity` y sin `accel`, y
  desaparece **cuando la ha mostrado entera**. Ojo: su `AnimTime` (`curtime - totaltime`) llega a 0 un tick
  después que el trigger `AnimTime` de un personaje.
- **Animaciones:**
  - por defecto, `projremanim` = `projhitanim` (-1) y `projcancelanim` = `projremanim`;
  - -1 = ninguna;
  - una que no existe deja la que tiene;
  - un `projanim` que no existe usa **la animación actual del que lo lanza, desde el primer elemento**.
- **Golpe:**
  - su Clsn1 contra la Clsn2 del rival, mientras le queden `projhits` (1);
  - **no pone en hitpause al que lo lanza** ni cuenta para su `MoveHit`;
  - el proyectil se para su `pausetime` (y un tick más, `freezeflag`), gasta un golpe y espera
    `projmisstime`;
  - sin golpes y con `projremove`, se quita con `projhitanim`;
  - el retroceso aleja del proyectil;
  - un proyectil AP por tick y jugador a cada rival.
- **Choques entre proyectiles** de jugadores distintos, **Clsn2 contra Clsn2**: cada uno, si tiene más puntos
  de prioridad (`projpriority`) que el otro, gasta un punto; si no, un golpe. Con la misma prioridad se
  cancelan los dos. El que sobrevive se para su `pausetime`.
- **`ProjHit`, `ProjContact`, `ProjGuarded` (sintaxis antigua):**
  - `ProjHit1234 = 1` es `ProjHitTime(1234) = 0`: cierto **el tick siguiente al impacto**;
  - con segunda parte (`ProjHit1234 = 1, < 15`) es `t >= 0 && t < 15`;
  - `= 0` lo niega; id 0 = cualquiera;
  - el tiempo cuenta desde 0 y avanza al final de los estados del jugador, salvo en su hitpause.
- **`InGuardDist`** también es cierto por un proyectil enemigo con `attr` a menos de su `guard.dist` (90 por
  defecto, `size.proj.attack.dist`), por delante del proyectil.

**Cómo los usa un juego real (Hyper DBZ, 1.029 controladores):**
- **marcas invisibles** de 1-3 ticks (animación vacía) que otro código consulta con `NumProjID`: variables
  compartidas entre helpers y jugador, con caducidad;
- **el truco de la guardia** (325): un proyectil invisible 400 por detrás, de 2 ticks, con `guard.dist = 1999`,
  para que el rival esté siempre en distancia de guardia;
- solo unos 30 son bolas que vuelan; las ráfagas de energía son helpers.

Simular bien las marcas cambió el combate: Freeza contra Jiren creaba un tercio de helpers de más porque
`NumProjID` valía siempre 0.

### Prioridad, juggle, ReversalDef y contadores

También según Ikemen GO (`hittableByChar`, `hitDetectionPlayer`, `hitResultCheck`, `attrCheck`,
`checkCornerPush`):

- **Prioridad** (`priority = 4, Hit`; por defecto 4, Hit). Solo importa cuando el que recibe también ataca:
  - gana la más alta, salvo que la más alta no llegue (su Clsn1 no toca);
  - con la misma, Hit contra Hit se cambian los golpes;
  - si alguno es Dodge, o el que ataca es Miss, este solo golpea si el otro no puede.

  Hyper DBZ usa casi siempre 4, Hit, y «1, Miss» en 164 golpes. Los proyectiles no miran la prioridad del
  HitDef.
- **Juggle:**
  - quien recibe lleva, por cada atacante, los puntos que le quedan en el combo (al principio, el
    `data.airjuggle` del atacante, 15);
  - el atacante puede golpear si aún no es su objetivo, o si el `juggle` de su Statedef no pasa de esos
    puntos;
  - si el rival cae (antes o después del golpe), el golpe gasta ese `juggle` y el coste del atacante vuelve a 0
    hasta otro estado que lo fije (sin `juggle`, el estado nuevo no lo cambia);
  - un proyectil gasta su `air.juggle` y lo comprueba aunque aún no sea su objetivo;
  - al salir de movetype H, la lista se borra; `AssertSpecial nojugglecheck` lo desactiva.
- **`ReversalDef`:**
  - la Clsn1 del que revierte contra la **Clsn1** del golpe activo del rival, si el `attr` de este está en
    `reversal.attr`; va antes que los golpes normales;
  - el golpe se anula, no hay daño;
  - el que revierte cuenta `MoveHit`, se para el primer `pausetime` y pasa a `p1stateno`;
  - el otro cuenta `MoveReversed`, se para el segundo `pausetime` y, con `p2stateno`, pasa a ese estado (del
    que revierte, por defecto), en movetype H;
  - no revierte proyectiles.
- **`MoveContact`, `MoveHit`, `MoveGuarded`, `MoveReversed`** son contadores:
  - valen 1 desde el contacto hasta el primer tick sin hitpause, y después suben 1 por tick;
  - `MoveContact` no cuenta un golpe revertido;
  - un cambio de estado sin `movehitpersist` los pone a 0 (`MoveHitReset`, también);
  - `HitCount` suma el `numhits` del HitDef, y `HitAdd` lo que diga.
- **`InGuardDist`:**
  - es cierto si delante, a menos del `guard.dist`, hay alguien en **movetype A**, aunque su HitDef no esté
    activo o no exista: se puede bloquear desde el principio del ataque;
  - la comparación es `<`: `AttackDist = 0` (el uso típico, 854 de 901) quita la guardia;
  - se calcula en la detección de golpes y el trigger ve la del tick anterior;
  - el `guard.dist` de un HitDef que no lo trae se queda como estaba (lo cambia `AttackDist` y vuelve a
    `size.attack.dist` al cambiar de estado sin `hitdefpersist`).
- **`HitDefAttr = SC, NA`:** atacando (movetype A) con un HitDef de esos attr, aunque ya haya golpeado. Al
  cambiar de estado sin `hitdefpersist`, el HitDef se borra.
- **`cornerpush`:**
  - si el rival que golpeó (un jugador que sigue recibiendo, y no por un proyectil) está contra el borde de
    la pantalla, el atacante retrocede;
  - sin `*.cornerpush.veloff`, MUGEN usa 1,3 × `guard.velocity` (0 si el attr es aéreo) con un rozamiento de
    0,7 por tick hasta bajar de 1;
  - se fija con la postura del rival antes del golpe; no hay durante una SuperPause.
- **Lo que mira `attrCheck` antes de las cajas:**
  - `chainid`: solo golpea si el último golpe que recibió el rival tenía ese id (que vuelve a -1 cuando se
    recupera);
  - `nochainid`: no golpea si el último golpe de este atacante fue de ese id;
  - `hitflag`: quien cae necesita F **además de** A; `-` falla si está recibiendo o en guardia; `+` exige que
    reciba y no esté en guardia;
  - `NotHitBy`/`HitBy` comparan con la **postura real del atacante**, no con el SCA del HitDef (salvo un
    proyectil);
  - `affectteam` E (por defecto), B o F.
- **Más:**
  - `hitonce` (por defecto 1 en los agarres) suelta los demás objetivos;
  - el objetivo cuenta también si **bloquea**;
  - `p1getp2facing` hace mirar al atacante como el rival y el retroceso sigue esa dirección;
  - `TargetDrop` sin `excludeid` suelta todos;
  - `DefenceMulSet` en MUGEN multiplica el daño recibido, y solo mientras recibe;
  - `Power` de un helper es la del jugador;
  - `LifeAdd` aplica la defensa salvo con `absolute = 1`.

Bloquear desde el principio del ataque cambió mucho la IA. En Goku contra Krillin bajó de 35 golpes a 11 en
1.800 ticks.

## Notas / gotchas

- **Las trampas aparecen juntas:** con golpes simplificados, la física real basta para atascar a los
  personajes. Goku se quedó en su agarre (estado 820) un minuto entero porque la salida depende de
  `NumTarget(800)` y de que el rival pase a un estado suyo.
- **`NumTarget` de un helper no cuenta para su root** (y al revés).
- **MUGEN 1.0** (`mugenversion = 1.0` en los `.def` de Hyper DBZ). En 1.1 el motor aplica algunas cosas por
  su cuenta, como el umbral de rozamiento.
- **Los CNS reales tienen errores que MUGEN tolera:** en el agarre de Krillin (estado 813 de Hyper DBZ) el
  rival vuelve con `SelfState value = 1`, y ningún personaje tiene estado 1. El motor debe caer al estado 0
  sin romperse.
- **Cómo probarlo:** un personaje sintético sin sprites (solo `.air` con cajas) jugado con una entrada
  programada y un muñeco con estados de recibir como los de `common1.cns`. Se vuelcan posición, velocidad,
  vida y pausas de los dos en cada tick y se comparan con las reglas (gravedad, rozamiento, daño, hitpause,
  retroceso). Conviene comprobar que el test falla si se desactiva lo que prueba.
- **Sin cubrir todavía:**
  - las chispas y `projscale`;
  - `ReversalDef` contra `ReversalDef`;
  - un HitDef que golpea a varios rivales en ticks distintos;
  - el cambio de attr al conectar un agarre;
  - el redondeo del daño (Ikemen redondea, con un mínimo de 1).
- **El cambio de estado de `p1stateno`/`p2stateno`** se aplica en Ikemen al acabar el hitpause, y el estado
  nuevo borra `MoveHit` salvo con `movehitpersist`. Cambiar de estado en el acto da el mismo resultado, salvo
  durante el hitpause.

## Referencias cruzadas

- [Compilar la lógica CNS de MUGEN — semántica real y trampas](cns-compilacion.md)
- [Formatos de MUGEN — SFF, AIR y SND](formatos-sff-air-snd.md)
- [Simulación idéntica en PC y Dreamcast](../../Dreamcast/cpu/coma-flotante-determinista.md)

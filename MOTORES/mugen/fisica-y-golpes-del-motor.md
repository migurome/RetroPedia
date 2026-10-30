---
titulo: Física y golpes del motor de MUGEN 1.0 — lo que el CNS no hace por sí solo
plataforma: [General]
categoria: [motores]
tags: [mugen, cns, hitdef, clsn, hitpause, hitshake, guard, custom-state, targetstate, playerpush, screenbound, camara]
estado: borrador
fecha: 2026-10-30
fuentes: [documentación de Elecbyte (cns.html, sctrls.html, trigger.html), common1.cns de MUGEN 1.0, CNS de Hyper DBZ Wicked Ambition, proyecto hyperdbz-dreamcast-port (dc/proto/src/logic.c)]
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
  - `ChangeAnim` usa las animaciones propias y `ChangeAnim2`, las del dueño del estado;
  - `Const()` y `Name` siguen siendo los propios.
- **Se vuelve con `SelfState` o al recibir un golpe normal.**
- **Implementación:** cada actor guarda su banco (`own`) y el del dueño del estado actual (`lb`). La memoria de
  `persistent` del estado actual debe dimensionarse con el estado más grande **de los dos** personajes.

## Notas / gotchas

- **Las trampas aparecen juntas:** con golpes simplificados, la física real basta para atascar a los
  personajes. Goku se quedó en su agarre (estado 820) un minuto entero porque la salida depende de
  `NumTarget(800)` y de que el rival pase a un estado suyo.
- **`NumTarget` de un helper no cuenta para su root** (y al revés).
- **MUGEN 1.0** (`mugenversion = 1.0` en los `.def` de Hyper DBZ). En 1.1 el motor aplica algunas cosas por
  su cuenta, como el umbral de rozamiento.
- **Sin cubrir todavía:** prioridades de golpe (`priority`), juggle, `Projectile`, `ReversalDef`,
  `HitDefAttr` y las chispas.

## Referencias cruzadas

- [Compilar la lógica CNS de MUGEN — semántica real y trampas](cns-compilacion.md)
- [Formatos de MUGEN — SFF, AIR y SND](formatos-sff-air-snd.md)
- [Simulación idéntica en PC y Dreamcast](../../Dreamcast/cpu/coma-flotante-determinista.md)

---
titulo: Comandos del .cmd de MUGEN y movimientos del motor — reconocedor y trampas
plataforma: [General]
categoria: [motores]
tags: [mugen, cmd, comandos, input, buffer, holdfwd, holdback, andar, saltar, ia, ailevel]
estado: borrador
fecha: 2026-10-30
fuentes: [documentación de Elecbyte (cmd de kfm, cns.html), common1.cns de MUGEN 1.0, CNS y .cmd de Hyper DBZ Wicked Ambition, proyecto hyperdbz-dreamcast-port (tools/cns/commands.py, dc/proto/src/command.c)]
---

# Comandos del .cmd de MUGEN y movimientos del motor

## Resumen

Un personaje de MUGEN recibe la entrada como **comandos con nombre** (`command = "holdfwd"`, `"x"`...)
definidos en su `.cmd`. Para jugar hay que:
1. guardar la entrada de cada tick;
2. comprobar cada `[Command]` contra ese historial;
3. hacer los movimientos que **el motor** hace por su cuenta: andar, saltar y agacharse no están en el CNS.

## Detalle técnico

### Sintaxis de `command =`

| Símbolo | Significado |
|---|---|
| `,` | pasos en orden, todos dentro de `time` ticks (por defecto `command.time` de `[Defaults]`) |
| `+` | elementos del mismo paso, a la vez |
| `/x` | mantener |
| `~x`, `~30x` | soltar; con número, después de mantener al menos esos ticks (carga) |
| `$D` | cualquier dirección que incluya D (D, DF, DB) |
| `>` | sin **ninguna tecla pulsada o soltada** entre el paso anterior y este |
| `U D F B UF UB DF DB` | direcciones; F y B según hacia dónde mira el personaje |
| `a b c x y z s` | botones; **en minúscula**: `B` es atrás y `b` es el botón |

- **Varios `[Command]` pueden compartir nombre:** son alternativas.
- **`buffer.time`:** cuántos ticks sigue activo el comando tras completarse (por defecto, 1).

### Reconocedor

- **Historial:** 128 ticks de entrada cruda, con las direcciones absolutas. Opuestas a la vez se anulan.
- **Comprobación, cada tick y hacia atrás:**
  1. el último paso tiene que ocurrir ahora;
  2. cada paso anterior se busca en el tick más reciente anterior, sin salirse de `time`.
- **Cuándo "ocurre" un elemento:** pulsar = empieza en ese tick; mantener = está; soltar = acaba (y antes
  llevaba al menos la carga).
- **Dos pasos distintos pueden ocurrir en el mismo tick, y hace falta:** en `~D, DF, F, x` (media luna),
  se suelta D en el mismo tick en que se pulsa DF. Dos pasos iguales (`F, F`) necesitan dos pulsaciones.
- **`>` cuenta también las teclas soltadas:** `a, >b` no se cumple con a, soltar a, b; sí con b pulsado
  mientras a sigue pulsado o en el mismo tick en que se suelta.
- **F y B:** se calculan con el facing **actual**, no con el de cuando se pulsó.

### Lo que hace el motor, y no el CNS (MUGEN 1.0)

Con un jugador **con control**:
- **De pie:** mantener arriba → 40 (salto), abajo → 10 (agacharse), adelante o atrás → 20 (andar).
- **En el 20 sin adelante ni atrás** → 0. **En el 11 sin abajo** → 12 (levantarse).
- **Manteniendo atrás con un ataque enemigo cerca** (`InGuardDist`) → 120 (guardia).

Se aplican después del estado −1, para que los ataques que lanza el −1 tengan prioridad. Los nombres que usa
son los del `.cmd` (`holdfwd`, `holdback`, `holdup`, `holddown`), que todos los personajes definen.
`AssertSpecial nowalk` impide andar, y `noautoturn` impide el giro automático.

### IA

Con `AILevel > 0`, MUGEN activa comandos al azar y la IA de cada personaje (en su CNS, con `AILevel`) decide.
El jugador humano tiene `AILevel = 0`. Como las guardas que dependen de `AILevel` se calculan al crear el
actor (ver [compilación del CNS](cns-compilacion.md)), cambiar entre CPU y persona obliga a reiniciar el
combate.

### Datos de Hyper DBZ

- **Comandos sencillos:** 1.622 `[Command]` en 22 personajes, casi todos de un botón o una dirección. Los
  especiales (medias lunas) los resuelve cada personaje en su CNS con un buffer propio en variables.
- **Más de 64 nombres:** hasta 106 (Babidi). Secuencias de hasta 7 pasos (`~D,DF,F,D,DF,F,a+c`).

## Notas / gotchas

- No pasar a minúsculas el valor de `command =`, o `B` (atrás) se convierte en el botón `b`.
- Los estados 20 (andar) y 11 (agachado) de `common1.cns` **no tienen salida**: sin el motor, el personaje
  se queda andando o agachado para siempre.
- Probar con secuencias guionizadas tick a tick (pulsar, mantener, soltar, carga, media luna lenta, `>`,
  `+`, facing −1) antes de probar con el mando.

## Referencias cruzadas

- [Física y golpes del motor de MUGEN 1.0](fisica-y-golpes-del-motor.md)
- [Compilar la lógica CNS de MUGEN](cns-compilacion.md)
- [Bus Maple, mando y VMU](../../Dreamcast/perifericos/maple-mando-y-vmu.md)

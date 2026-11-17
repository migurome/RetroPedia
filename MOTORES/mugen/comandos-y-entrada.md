---
titulo: Comandos del .cmd de MUGEN y movimientos del motor — reconocedor y trampas
plataforma: [General]
categoria: [motores]
tags: [mugen, mando, memoria, arquitectura]
estado: revisado
comprobado: emulador
comprobado_en: "parcial: Flycast 2.7, hyperdbz-dreamcast-port desde hdbz.1.013; Goku jugable, y test_comandos.py en PC con los 1.622 comandos de los 22 personajes; la IA del motor, desde la Fase 6 (test_ia, tools/ia.py)"
fecha: 2026-10-30
fuentes: [CHANGELOG de hyperdbz-dreamcast-port (hdbz.1.013) y tools/qa/test_comandos.py, documentación de Elecbyte (cmd de kfm, cns.html), common1.cns de MUGEN 1.0, CNS y .cmd de Hyper DBZ Wicked Ambition, proyecto hyperdbz-dreamcast-port (tools/cns/commands.py, dc/proto/src/command.c)]
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

La IA de cada personaje está en su CNS (con `AILevel`; Goku de Hyper DBZ lo usa 489 veces). Además, el **motor** hace dos cosas
con un jugador de la CPU (`AILevel > 0`; la persona tiene 0). El código de MUGEN no es público; esto es lo que hace Ikemen GO
(`input.go`, `AiInput.Update`, y `char.go`, `commandUpdate` y `command`):

| Qué | Cómo | Para qué |
|---|---|---|
| Entrada al azar («button jamming») | Cada tick, 1 entre 15 de empezar a mantener una dirección al azar (de 8) de 1 a 60 ticks; cada botón, 1 entre `(165 − 11,25 · AILevel) · 7` (525 en el 8, 1076 en el 1) de mantenerse de 1 a 30 ticks; START, 1 entre 3600. Nada en las intros ni en las poses de victoria | Pasa por el reconocedor de comandos como la entrada de una persona: la CPU anda, salta, se agacha y se cubre «de serie» |
| Comando regalado («AI cheating») | Un comando al azar da verdadero un tick, solo si es de varios pasos o varios botones a la vez (los de uno ya los pulsa la entrada al azar) | Los especiales que la entrada al azar no completaría |

**Frecuencia del comando regalado:** según un comentario de Ikemen GO, en MUGEN el trigger `command` da verdadero de media una vez
cada 10 s en la dificultad 8 y cada 30 s en la 1 (entre medias, en línea recta). Ikemen lo hace mucho más a menudo (cada tick, una
probabilidad de `(AILevel/2) / (AILevel/2 + 32)`: el 11 % en el 8) y lo dice: «más eficiente, no fiel a MUGEN».

En el port de Hyper DBZ (paso 6.1, `ai_input` y `ai_cheat` en `logic.c`):
- La frecuencia del regalado es la de MUGEN, no la de Ikemen.
- La entrada al azar tiene su propio generador: no gasta el `Random` del CNS.
- Sin persona ni IA (`AILevel 0`: el muñeco de la práctica) no hay entrada. Hasta la 5.108 la CPU recibía uno o dos comandos
  cualesquiera en el 70 % de los ticks, también con `AILevel 0`.
- Los registros de partida de antes se reproducen con la CPU de entonces (la cabecera lleva la build).

Medida con `tools/ia.py` (cada personaje contra sí mismo, P1 en los niveles 1, 4 y 8, P2 en el 4): la dificultad sube con el
nivel en todos los que tienen IA; los que casi no tienen (Vegetto) apenas cambian. Como las guardas que dependen de `AILevel` se
calculan al crear el actor (ver [compilación del CNS](cns-compilacion.md)), cambiar entre CPU y persona obliga a reiniciar el
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

## Lo que se aprendió con Hyper DBZ (Ikemen GO como referencia)

- **Un helper con `keyctrl` lee adelante y atrás con su propia orientación,** no con la del jugador (`fbFlip`). Los personajes Z2 lo aprovechan: un
  helper que no gira (90000005) da las direcciones absolutas, y las técnicas las leen con `ifelse(facing = 1, derecha, izquierda)`. Una VM que le
  pase los comandos del jugador invierte esas técnicas al cruzar, aunque andar y lo que va por el `.cmd` funcionen bien.
- **Giro automático:** con control en los estados 0, 11 y 20, o en el 52 con `AnimTime = 0`, antes de leer el mando; de pie o agachado, pasa a la
  animación 5 o 6. También al hacer `ChangeState` con control de pie o agachado (MUGEN sin documentar).

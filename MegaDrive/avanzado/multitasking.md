---
titulo: Multitarea en Mega Drive
plataforma: [MegaDrive]
categoria: [avanzado]
tags: [multitarea, scheduler, interrupciones, supervisor-mode, usp]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/multitasking]
---

# Multitarea en Mega Drive

## Resumen

Presenta un planificador (scheduler) primitivo de multitarea expulsiva (preemptive) para el 68000 en Mega Drive, que divide la ejecución del juego en una tarea en primer plano (a framerate completo, en modo supervisor) y una tarea en segundo plano (en modo usuario, que consume los ciclos de CPU restantes para operaciones largas).

## Detalle técnico

Tal como lo describe el autor: "divide la ejecución del juego en dos tareas separadas que se ejecutan de forma independiente."

Operan dos tipos de tarea:

- **Tarea en primer plano (foreground)**: se ejecuta a framerate completo en modo supervisor, gestionando la lógica crítica de cada frame.
- **Tarea en segundo plano (background)**: se ejecuta en modo usuario, consumiendo los ciclos de CPU restantes para operaciones largas como "un renderizador por software para un juego 3D, o cálculos de simulación."

### Variables de RAM necesarias

```
BgTaskSr:       rs.w 1  ; Background task's SR register
BgTaskPc:       rs.l 1  ; Background task's PC register
BgTaskRegs:     rs.l 15 ; Background task's d0-d7 and a0-a6
BgTaskRegsEnd:  rs.l 0  ; (for use with MOVEM)
```

### Subrutina SetUpBgTask

```
SetUpBgTask:
    move.l  a5, usp
    move.l  a6, (BgTaskPc)
    clr.w   (BgTaskSr)
    rts
```

Parámetros de entrada: `a5` = puntero de pila, `a6` = punto de entrada

### Subrutina StopBgTask

```
StopBgTask:
    lea     @Stub(pc), a6
    bra     SetUpBgTask

@Stub:
    bra.s   @Stub
```

### Subrutina YieldToBgTask

```
YieldToBgTask:
    move.w  sr, d7
    move.w  #$2700, sr
    move.w  d7, -(sp)
    
    lea     (BgTaskRegs+4), a0
    movem.l (a0)+, d0-d7/a1-a6
    move.l  (BgTaskRegs), a0
    
    move.l  (BgTaskPc), -(sp)
    move.w  (BgTaskSr), -(sp)
    rte
```

Esta instrucción "destruye (clobbers) todos los registros d0-d7/a0-a6 al retornar para la tarea en primer plano."

### Manejador TaskSwitchIrq

```
TaskSwitchIrq:
    btst    #5, (sp)
    bne.s   @NoBgTask
    
    move.l  a0, (BgTaskRegs)
    lea     (BgTaskRegsEnd), a0
    movem.l d0-d7/a1-a6, -(a0)
    
    move.w  (sp)+, (BgTaskSr)
    move.l  (sp)+, (BgTaskPc)

@NoBgTask:
    rte
```

El manejador comprueba el bit 5 del registro de estado (status register) en la pila para detectar el modo supervisor.

## Notas / gotchas

- No se puede usar `usp` como variable temporal, ya que sirve como puntero de pila de la tarea en segundo plano.
- "Todos los peligros habituales de las interrupciones también aplican aquí. Ten cuidado al acceder a hardware."
- La tarea en segundo plano no puede manipular directamente el enmascarado de interrupciones (aunque `trap` y los opcodes `$A000-$AFFF` sí evitan esta restricción).
- Con múltiples tareas es necesario guardar también `a7` junto con el resto de registros.

## Referencias cruzadas

- (pendiente de enlazar)

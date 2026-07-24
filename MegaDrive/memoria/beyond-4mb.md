---
titulo: Más allá de 4MB (bank switching / mappers)
plataforma: [MegaDrive]
categoria: [memoria]
tags: [bank-switching, mapper, a130xx, rom, ssf2]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/beyond-4mb]
---

# Más allá de 4MB (bank switching / mappers)

## Resumen

Describe las técnicas de bank switching (cambio de banco) para superar la limitación estándar de 4MB de ROM mediante mappers basados en cartucho, usando los registros de mapeo situados en `$A130Fx`.

## Detalle técnico

### Configuración de la cabecera de la ROM

Para habilitar el soporte de mapper, el campo de sistema en la dirección `$000100` debe fijarse en `"SEGA SSF"` (relleno con espacios) en lugar del estándar `"SEGA MEGA DRIVE"` o `"SEGA GENESIS"`. Esta configuración funciona con Mega Everdrive, Mega SD, BlastEm y plataformas similares. Algunos emuladores como Kega Fusion pueden autodetectar el uso de mapper basándose en escrituras a los registros, por lo que se recomienda inicializar el mapper cuanto antes.

### Arquitectura de bank switching

La ROM se divide en páginas de 512KB, mientras que la consola direcciona 4MB divididos en bancos de 512KB. Cada banco de la consola mapea a una página de la ROM mediante registros dedicados. El banco 0 (direcciones `$000000-$07FFFF`) apunta permanentemente a la página 0 para permitir un arranque seguro.

**Registros del mapper:**

| Dirección | Banco | Rango de direcciones de consola |
|---------|------|----------------------|
| `$A130F3` | 1 | `$080000-$0FFFFF` |
| `$A130F5` | 2 | `$100000-$17FFFF` |
| `$A130F7` | 3 | `$180000-$1FFFFF` |
| `$A130F9` | 4 | `$200000-$27FFFF` |
| `$A130FB` | 5 | `$280000-$2FFFFF` |
| `$A130FD` | 6 | `$300000-$37FFFF` |
| `$A130FF` | 7 | `$380000-$3FFFFF` |

### Implementación en ensamblador

Definir alias de registros:

```
MapperBank1: equ $A130F3
MapperBank2: equ $A130F5
MapperBank3: equ $A130F7
MapperBank4: equ $A130F9
MapperBank5: equ $A130FB
MapperBank6: equ $A130FD
MapperBank7: equ $A130FF
```

Inicializar el mapeo estándar de 4MB:

```
move.b  #1, (MapperBank1)
move.b  #2, (MapperBank2)
move.b  #3, (MapperBank3)
move.b  #4, (MapperBank4)
move.b  #5, (MapperBank5)
move.b  #6, (MapperBank6)
move.b  #7, (MapperBank7)
```

Acceder a páginas de ROM adicionales (ejemplo: juego de 5MB usando las páginas 8-9):

```
move.b  #8, (MapperBank6)
move.b  #9, (MapperBank7)
```

## Notas / gotchas

- El circuito del mapper reside en el cartucho, lo que requiere una implementación en CPLD y afecta a los costes de producción.
- El procesador Z80 debe mantenerse sincronizado con los cambios de banco que afecten a los datos de sonido; el enfoque más seguro es mantener el audio en bancos fijos.
- El tamaño máximo de ROM depende del hardware: los cartuchos físicos están limitados por la memoria instalada (menos una posible sobrecarga de firmware); el soporte en emuladores varía.

## Referencias cruzadas

- (pendiente de enlazar)

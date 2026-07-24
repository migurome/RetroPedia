---
titulo: Puerto de depuración del VDP
plataforma: [MegaDrive]
categoria: [graficos]
tags: [vdp, referencia, depuracion, z80, psg, sprites]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/vdp-debug]
---

# Puerto de depuración del VDP

## Resumen

Documenta el puerto de depuración del VDP (`$C0001C`), que permite alterar el comportamiento interno del chip (mezcla de capas, override del PSG, fases de renderizado de sprites, velocidad del reloj del Z80). El propio autor advierte que este registro no está completamente entendido y que la página solo recoge lo que se puede documentar con confianza a día de hoy.

## Detalle técnico

### Comentarios generales

Existen varios registros de depuración en el VDP. Para seleccionar un registro, hay que escribir su número en los bits 11-8 de `$C00018`, y luego acceder al registro a través de `$C0001C`. Los registros 0 a 7 son escribibles, el registro 15 es legible.

- Nada de lo que desactiva alguna parte del renderizado afecta al ancho de banda de VRAM. No esperar ganar espacio extra para cargar gráficos usando este registro.
- Escribir en este puerto va directo a distintas partes del VDP (en vez de quedar correctamente enclavado como en los demás puertos), así que dependiendo del prefetch y otras cosas en curso hay riesgo de corromper la VRAM al escribir en él.
- Además, en algunos VDP con solo activar ciertos bits se acaba corrompiendo la CRAM con el tiempo. Lo que hay en este registro no es nada estable.

### Registro de depuración 0

#### Bits 8-6: mezcla de capas

El más interesante del grupo, y el que _Overdrive 2_ adora explotar en muchos de sus efectos. Desactiva y activa capas, y es posible hacer que el VDP intente mostrar dos capas al mismo tiempo.

- **Bit 6:**
  - `0`: renderizado normal
  - `1`: ocultar todas las capas
- **Bits 8-7:**
  - `00`: no hacer nada
  - `01`: forzar que se muestre la capa de sprites
  - `10`: forzar que se muestre el plano A
  - `11`: forzar que se muestre el plano B

Cuando el bit 6 está activado, usar los bits 8-7 permite mostrar una sola capa. Esto puede haber sido útil para depurar en hardware real. La capa elegida se fuerza a mostrarse incluso en el área de borde, por lo que se verán píxeles que normalmente se renderizan fuera de pantalla, así como ruido de los patrones de acceso a memoria en las áreas no cubiertas por el plano.

Lo interesante ocurre cuando el bit 6 está desactivado _y_ se fuerza una capa con los bits 8-7. En este caso, el VDP intentará mostrar tanto lo que normalmente saldría _como_ la capa elegida, lo que resulta en que ambas salidas se combinan con un AND (concretamente, sus índices de paleta/color). _Overdrive 2_ explota esto para todo tipo de trucos.

**Advertencia del autor:** "This part is _really_ not stable. The AND effect is the result of a bus fight, and some VDP chips (mainly early ones, but also a few of the later ones) will end up displaying a horrible mess instead (or in some cases, do the effect but garble up the sprite layer). Don't expect it to work fine on every console. You have been warned if you intend to use this." (Esta parte es _muy_ inestable. El efecto AND es resultado de una pelea de bus, y algunos chips VDP —sobre todo los primeros, pero también algunos posteriores— acaban mostrando un desastre en su lugar, o en algunos casos hacen el efecto pero corrompen la capa de sprites. No esperar que funcione bien en todas las consolas.)

#### Bits 11-9: override del PSG

Cuando el bit 9 está desactivado, el PSG funciona con normalidad. Cuando está activado, en cambio, todos los canales del PSG se reemplazan por el nivel de volumen de uno de ellos. Los bits 11-10 eligen qué canal es la fuente del nivel de volumen.

#### Bits 14-12: renderizado de sprites

Cada uno de estos bits interfiere con una de las fases de renderizado de sprites (presumiblemente alternan si cada fase se ejecuta o no). Activarlos puede resultar en cosas como que los sprites no se muestren o que la misma línea de sprites se repita por toda la pantalla, dependiendo de qué fase se alteró y en qué momento.

### Registro de depuración 1

#### Bit 0: velocidad del reloj del Z80

El bit 0 alterna la velocidad de ZCLK (reloj del Z80 y del PSG):

- `0` selecciona MCLK÷15 (unos 3.58MHz en 60Hz)
- `1` selecciona MCLK÷7 (unos 7.67MHz en 60Hz)

Activar este bit en realidad selecciona VCLK (el reloj del 68000 y del YM2612). Este bit afecta tanto al Z80 como al PSG, por lo que el PSG se vuelve aún más agudo.

**Advertencia del autor:** algunas consolas vienen con RAM de Z80 lenta y el Z80 está especificado para hasta 4MHz como máximo, así que no hay ninguna garantía de que hacer overclock de esta forma sea fiable. También afectará a cualquier driver de sonido que dependa mucho del conteo de ciclos.

#### Bit 1: función del pin EDCLK

Este bit alterna qué hace el pin EDCLK:

- `0` hace que el pin tome EDCLK como entrada
- `1` hace que el pin saque DCLK en su lugar

**Advertencia del autor:** hay que recordar que la consola sigue generando EDCLK por su cuenta independientemente de este pin, así que en una Mega Drive lo único que se obtiene es que ambas fuentes de reloj compitan entre sí sin lograr nada útil.

## Notas / gotchas

- El propio autor (Iwis) avisa: "This page is missing _a lot_ of stuff, it only includes whatever I'm confident to document right now." (Esta página se deja _muchísimas_ cosas fuera, solo incluye lo que el autor tiene confianza en documentar ahora mismo).
- Escribir en este puerto no pasa por el latching normal de los demás puertos del VDP: hay riesgo real de corrupción de VRAM, e incluso de corrupción progresiva de CRAM en algunos chips VDP con ciertos bits activados.
- Desactivar partes del renderizado mediante este puerto NO libera ancho de banda de VRAM.
- El efecto de "mezcla AND" entre capas (bits 8-6) depende de una pelea de bus real y su comportamiento varía entre revisiones de VDP; puede corromper visualmente la capa de sprites en chips inconsistentes.
- Overclockear el Z80/PSG con el bit 0 del registro 1 puede exceder la velocidad nominal del Z80 (4MHz) y romper drivers de sonido que dependan de conteo de ciclos preciso.
- Activar el pin EDCLK como salida DCLK (bit 1 del registro 1) provoca que compitan dos fuentes de reloj sin ningún beneficio, ya que la consola sigue generando su propio EDCLK.
- La página no incluye ejemplos de código en C o ensamblador.

## Referencias cruzadas

- (pendiente de enlazar)

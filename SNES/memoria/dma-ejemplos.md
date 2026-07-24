---
titulo: DMA/HDMA — ejemplos de uso
plataforma: [SNES]
categoria: [memoria]
tags: [dma, hdma, ejemplos, oam, vram, tilemap]
estado: borrador
fecha: 2026-07-21
fuentes: [https://snes.nesdev.org/wiki/DMA_examples]
---

# DMA/HDMA — ejemplos de uso

## Resumen

Colección de casos de uso típicos del DMA general de la SNES (no HDMA) construidos sobre los registros de canal descritos en `dma-registers`: copiar el buffer de OAM entero de un tirón, actualizar columnas/filas de un tilemap durante el scroll, y rellenar zonas de VRAM o WRAM con un valor fijo usando el modo de "dirección constante". Sirve como referencia rápida de qué combinación de patrón de transferencia + modo de dirección + registro destino usar para cada tarea habitual.

## Detalle técnico

### Nombres de registro usados en los ejemplos

La página define alias amigables para el bloque de registros de un canal de DMA (equivalentes a los `DMAPn`/`BBADn`/`A1TnL`/`A1TnH`/`A1Bn`/`DASnL`/`DASnH` de `dma-registers`, normalmente aplicados al canal 0):

- `DMAMODE` (`$4300`): parámetros de transferencia (patrón + modo de dirección + dirección de copia).
- `DMAPPUREG` (`$4301`): registro del bus B destino/origen.
- `DMAADDR` (`$4302`-`$4303`): dirección de origen de 16 bits en el bus A.
- `DMAADDRBANK` (`$4304`): banco de la dirección de origen.
- `DMALEN` (`$4305`-`$4306`): número de bytes a transferir.

Los flags de configuración de `DMAMODE` que usan los ejemplos son combinables por OR:

| Flag | Valor | Significado |
|---|---|---|
| `DMA_LINEAR` | `$00` | Patrón de 1 byte |
| `DMA_01` | `$01` | Patrón de 2 bytes, B+0/B+1 |
| `DMA_00` | `$02` | Patrón de 2 bytes, B+0/B+0 |
| `DMA_0011` | `$03` | Patrón de 4 bytes, B+0/B+0/B+1/B+1 |
| `DMA_0123` | `$04` | Patrón de 4 bytes, B+0/B+1/B+2/B+3 |
| `DMA_0101` | `$05` | Patrón de 4 bytes, B+0/B+1/B+0/B+1 |
| `DMA_FORWARD` | `$00` | Incrementar dirección de origen |
| `DMA_CONST` | `$08` | Dirección de origen fija |
| `DMA_BACKWARD` | `$10` | Decrementar dirección de origen |
| `DMA_INDIRECT` | `$40` | Modo indirecto (HDMA) |
| `DMA_READPPU` | `$80` | Dirección de copia B→A |

### Ejemplo: copiar el buffer de OAM completo

Caso más simple de DMA: enviar el buffer de sombra de OAM (en RAM del sistema) a la tabla de sprites del PPU de un solo golpe. Pasos: reiniciar `OAMADD` (puntero de escritura de OAM) a 0, configurar `DMAMODE`/`DMAPPUREG` para apuntar a `OAMDATA`, fijar `DMAADDR`/`DMAADDRBANK` a la dirección del buffer en RAM, fijar `DMALEN` a 544 bytes (tamaño completo de la tabla de OAM, incluida la tabla de alta prioridad), y finalmente activar el canal correspondiente en `MDMAEN` para disparar la transferencia.

### Ejemplo: actualización de scroll (columna de tilemap)

Al desplazar el fondo y descubrir una columna nueva de tiles hay que subir esa columna a VRAM. Se configura `VMAIN` para que los accesos a `VMDATA` incrementen la dirección de VRAM verticalmente (para escribir una columna en vez de una fila), y se lanza un DMA de 32 tiles × 2 bytes hacia `VMDATA`.

### Ejemplo: actualización de filas de un tilemap

Para tilemaps de 64 tiles de ancho (que en VRAM están partidos en dos mitades de 32×32 no contiguas), actualizar una fila completa requiere dos transferencias DMA separadas (una por cada mitad del ancho). Si se quieren refrescar dos filas a la vez, la wiki describe la variante con cuatro transferencias DMA independientes, una por cada combinación de mitad-de-ancho × fila.

### Ejemplo: transferencias con dirección de origen fija (`DMA_CONST`)

Tres variantes que aprovechan el modo de dirección fija para "rellenar" un destino con un valor constante en vez de copiar un bloque de datos:

- **Vaciar WRAM:** escribir un valor fijo repetidamente vía DMA hacia `WMDATA`. Importante: el origen de esta transferencia concreta no puede estar en la propia WRAM (ver más abajo).
- **Rellenar VRAM byte a byte:** usar como origen un byte fijo en página cero y como destino `VMDATA`, con patrón de 1 byte.
- **Rellenar VRAM palabra a palabra:** al no existir un patrón de "repetir 1 palabra" con origen fijo, hacen falta dos transferencias DMA seguidas, una hacia el registro de byte bajo y otra hacia el de byte alto de `VMDATA`.

## Notas / gotchas

- **La dirección de origen de una transferencia DMA hacia `WMDATA` no puede estar en la propia WRAM** — es una limitación de hardware documentada explícitamente en la wiki, no un simple detalle de estilo.
- Los primeros modelos de SNES tienen un fallo conocido si una transferencia de DMA general termina justo cuando arranca el HDMA de la siguiente línea; los ejemplos de esta página muestran secuencias de registros pensadas para evitar ese solape.
- Esta entrada documenta la lógica y los valores de registro exactos de cada ejemplo (nombres de registro, flags, tamaños en bytes), pero **no reproduce las listas de instrucciones ensamblador línea por línea del original**, ya que la herramienta de extracción usada no permitió obtener el código fuente verbatim; para el listado ensamblador completo hay que consultar directamente `https://snes.nesdev.org/wiki/DMA_examples` (contenido bajo licencia CC0).

## Referencias cruzadas

- (pendiente de enlazar)

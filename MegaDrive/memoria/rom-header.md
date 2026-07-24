---
titulo: Cabecera de la ROM
plataforma: [MegaDrive]
categoria: [memoria]
tags: [rom-header, tmss, checksum, region, sram]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/rom-header]
---

# Cabecera de la ROM

## Resumen

Describe el formato completo de la cabecera de la ROM de Mega Drive/Genesis, ubicada en las direcciones `$100-$1FF` justo después de los vectores del 68000. Contiene metadatos del juego: título, copyright, fecha de lanzamiento, número de serie, dispositivos soportados y configuración de memoria. Los emuladores usan estos datos para mostrar información e inicializar periféricos.

## Detalle técnico

### Estructura completa de la cabecera

| Dirección | Tamaño | Campo |
|---------|------|-------|
| `$100` | 16 bytes | Tipo de sistema |
| `$110` | 16 bytes | Copyright y fecha de lanzamiento |
| `$120` | 48 bytes | Título del juego (doméstico) |
| `$150` | 48 bytes | Título del juego (internacional) |
| `$180` | 14 bytes | Número de serie |
| `$18E` | 2 bytes | Checksum de la ROM |
| `$190` | 16 bytes | Soporte de dispositivos |
| `$1A0` | 8 bytes | Rango de direcciones de ROM |
| `$1A8` | 8 bytes | Rango de direcciones de RAM |
| `$1B0` | 12 bytes | Memoria extra (SRAM/FRAM/EEPROM) |
| `$1BC` | 12 bytes | Soporte de módem |
| `$1C8` | 40 bytes | Reservado (espacios) |
| `$1F0` | 3 bytes | Soporte de región |
| `$1F3` | 13 bytes | Reservado (espacios) |

### Tipo de sistema

**Advertencia crítica:** "Esta es la única parte de la cabecera a la que la consola presta atención. Si no encuentra el texto inicial 'SEGA' en este campo, los sistemas con TMSS se negarán a arrancar el juego."

Valores aceptados: `SEGA MEGA DRIVE`, `SEGA GENESIS`, `SEGA 32X`, `SEGA EVERDRIVE`, `SEGA SSF`, `SEGA MEGAWIFI`, `SEGA PICO`, `SEGA TERA68K`, `SEGA TERA286`

### Copyright y fecha de lanzamiento

Formato: `(C)XXXX YYYY.ZZZ`

- `XXXX`: abreviatura de cuatro caracteres del publisher
- `YYYY`: año de lanzamiento de cuatro dígitos
- `ZZZ`: abreviatura del mes de tres letras (mayúsculas)

Código de ejemplo:

```
dc.b  '(C)SEGA 1992.NOV'  ; Sonic 2's copyright!
```

### Títulos del juego

Dos campos separados de 48 bytes para acomodar diferencias de localización. El texto debe ir en mayúsculas y relleno con espacios. "Este es el único campo donde el texto _no_ tiene que ser ASCII: también se permite usar caracteres Shift-JIS."

Código de ejemplo:

```
dc.b  'CHAMPION FINGER WRESTLING'  ; Domestic title
dcb.b $150-*, ' '                  ; (padding)
dc.b  'CHAMPION FINGER WRESTLING'  ; Overseas title
dcb.b $180-*, ' '                  ; (padding)
```

### Número de serie

Formato: `XX YYYYYYYY-ZZ`

- `XX`: tipo de software (normalmente `GM` para juegos; también `AI`, `OS`, `BR`)
- `YYYYYYYY`: identificador único del juego
- `ZZ`: número de revisión (`00` para el lanzamiento inicial, `01` para el primer parche, etc.)

Código de ejemplo:

```
dc.b  'GM 00001051-00'  ; Serial number
```

### Checksum de la ROM

Valor de 16 bits calculado sumando cada palabra de 16 bits desde la dirección `$000200` hasta el final de la ROM, conservando solo los bits inferiores. Algunos juegos lo verifican; los emuladores también pueden comprobarlo.

### Soporte de dispositivos

Una letra por cada dispositivo soportado; el espacio no usado se rellena con espacios. Común: `J` (3 botones) o `J6` (3 y 6 botones).

**Advertencia importante:** "Los mandos de 6 botones solo deben añadirse a la lista si el juego realmente hace uso de los botones extra. _No_ hay que añadirlo simplemente para indicar que el juego funcionará con esos mandos."

Dispositivos soportados:

- `J`: mando de 3 botones
- `6`: mando de 6 botones
- `0`: mando de Master System
- `A`: joystick analógico
- `4`: multitap
- `G`: pistola de luz
- `L`: Activator
- `M`: ratón
- `B`: trackball
- `T`: tableta
- `V`: paddle
- `K`: teclado o teclado numérico
- `R`: RS-232
- `P`: impresora
- `C`: CD-ROM (Sega CD)
- `F`: unidad de disquete
- `D`: descarga

Código de ejemplo:

```
dc.b  'J6'         ; Device support
dcb.b $1A0-*, ' '  ; (padding)
```

### Rango de direcciones de ROM

Dos valores de 32 bits que indican la cobertura del cartucho. El primer valor siempre es `$000000`; el segundo típicamente es el tamaño de la ROM menos uno (ajustado si hay mappers).

Código de ejemplo:

```
dc.l  $000000    ; ROM start address
dc.l  RomSize-1  ; ROM end address
```

Tamaños comunes:

- `$07FFFF`: 512KB
- `$0FFFFF`: 1MB
- `$1FFFFF`: 2MB
- `$3FFFFF`: 4MB

### Rango de direcciones de RAM

Dos valores de 32 bits. Para Mega Drive, siempre:

```
dc.l  $FF0000  ; RAM start address
dc.l  $FFFFFF  ; RAM end address
```

### Memoria extra

#### Sin memoria extra

Rellenar el campo con espacios.

#### SRAM/FRAM

Estructura (12 bytes en total):

- 2 bytes: `RA`
- 1 byte: tipo de RAM
- 1 byte: `$20`
- 4 bytes: dirección de inicio
- 4 bytes: dirección de fin

Valores de tipo de RAM:

- `$A0`: sin guardado, 16 bits
- `$B0`: sin guardado, 8 bits (direcciones pares)
- `$B8`: sin guardado, 8 bits (direcciones impares)
- `$E0`: con guardado, 16 bits
- `$F0`: con guardado, 8 bits (direcciones pares)
- `$F8`: con guardado, 8 bits (direcciones impares)

Código de ejemplo:

```
dc.b 'RA',$F8,$20    ; SRAM type
dc.l $200001         ; SRAM start address
dc.l $20FFFF         ; SRAM end address
```

#### EEPROM

```
dc.b 'RA',$E8,$40    ; EEPROM type
dc.l $200001         ; EEPROM start address
dc.l $200001         ; EEPROM end address
```

**Advertencia:** "Esto no incluye información sobre cómo está cableada la EEPROM, y distintos publishers optaron por diferentes formas de cablearla."

### Soporte de módem

Si no hay soporte, rellenar con espacios. Si hay soporte, formato: `MOxxxxyy,zww` (nótese la coma)

- `xxxx`: publisher (del campo de copyright)
- `yy`: número de juego
- `z`: versión (`0` original, luego `1`, `2`, etc.)
- `ww`: región y soporte de micrófono

Valores de región/micrófono:

- `00`: Japón (sin mic)
- `10`: Japón (con mic)
- `20`: internacional (sin mic)
- `30`: internacional (con mic)
- `40`: Japón e internacional (sin mic)
- `50`: Japón e internacional (con mic)
- `60`: Japón (sin mic) e internacional (con mic)
- `70`: Japón (con mic) e internacional (sin mic)
- `80`, `90`: reservado (no documentado)

**Nota:** "Algunos juegos parecen haber usado un punto en vez de una coma (es decir, 'MOxxxxyy.zww'), así que es posible que ambos fueran válidos."

### Soporte de región

#### Estilo antiguo (recomendado)

Una letra por región; las posiciones no usadas se rellenan con espacios. El orden no importa.

Regiones soportadas:

- `J`: Japón
- `U`: América
- `E`: Europa

Ejemplo: `JUE` para soporte mundial; `J  ` para Japón solamente.

**Advertencia crítica:** "Si estás escribiendo código para comprobar la región de un juego, ten en cuenta que en algunos casos los espacios están en medio o al principio, ¡no solo al final! Comprueba siempre los tres bytes."

#### Estilo nuevo

Un único carácter ASCII hexadecimal (0-F); el resto relleno con espacios.

Estructura de bits:

- Bit 0 (+1): Japón, 60Hz
- Bit 1 (+2): doméstico, 50Hz
- Bit 2 (+4): América, 60Hz
- Bit 3 (+8): Europa, 50Hz

Mapeo de carácter a región:

- `1`: solo Japón
- `5`: Japón + América
- `9`: Japón + Europa
- `D`: Japón + América + Europa
- `7`: Japón + doméstico 50Hz + América
- `B`: Japón + doméstico 50Hz + América + Europa
- `F`: todas las regiones (Japón + doméstico 50Hz + América + Europa)
- (combinaciones adicionales: `2`, `3`, `4`, `6`, `8`, `A`, `C`, `E`)

## Notas / gotchas

- Si no aparece "SEGA" al inicio del campo de tipo de sistema, los sistemas con TMSS se negarán a arrancar el juego.
- Los 6 botones solo deben declararse si el juego los usa realmente, no como mera indicación de compatibilidad.
- El campo de título es el único que admite Shift-JIS además de ASCII.
- Comprobar siempre los tres bytes del campo de región antiguo, ya que los espacios pueden estar en medio o al principio.
- **Nota de ambigüedad sobre el estilo nuevo de región:** "Sí, esto significa que 'E' es ambiguo, ya que en el estilo antiguo significa solo Europa, pero en el estilo nuevo significa todo menos Japón. En la práctica es poco probable encontrarlo en el estilo nuevo (ya que incluye la región 'inválida' sin motivo real), así que se puede asumir que realmente significa Europa."

## Referencias cruzadas

- (pendiente de enlazar)

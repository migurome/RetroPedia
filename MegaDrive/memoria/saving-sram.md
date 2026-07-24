---
titulo: Guardado en SRAM
plataforma: [MegaDrive]
categoria: [memoria]
tags: [sram, guardado, cabecera-rom, bateria, checksum]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/saving-sram]
---

# Guardado en SRAM

## Resumen

Explica cómo habilitar y usar la SRAM (memoria respaldada por batería) del cartucho para guardar partidas en Mega Drive: cómo declararla en la cabecera de la ROM, cómo bloquearla/desbloquearla para leer y escribir, y las precauciones necesarias para no corromper los datos guardados.

## Detalle técnico

### Configuración de la cabecera de la ROM

Para habilitar el soporte de SRAM, hay que modificar la cabecera de la ROM con estos 12 bytes:

```
dc.b 'RA',$F8,$20    ; SRAM type
dc.l $200001         ; SRAM start address
dc.l $20FFFF         ; SRAM end address
```

### Direcciones de memoria y constantes

La documentación especifica estas ubicaciones clave:

- **Rango de SRAM:** `$200001` a `$20FFFF` (32KB en total)
- **Registro de bloqueo de SRAM:** `$A130F1`
- **Nota de direccionamiento:** solo son accesibles bytes alternos dentro de la SRAM

### Procedimiento de escritura

El proceso requiere tres pasos secuenciales:

1. Escribir el valor 1 en el registro de bloqueo de SRAM
2. Transferir los datos a las direcciones de SRAM
3. Escribir el valor 0 para volver a bloquear la SRAM

Código de ejemplo que demuestra el patrón:

```
move.b  #1, (SramLock)   ; Unlock SRAM

lea     (DataPtr), a0    ; Beginning of data
lea     (SramStart), a1  ; Beginning of SRAM
move.w  #DataSize-1, d0  ; Number of bytes
Loop:
move.b  (a0), (a1)       ; Write byte to SRAM
addq.l  #1, a0           ; Advance data byte
addq.l  #2, a1           ; Advance SRAM byte
dbf     d0, Loop         ; Keep going

move.b  #0, (SramLock)   ; Lock SRAM
```

### Procedimiento de lectura

"Leer los datos de vuelta es similar a escribirlos", con los mismos pasos de bloqueo/desbloqueo alrededor de las operaciones de acceso a datos.

## Notas / gotchas

- Hay que inicializar comprobaciones de validación de la SRAM en el primer arranque (los datos iniciales son basura).
- Implementar checksums para verificar la integridad de los datos frente a corrupción por pérdida de energía.
- Contemplar escenarios de fallo de la batería dentro de los sistemas de juego.
- **Advertencia adicional:** Sega advirtió contra el uso de las direcciones límite `$200001` y `$20FFFF` debido a un riesgo elevado de pérdida de datos.
- **Consideración para ROMs grandes:** para juegos de más de 2MB, el mecanismo de bloqueo de SRAM cambia la visibilidad de la SRAM hacia el espacio de direcciones superior del cartucho. El código que accede a la SRAM debe residir en la región inferior de 2MB o ejecutarse desde RAM.

## Referencias cruzadas

- (pendiente de enlazar)

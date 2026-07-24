---
titulo: Padding de la ROM
plataforma: [MegaDrive]
categoria: [memoria]
tags: [rom, padding, romfix, checksum, mdtools]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/rom-padding]
---

# Padding de la ROM

## Resumen

El binario compilado del juego necesita rellenarse (padding) hasta igualar el tamaño real de la memoria del cartucho (por ejemplo, un chip Flash de 4MB). Sin el padding correcto, los emuladores pueden fallar al reconocer la ROM o mostrar basura en el título aunque la cabecera sea correcta.

## Detalle técnico

Existen tres métodos para hacer el padding de una ROM:

### Método 1: utilidad `romfix`

Simplemente ejecutando la ROM a través de ella "se rellenará hasta el siguiente tamaño que considere seguro", vía el paquete mdtools. El comando básico es:

```
romfix filename.bin
```

El autor advierte que "romfix también hace otras cosas (como arreglar el checksum de la ROM)", por lo que conviene verificar todos sus efectos. La flag `-z` muestra el tamaño del archivo antes/después.

### Método 2: directiva del ensamblador

Usando la directiva `dcb` para el padding:

```
dcb.b   $400000-*, 0
```

Esto rellena hasta 4MB con ceros. Sin embargo, la página señala que los ensambladores tipo asmx pueden no incluir el padding final automáticamente, lo que requiere una solución alternativa:

```
dcb.b   $3FFFFE-*, 0
dc.w    $0000
```

Esto rellena hasta dos bytes antes de los 4MB, y luego añade manualmente esos últimos bytes.

### Método 3: herramientas de línea de comandos Unix

Para sistemas Linux/Unix:

```
cat original.bin /dev/zero | dd bs=1024 count=4096 of=padded.bin
```

Esto combina las utilidades `cat` y `dd` para crear un archivo de 4MB con padding.

## Notas / gotchas

- Advertencia clave del autor: "Si se niega a ejecutar el juego aparentemente sin motivo y muestra basura como título del juego a pesar de que la cabecera de la ROM es correcta en el archivo, esto es probablemente lo que está pasando" — indicando que un padding incorrecto provoca una mala identificación del formato por parte de emuladores/flashcarts.
- `romfix` también corrige el checksum de la ROM además de hacer el padding, así que hay que tener en cuenta ese efecto colateral.

## Referencias cruzadas

- (pendiente de enlazar)

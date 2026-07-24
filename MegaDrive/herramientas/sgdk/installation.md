---
titulo: Instalación de SGDK
plataforma: [MegaDrive]
categoria: [herramientas]
tags: [sgdk, instalacion, java, gcc, windows]
estado: borrador
fecha: 2026-07-21
fuentes: [https://github.com/Stephane-D/SGDK/wiki/SGDK-Installation]
---

# Instalación de SGDK

## Resumen

Pasos para instalar SGDK y verificar que la librería compila correctamente. La guía oficial de la wiki cubre **solo Windows**; para otros sistemas operativos remite al README del repositorio.

## Detalle técnico

### Paso 1 — Instalar Java

La herramienta de compilación de recursos (`rescomp`) requiere Java. Versión mínima: Java 8 (64-bit para Windows), disponible en https://www.java.com/en/download/.

### Paso 2 — Descargar y extraer SGDK

Descargar el archivo más reciente desde la página de releases: https://github.com/Stephane-D/SGDK/releases, y extraerlo en la ubicación preferida (por ejemplo, `D:\sgdk`). Esa ruta se referencia como `<SGDK_PATH>` en el resto de instrucciones.

### Paso 3 — Verificar la instalación

Abrir una terminal (cmd) y ejecutar:

```
<SGDK_PATH>\bin\make -f <SGDK_PATH>\makelib.gen
```

(sustituyendo `<SGDK_PATH>` por la ruta real de instalación). Debería verse el log de compilación; al terminar con éxito debe existir el archivo `<SGDK_PATH>\lib\libmd.a`.

## Notas / gotchas

- La guía de la wiki es específica de Windows; en otros SO consultar el README del repositorio para instrucciones equivalentes (no cubierto en esta entrada).
- Tras verificar la instalación, el siguiente paso es consultar [Uso de SGDK](usage.md) para estructurar y compilar un proyecto real.

## Referencias cruzadas

- [SGDK — overview](overview.md)
- [Uso de SGDK (estructura de proyecto y build)](usage.md)

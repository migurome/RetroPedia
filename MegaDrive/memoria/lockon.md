---
titulo: Cartucho Lock-On (Sonic & Knuckles)
plataforma: [MegaDrive]
categoria: [memoria]
tags: [lock-on, sonic-knuckles, mapeo-memoria, a130f1, cartucho]
estado: borrador
fecha: 2026-07-21
fuentes: [https://plutiedev.com/lockon]
---

# Cartucho Lock-On (Sonic & Knuckles)

## Resumen

Describe el mecanismo de "Lock-On" del cartucho de Sonic & Knuckles, que permitía conectar otros cartuchos encima creando experiencias de juego combinadas o modos especiales, mediante un mapeo de memoria conmutado por el registro `$A130F1`.

## Detalle técnico

### Arquitectura de mapeo de memoria

**Configuración estándar de Lock-On (bit 0 de `$A130F1` = 0):**

- `$000000-$1FFFFF`: ROM principal de Sonic & Knuckles
- `$200000-$2FFFFF`: región superior del cartucho conectado (locked cartridge)
- `$300000-$3FFFFF`: región superior del cartucho conectado (reflejada/mirrored)

**Con la ROM de parche activa (bit 0 de `$A130F1` = 1):**

- `$000000-$1FFFFF`: ROM principal de Sonic & Knuckles
- `$200000-$2FFFFF`: cartucho conectado
- `$300000-$3FFFFF`: "ROM de parche de 256KB (reflejada para llenar el espacio)"

### Detalles técnicos clave

**Comportamiento de Sonic 3 & Knuckles:** el cartucho explota la capacidad de mirroring de ROM de Sonic 3 cuando la SRAM no está mapeada. Los 2MB superiores del cartucho conectado se vuelven visibles, permitiendo que el registro `$A130F1` alterne entre acceso a ROM y a la RAM de guardado.

**Implementación de Sonic 2:** requirió una ROM de parche separada de 256KB porque "el motor y algunos gráficos necesitan ser reemplazados, pero no hay suficiente espacio para eso en la ROM principal de Sonic & Knuckles". La ROM de parche se mapea en la región de 1MB superior cuando se conmuta el registro.

**Limitación de compatibilidad:** los juegos de más de 2MB fallan en la detección porque "Sonic & Knuckles busca una cabecera de ROM válida para ver si hay un cartucho presente". Sin mirroring de ROM en cartuchos más grandes, no aparece ninguna cabecera en la ubicación superior de 2MB esperada.

**Excepciones notables:** Tanglewood y Sonic Classics colocaron intencionadamente cabeceras de ROM falsas en la ubicación esperada para activar la funcionalidad de Get Blue Spheres.

## Notas / gotchas

- El bit 0 de `$A130F1` es el que conmuta entre el modo estándar de Lock-On y la ROM de parche.
- Cartuchos mayores de 2MB no son detectados por Sonic & Knuckles al no poder reflejar su cabecera en la posición esperada, salvo que se coloque deliberadamente una cabecera falsa (como hicieron Tanglewood y Sonic Classics).

## Referencias cruzadas

- (pendiente de enlazar)

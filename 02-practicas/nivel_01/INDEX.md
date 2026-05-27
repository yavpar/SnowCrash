# Nivel 01 - Intermedio: Explotación Práctica

## Visión General

En Nivel 00 aprendiste los fundamentos. En Nivel 01 **aplicas técnicas reales** de explotación de vulnerabilidades.

- **Dificultad:** ⭐⭐ Intermedio
- **Tiempo estimado:** 8-10 horas
- **Requisito previo:** Completar Nivel 00
- **Herramientas:** GDB, objdump, gcc, Python

---

## Estructura de Ejercicios

### Ejercicio 01: Debugging Básico con GDB
**Tema:** Usar GDB para detectar buffer overflow
- Cómo usar breakpoints
- Inspeccionar variables en runtime
- Ver memory crudamente
- Caso práctico: detectar strcpy overflow

**Tiempo:** 45-60 minutos
**Archivo:** `ejercicio_01.md`

### Ejercicio 02: Disassembly Práctico con Objdump
**Tema:** Leer código compilado a assembly
- Desensamblar funciones reales
- Leer assembly x86-64
- Relación C ↔ Assembly
- Comparar -O0 vs -O2

**Tiempo:** 60-75 minutos
**Archivo:** `ejercicio_02.md`

### Ejercicio 03: Detectar Vulnerabilidades
**Tema:** Análisis estático de código inseguro
- Funciones peligrosas (strcpy, gets, sprintf)
- Análisis manual de tamaños
- Uso de herramientas (grep, cppcheck)
- Proponer fixes

**Tiempo:** 45-60 minutos
**Archivo:** `ejercicio_03.md`

### Ejercicio 04: Buffer Overflow - Exploit
**Tema:** Explotar un buffer overflow real
- Encontrar offset a RIP
- Controlar instruction pointer
- Saltar a función arbitraria
- Construir payload

**Tiempo:** 75-90 minutos
**Archivo:** `ejercicio_04.md`

### Ejercicio 05: Format String Vulnerabilities
**Tema:** Explotar printf/sprintf inseguro
- Leer memoria con %x, %s
- Encontrar offset en stack
- Escribir memoria con %n
- Información leak

**Tiempo:** 90-120 minutos
**Archivo:** `ejercicio_05.md`

### Ejercicio 06: ROP Gadgets Basics
**Tema:** Introducción a Return-Oriented Programming
- Por qué ROP (NX bit)
- Buscar gadgets útiles
- Encadenar gadgets
- Preparación para ROP chains

**Tiempo:** 75-90 minutos
**Archivo:** `ejercicio_06.md`

---

## Progresión Recomendada

```
01 → 02 → 03 → 04 → 05 → 06
 ↓   ↓    ↓    ↓    ↓    ↓
GDB ObjD Ana   BO   FS   ROP

Cada ejercicio requiere conocimientos del anterior
```

- **01-03:** Entender el problema
- **04-05:** Explotar el problema
- **06:** Alternativa cuando hay defenses

---

## Habilidades por Ejercicio

### Ejercicio 01: GDB Mastery
Después de completar:
- [ ] Puedo usar breakpoints estratégicamente
- [ ] Inspecciono variables sin compilar prints
- [ ] Veo stack layout en memoria
- [ ] Encuentro dónde se corrompe memoria

### Ejercicio 02: Assembly Reading
Después de completar:
- [ ] Leo assembly sin referencia
- [ ] Entiendo calling conventions
- [ ] Reconozco patrones comunes
- [ ] Relacioo C con machine code

### Ejercicio 03: Vulnerability Analysis
Después de completar:
- [ ] Identif fico funciones peligrosas
- [ ] Calculo overflows teóricamente
- [ ] Propongo alternativas seguras
- [ ] Automatizo búsquedas

### Ejercicio 04: Exploit Development
Después de completar:
- [ ] Construyo payloads funcionales
- [ ] Controlo RIP de manera confiable
- [ ] Entiendo qué hace posible explotar
- [ ] Debuggeo exploits en GDB

### Ejercicio 05: Format String Abuse
Después de completar:
- [ ] Ejecuto información leaks
- [ ] Escribo en memoria arbitraria
- [ ] Encuentro offsets automáticamente
- [ ] Combino con otros exploits

### Ejercicio 06: ROP Foundation
Después de completar:
- [ ] Busco gadgets útiles
- [ ] Construyo simple ROP chains
- [ ] Entiendo limitaciones de NX bit
- [ ] Estoy listo para proyectos avanzados

---

## Dependencias Entre Ejercicios

```
Ejercicio 01 (GDB)
    ↓ Necesario para debuggear
Ejercicio 02 (Objdump)
    ↓ Entender qué explotar
Ejercicio 03 (Análisis)
    ↓ Encontrar vulnerabilidades
Ejercicio 04 (Exploit)
    ↓ Primer exploit real
    ↓
Ejercicio 05 (Format String)  ← Paralelo 04, diferente vulnerabilidad
Ejercicio 06 (ROP)            ← Paralelo 04-05, diferentes defenses
```

---

## Herramientas Necesarias

Todas instaladas en Fedora por defecto:

| Herramienta | Ejercicios | Propósito |
|-------------|-----------|----------|
| GDB | 01, 04, 05, 06 | Debugging |
| GCC | Todos | Compilar |
| objdump | 02, 03, 06 | Disassembly |
| Python3 | 04, 05, 06 | Payloads |
| strings | 03 | Análisis estático |
| grep | 03, 06 | Búsqueda de patrones |

Opcional pero útil:
- `cppcheck` - Análisis estático
- `ROPgadget` - Búsqueda automática gadgets
- `Ropper` - Alternativa a ROPgadget

---

## Cómo Usar Esta Carpeta

### Comenzar un ejercicio

1. Lee `ejercicio_0X.md` completamente
2. Crea el código en tu editor
3. Compila con flags especificados
4. Intenta solucionar el problema
5. Usa GDB si te atascas
6. Verifica los checkpoints

### Recursos útiles

- `recursos/gdb-guia.md` - Referencia completa de GDB
- `recursos/objdump-guia.md` - Referencia de assembly
- `recursos/glosario.md` - Términos desconocidos
- `recursos/herramientas.md` - Flags de compilación

### Desafíos Extra

Cada ejercicio tiene "Desafío Extra" con 3 niveles:
- Nivel 1: Fácil, automatizar algo
- Nivel 2: Medio, combinar técnicas
- Nivel 3: Difícil, concepto avanzado

No es requisito completarlos, pero recomendado si tienes tiempo.

---

## Errores Comunes

### Error: "-fno-stack-protector: not recognized"
**Solución:** Instalar herramientas de desarrollo
```bash
sudo dnf install gcc gdb
```

### Error: "Segmentation fault (core dumped)"
**Solución:** Es esperado cuando hay overflow. Usa GDB:
```bash
gdb ./programa
(gdb) run "entrada_larga"
(gdb) backtrace
```

### Error: "ROP chain no funciona"
**Solución:** Verifica offsets en GDB, asegúrate que gadgets existen

### Error: "Format string no interpreta %x"
**Solución:** Debe ser `printf(usuario)` no `printf("%s", usuario)`

---

## Checklist de Progreso

### Inicio de Nivel 01
- [ ] Completé Nivel 00
- [ ] Tengo GDB, GCC, Python3 funcionando
- [ ] Entiendo conceptos de Nivel 00

### Ejercicio 01
- [ ] Compilé misterio.c con -g -O0
- [ ] Usé GDB para ver buffer overflow
- [ ] Entendí offset de variables

### Ejercicio 02
- [ ] Desensambblé suma.c
- [ ] Leí assembly paso a paso
- [ ] Comparé -O0 vs -O2

### Ejercicio 03
- [ ] Identif iqué funciones peligrosas
- [ ] Creé versión segura
- [ ] Entendí qué hace cada fix

### Ejercicio 04
- [ ] Compilé objetivo.c sin protecciones
- [ ] Encontré offset a RIP
- [ ] Ejecuté exploit exitosamente

### Ejercicio 05
- [ ] Encontré formato string vulnerable
- [ ] Leí valores del stack
- [ ] Entendí por qué %n es peligroso

### Ejercicio 06
- [ ] Busqué gadgets en binario
- [ ] Entendí por qué ROP es necesario
- [ ] Construí ROP chain teórico

### Fin de Nivel 01
- [ ] Completé todos 6 ejercicios
- [ ] Hice al menos 3 desafíos extra
- [ ] Entiendo técnicas de exploitación
- [ ] Puedo debuggear mis exploits

---

## Siguiente Nivel

**Nivel Bonus:** Desafíos integrados
- Combine buffer overflow + ROP
- Combine format strings + leaks
- Exploit binario sin fuente

Después:
- **Proyecto siguiente:** Explotación avanzada con ASLR
- **Nuevo proyecto:** ROP chains automáticas

---

## Tiempo Total Estimado

```
Ejercicio 01:  45-60 min   (45 min)
Ejercicio 02:  60-75 min   (105 min = 1:45)
Ejercicio 03:  45-60 min   (150 min = 2:30)
Ejercicio 04:  75-90 min   (225 min = 3:45)
Ejercicio 05:  90-120 min  (315 min = 5:15)
Ejercicio 06:  75-90 min   (390 min = 6:30)
Desafíos:      2-3 horas   (480 min = 8:00)
────────────────────────────────────
TOTAL:         8-10 horas
```

---

## Notas Finales

Estos ejercicios son **prácticos y pragmáticos**. No es teoría pura.

Cada ejercicio:
1. Plantea un problema real
2. Enseña la herramienta necesaria
3. Te hace explotar

No memori ces. **Entiende.**

Cuando termines Nivel 01:
- Podrás explotar buffer overflow reales
- Sabrás usar GDB como experto
- Entenderás código compilado
- Estarás listo para ROP avanzado

¡Éxito!

---

Última actualización: Mayo 25, 2026

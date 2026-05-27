# 🎯 Prácticas - Ejercicios Progresivos

Este es el índice de todos los ejercicios organizados por nivel de dificultad.

**Filosofía:** Cada ejercicio es pequeño, enfocado, y construye sobre conocimientos previos.

---

## 🟢 Nivel 00 - Fundamentos (Muy Fácil)

**Requisitos:** Haber leído Lecciones 01-03  
**Objetivo:** Familiarizarse con las herramientas básicas y conceptos fundamentales

### Ejercicio 01: Mi Primer Binario
**Tema:** Compilar y ejecutar un programa simple  
**Tiempo:** 15 minutos  
**Conceptos:** gcc, ejecución de programas

📂 [Ver Ejercicio](nivel_00/ejercicio_01.md)

---

### Ejercicio 02: Explorando con `file` y `strings`
**Tema:** Analizar un binario sin ejecutarlo  
**Tiempo:** 20 minutos  
**Conceptos:** Herramientas de análisis básicas, información de binarios

📂 [Ver Ejercicio](nivel_00/ejercicio_02.md)

---

### Ejercicio 03: Entendiendo la Memoria
**Tema:** Ver cómo funciona la memoria de un programa  
**Tiempo:** 25 minutos  
**Conceptos:** Stack, variables, direcciones de memoria

📂 [Ver Ejercicio](nivel_00/ejercicio_03.md)

---

### Ejercicio 04: Mi Primer Puntero
**Tema:** Trabajar con punteros en C  
**Tiempo:** 30 minutos  
**Conceptos:** Punteros, referencias, dereferenciación

📂 [Ver Ejercicio](nivel_00/ejercicio_04.md)

---

## 🟡 Nivel 01 - Intermedio (Fácil)

**Requisitos:** Completados Nivel 00 + Lecciones 04-05  
**Objetivo:** Usar herramientas de análisis avanzadas y reconocer patrones

### Ejercicio 01: GDB Básico
**Tema:** Comenzar con el debugger  
**Tiempo:** 30 minutos  
**Conceptos:** Breakpoints, stepping, inspeccionar variables

📂 [Ver Ejercicio](nivel_01/ejercicio_01.md)

---

### Ejercicio 02: Disassembly con Objdump
**Tema:** Leer código ensamblador  
**Tiempo:** 40 minutos  
**Conceptos:** Assembly x86-64, reconocer instrucciones, flujo de control

📂 [Ver Ejercicio](nivel_01/ejercicio_02.md)

---

### Ejercicio 03: Tracing Syscalls
**Tema:** Ver qué hace un programa a bajo nivel  
**Tiempo:** 25 minutos  
**Conceptos:** strace, syscalls, comportamiento del sistema

📂 [Ver Ejercicio](nivel_01/ejercicio_03.md)

---

### Ejercicio 04: Primeros Signos de Vulnerabilidad
**Tema:** Identificar código vulnerable  
**Tiempo:** 35 minutos  
**Conceptos:** Code review, patrones peligrosos, vulnerabilidades comunes

📂 [Ver Ejercicio](nivel_01/ejercicio_04.md)

---

### Ejercicio 05: Buffer Overflow - Teoría a Práctica
**Tema:** Causar un crash deliberadamente  
**Tiempo:** 45 minutos  
**Conceptos:** Buffer overflow, stack corruption, control de ejecución

🔮 **Hint Futuro:** Dominar esto es CRÍTICO para todo lo que viene después.

📂 [Ver Ejercicio](nivel_01/ejercicio_05.md)

---

### Ejercicio 06: Format String Basics
**Tema:** Leer memoria con format strings  
**Tiempo:** 40 minutos  
**Conceptos:** Format strings, printf, lectura de stack

📂 [Ver Ejercicio](nivel_01/ejercicio_06.md)

---

## 🔴 Bonus - Desafíos Avanzados

**Requisitos:** Completados Nivel 01 + todas las Lecciones  
**Objetivo:** Aplicar todo lo aprendido en desafíos complejos

### Desafío 01: Binario Misterio
**Tema:** Analizar y explotar un binario desconocido  
**Tiempo:** 1.5-2 horas  
**Conceptos:** Síntesis de todos los conceptos previos

📂 [Ver Desafío](bonus/desafio_01.md)

---

### Desafío 02: Protecciones Modernas
**Tema:** Evadir protecciones (ASLR, NX, Stack Canaries)  
**Tiempo:** 2-3 horas  
**Conceptos:** Seguridad defensiva, bypass de protecciones

🔮 **Hint Futuro:** Las protecciones modernas son centrales en proyectos posteriores.

📂 [Ver Desafío](bonus/desafio_02.md)

---

### Desafío 03: Cadena de Exploración
**Tema:** Combinar múltiples técnicas  
**Tiempo:** 2-3 horas  
**Conceptos:** ROP chains, gadgets, explotación compleја

📂 [Ver Desafío](bonus/desafio_03.md)

---

## 📈 Progresión Recomendada

```
Nivel 00 (4 ejercicios) → ~1.5 horas
         ↓
    Lecciones 04-05
         ↓
Nivel 01 (6 ejercicios) → ~3 horas
         ↓
    Lecciones 06-08
         ↓
Bonus (3 desafíos) → ~6 horas
```

**Total:** ~10.5 horas de práctica + 7.5 horas de teoría = **~18 horas**

---

## 🎯 Estructura de Cada Ejercicio

Cada carpeta de ejercicio contiene:

```
ejercicio_XX/
├── ejercicio_XX.md      (instrucciones y objetivos de aprendizaje)
├── solucion_XX.sh       (script con la solución)
├── solucion_XX.c        (código fuente C si aplica)
├── notas.md             (notas adicionales y pistas)
└── README.md            (quickstart)
```

---

## 💡 Cómo Usar los Ejercicios

### Para Cada Ejercicio:

1. **Lee el archivo `.md`** - Entiende qué se pide
2. **Intenta resolver** - Usa lo que aprendiste en teoría
3. **Si estás atrapado** - Lee `notas.md` para pistas
4. **Compara con solución** - Revisa `solucion_*.sh` o `solucion_*.c`
5. **Experimenta** - Modifica la solución y aprende

---

## 🆘 Si Estás Atrapado

1. **Revisa la lección asociada** - Vuelve a leer la teoría
2. **Consulta `notas.md`** - Tiene pistas y explicaciones
3. **Busca en `recursos/`** - Guías de herramientas
4. **Experimenta en la terminal** - Prueba comandos pequeños

**No hagas:**
- ❌ Copiar la solución sin entender
- ❌ Saltarte un ejercicio porque es "demasiado fácil"
- ❌ Continuar si no entiendes un concepto

**Sí haz:**
- ✅ Tomar tiempo para entender cada paso
- ✅ Documentar lo que aprendas
- ✅ Compartir aprendizajes (en tu Github, por ejemplo)

---

## 📊 Tabla Rápida de Ejercicios

| Nivel | Ejercicio | Tema | Tiempo | Pre-requisitos |
|-------|-----------|------|--------|-----------------|
| 00 | 01 | Compilar | 15 min | - |
| 00 | 02 | file/strings | 20 min | Ej 01 |
| 00 | 03 | Memoria | 25 min | Ej 02 |
| 00 | 04 | Punteros | 30 min | Ej 03 |
| 01 | 01 | GDB | 30 min | Nivel 00 |
| 01 | 02 | Objdump | 40 min | Ej 01 Nivel 01 |
| 01 | 03 | strace | 25 min | Ej 02 Nivel 01 |
| 01 | 04 | Vulnerabilidades | 35 min | Ej 03 Nivel 01 |
| 01 | 05 | Buffer Overflow | 45 min | Ej 04 Nivel 01 |
| 01 | 06 | Format Strings | 40 min | Ej 05 Nivel 01 |
| Bonus | 01 | Binario Misterio | 1.5-2h | Nivel 01 |
| Bonus | 02 | Protecciones | 2-3h | Desafío 01 |
| Bonus | 03 | Cadena Explotación | 2-3h | Desafío 02 |

---

## 🔗 Navegación

- ← [Volver a README.md](../README.md)
- → [Ir a Nivel 00 - Ejercicio 01](nivel_00/ejercicio_01.md)
- 📚 [Ver Teoría](../01-teoria/INDEX.md)

---

**Última actualización:** Mayo 2026

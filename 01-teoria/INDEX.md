# 📚 Teoría - Índice de Lecciones

Aquí están todas las lecciones de teoría organizadas en orden progresivo.

**Importante:** Lee las lecciones en orden. Cada una construye sobre conceptos de la anterior.

---

## 🏛️ Fundaciones (Nivel 0)

Estos temas estableen la base para todo lo demás.

### Lección 01: Introducción a la Seguridad y los Conceptos Básicos
**Contenido:**
- ¿Qué es la seguridad informática?
- ¿Por qué estudiamos explotación?
- Conceptos: vulnerabilidades, exploits, payloads
- ¿Por qué ft_snowcrash es importante?

**Tiempo estimado:** 30 minutos  
**Ejercicios asociados:** [02-practicas/nivel_00/](../02-practicas/nivel_00/)  
**Pre-requisitos:** Ninguno

📄 [Leer Lección 01](leccion_01.md)

---

### Lección 02: Binarios, Archivos ELF y Memoria
**Contenido:**
- ¿Qué es un binario?
- Estructura de archivos ELF en Linux
- Cómo funciona la memoria en Linux
- Stack, heap, y segmentos
- Direcciones de memoria y punteros

**Tiempo estimado:** 45 minutos  
**Ejercicios asociados:** [02-practicas/nivel_00/](../02-practicas/nivel_00/)  
**Pre-requisitos:** Lección 01

📄 [Leer Lección 02](leccion_02.md) (próximamente)

---

### Lección 03: Lenguaje C Fundamentals para Seguridad
**Contenido:**
- Variables y tipos de datos
- Punteros (concepto crítico)
- Arrays y buffers
- Funciones y stack frames
- Cómo se ejecuta el código en la CPU

**Tiempo estimado:** 1 hora  
**Ejercicios asociados:** [02-practicas/nivel_00/](../02-practicas/nivel_00/)  
**Pre-requisitos:** Lecciones 01-02

📄 [Leer Lección 03](leccion_03.md) (próximamente)

---

## 🔧 Herramientas (Nivel 1)

Ahora aprendemos a usar las herramientas para analizar binarios.

### Lección 04: GDB - El Debugger
**Contenido:**
- Qué es un debugger y por qué es crucial
- Comandos básicos de GDB
- Breakpoints y stepping
- Inspeccionar memoria y registros
- Rastrear ejecución de programas

**Tiempo estimado:** 1 hora  
**Ejercicios asociados:** [02-practicas/nivel_01/](../02-practicas/nivel_01/)  
**Pre-requisitos:** Lecciones 01-03

📄 [Leer Lección 04](leccion_04.md) (próximamente)

---

### Lección 05: Objdump y Análisis de Binarios
**Contenido:**
- Desensamblar código con objdump
- Leer assembly language (x86-64)
- Identificar funciones y saltos
- Análisis estático vs dinámico
- Reconocer patrones de vulnerabilidades

**Tiempo estimado:** 1 hora  
**Ejercicios asociados:** [02-practicas/nivel_01/](../02-practicas/nivel_01/)  
**Pre-requisitos:** Lecciones 01-04

📄 [Leer Lección 05](leccion_05.md) (próximamente)

---

## 🎯 Vulnerabilidades (Nivel 2)

Identificamos y entendemos vulnerabilidades reales.

### Lección 06: Buffer Overflows
**Contenido:**
- Qué es un buffer overflow
- Cómo ocurren en la memoria
- Detectarlos con herramientas
- Primeros intentos de explotación
- Protecciones del sistema contra overflows

**Tiempo estimado:** 1.5 horas  
**Ejercicios asociados:** [02-practicas/nivel_01/](../02-practicas/nivel_01/)  
**Pre-requisitos:** Lecciones 01-05

🔮 **Hint Futuro:** Este concepto es FUNDAMENTAL para proyectos posteriores de explotación binaria.

📄 [Leer Lección 06](leccion_06.md) (próximamente)

---

### Lección 07: Format Strings
**Contenido:**
- Vulnerabilidades de format strings
- Cómo se explotan
- Lectura de memoria
- Escritura de memoria
- Defensa contra format strings

**Tiempo estimado:** 1 hora  
**Ejercicios asociados:** [02-practicas/nivel_01/](../02-practicas/nivel_01/)  
**Pre-requisitos:** Lecciones 01-06

📄 [Leer Lección 07](leccion_07.md) (próximamente)

---

## 🏆 Síntesis (Nivel 3)

Combinamos todo lo aprendido.

### Lección 08: Pensamiento de Seguridad y Análisis
**Contenido:**
- Cómo abordar un binario desconocido
- Metodología de análisis
- Reconocer patrones vulnerables
- Planificar explotación
- Documentar hallazgos

**Tiempo estimado:** 1 hora  
**Ejercicios asociados:** [02-practicas/bonus/](../02-practicas/bonus/)  
**Pre-requisitos:** Todas las lecciones anteriores

📄 [Leer Lección 08](leccion_08.md) (próximamente)

---

## 🎓 Resumen Visual

```
Lección 01
    ↓
Lección 02
    ↓
Lección 03 (C Basics)
    ↓
Lección 04 (GDB)
    ↓
Lección 05 (Objdump)
    ↓
Lección 06 (Buffer Overflow) ← CRÍTICA
    ↓
Lección 07 (Format Strings)
    ↓
Lección 08 (Síntesis)
```

---

## 📊 Tiempo Total Estimado

- **Nivel 0** (Lecciones 01-03): ~2 horas
- **Nivel 1** (Lecciones 04-05): ~2 horas
- **Nivel 2** (Lecciones 06-07): ~2.5 horas
- **Nivel 3** (Lección 08): ~1 hora

**Total:** ~7.5 horas de teoría (sin contar prácticas)

---

## 🔗 Navegación

- ← [Volver a README.md](../README.md)
- → [Ir a Lección 01](leccion_01.md)
- 📂 [Ir a Prácticas](../02-practicas/)

---

**Última actualización:** Mayo 2026

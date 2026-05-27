# 🏗️ ESTRUCTURA DEL PROYECTO - Guía Visual

Este documento te muestra exactamente qué archivos se han creado y qué encontrarás en cada uno.

---

## 📂 Árbol de Carpetas

```
SnowCrash/
│
├── 📄 CLAUDE.md                    ← Configuración para Claude Code
├── 📄 README.md                    ← Página de inicio del curso
├── 📄 SETUP.md                     ← Instalación en Fedora
├── 📄 ESTRUCTURA-PROYECTO.md       ← Este archivo (guía visual)
│
├── 📁 01-teoria/                   ← Conceptos teóricos
│   └── INDEX.md                    ← Índice de todas las lecciones
│
├── 📁 02-practicas/                ← Ejercicios prácticos
│   ├── INDEX.md                    ← Índice de ejercicios
│   ├── 📁 nivel_00/                ← Ejercicios básicos (4 ejercicios)
│   ├── 📁 nivel_01/                ← Ejercicios intermedios (6 ejercicios)
│   └── 📁 bonus/                   ← Desafíos avanzados (3 desafíos)
│
└── 📁 recursos/                    ← Materiales de apoyo
    └── README.md                   ← Índice de recursos
```

---

## 📖 Descripción de Cada Archivo

### **En la raíz del proyecto:**

#### `README.md`
- **Qué es:** La página de inicio del curso
- **Cuándo leerlo:** PRIMERO, cuando empieces
- **Contiene:**
  - Bienvenida y descripción general
  - Estructura del curso
  - Cómo usar el material
  - Objetivos de aprendizaje
  - FAQ básicas

#### `SETUP.md`
- **Qué es:** Guía de instalación para Fedora
- **Cuándo leerlo:** Antes de comenzar el curso
- **Contiene:**
  - Pasos para instalar herramientas
  - Verificación de instalación
  - Solución de problemas (troubleshooting)
  - Scripts automáticos

#### `CLAUDE.md`
- **Qué es:** Instrucciones para futuros desarrolladores
- **Cuándo leerlo:** No necesitas leerlo ahora (es para Claude Code)
- **Contiene:**
  - Estructura técnica del proyecto
  - Filosofía de enseñanza
  - Cómo agregar nuevo contenido

#### `ESTRUCTURA-PROYECTO.md`
- **Qué es:** Este archivo - guía visual
- **Cuándo leerlo:** Para entender la navegación
- **Contiene:**
  - Árbol de carpetas
  - Descripción de archivos
  - Ruta recomendada de aprendizaje

---

### **En la carpeta `01-teoria/`:**

#### `INDEX.md`
- **Qué es:** Índice de todas las lecciones teóricas
- **Cuándo leerlo:** Cuando quieras ver qué temas hay
- **Contiene:**
  - Listado de 8 lecciones ordenadas
  - Descripción de cada lección
  - Tiempo estimado
  - Pre-requisitos
  - Enlaces a las lecciones

**Lecciones disponibles (será completadas progresivamente):**
1. Introducción a la Seguridad
2. Binarios y Memoria
3. C Fundamentals
4. GDB (Debugger)
5. Objdump y Assembly
6. Buffer Overflows
7. Format Strings
8. Análisis y Síntesis

---

### **En la carpeta `02-practicas/`:**

#### `INDEX.md`
- **Qué es:** Índice de todos los ejercicios prácticos
- **Cuándo leerlo:** Para ver qué ejercicios hay
- **Contiene:**
  - 13 ejercicios totales (4 básicos + 6 intermedios + 3 bonus)
  - Descripción de cada uno
  - Tiempo estimado
  - Tabla rápida de referencia

**Estructura por nivel:**
- **Nivel 00** (4 ejercicios): Mi primer binario, file/strings, memoria, punteros
- **Nivel 01** (6 ejercicios): GDB, objdump, strace, vulnerabilidades, buffer overflow, format strings
- **Bonus** (3 desafíos): Binario misterio, protecciones, cadena de explotación

---

### **En la carpeta `recursos/`:**

#### `README.md`
- **Qué es:** Índice de materiales de apoyo
- **Cuándo leerlo:** Cuando necesites consultar referencias
- **Contiene:**
  - Guías de herramientas (próximamente)
  - Glosario de términos (próximamente)
  - Referencias y enlaces (próximamente)
  - Scripts auxiliares

---

## 🚀 Ruta Recomendada de Aprendizaje

### **Semana 1: Configuración y Fundamentos**

```
Día 1:
├─ Lee README.md (15 min)
├─ Lee SETUP.md (30 min)
└─ Ejecuta los scripts de instalación (30 min)

Día 2-3:
├─ Lee 01-teoria/INDEX.md (10 min)
├─ Abre leccion_01.md (cuando esté disponible)
└─ Haz ejercicios Nivel 00 (2-3 horas)
```

### **Semana 2-3: Herramientas**

```
├─ Lee leccion_02.md y leccion_03.md
├─ Lee leccion_04.md (GDB)
├─ Lee leccion_05.md (Objdump)
└─ Haz ejercicios Nivel 01 (4-5 horas)
```

### **Semana 4: Vulnerabilidades**

```
├─ Lee leccion_06.md (Buffer Overflow)
├─ Lee leccion_07.md (Format Strings)
├─ Profundiza con ejercicios Nivel 01
└─ Intenta desafíos Bonus (6-8 horas)
```

### **Semana 5: Síntesis**

```
├─ Lee leccion_08.md (Análisis)
├─ Completa todos los ejercicios pendientes
└─ Documenta lo aprendido en tu GitHub
```

---

## 📍 Cómo Navegar el Proyecto

### Si estás en `README.md`:
- Quiero aprender → Ve a `01-teoria/INDEX.md`
- Necesito instalar → Ve a `SETUP.md`
- Quiero hacer ejercicios → Ve a `02-practicas/INDEX.md`

### Si estás en `01-teoria/INDEX.md`:
- Quiero la lección 1 → Click en enlace de leccion_01.md
- Quiero los ejercicios → Ve a `02-practicas/nivel_00/`
- Quiero volver → Click en README.md

### Si estás en `02-practicas/INDEX.md`:
- Quiero un ejercicio específico → Click en el enlace
- Necesito la teoría → Ve a `01-teoria/INDEX.md`
- Necesito herramientas → Ve a `recursos/README.md`

---

## 💾 Archivos Que Se Crearán Mientras Avanzas

A medida que el curso progresa, se crearán estos archivos:

### Teoría (próximamente):
```
01-teoria/
├── leccion_01.md     ✅ Introducción a Seguridad
├── leccion_02.md     ⏳ Binarios y ELF
├── leccion_03.md     ⏳ C Fundamentals
├── leccion_04.md     ⏳ GDB
├── leccion_05.md     ⏳ Objdump
├── leccion_06.md     ⏳ Buffer Overflow
├── leccion_07.md     ⏳ Format Strings
└── leccion_08.md     ⏳ Análisis
```

### Prácticas (próximamente):
```
02-practicas/
├── nivel_00/
│   ├── ejercicio_01.md + solucion_01.c
│   ├── ejercicio_02.md + solucion_02.sh
│   ├── ejercicio_03.md + solucion_03.c
│   └── ejercicio_04.md + solucion_04.c
├── nivel_01/
│   ├── ejercicio_01.md + solucion_01.sh
│   ├── ejercicio_02.md + solucion_02.c
│   ├── ejercicio_03.md + solucion_03.sh
│   ├── ejercicio_04.md + solucion_04.c
│   ├── ejercicio_05.md + solucion_05.sh
│   └── ejercicio_06.md + solucion_06.c
└── bonus/
    ├── desafio_01.md + solucion_01.sh
    ├── desafio_02.md + solucion_02.c
    └── desafio_03.md + solucion_03.sh
```

### Recursos (próximamente):
```
recursos/
├── herramientas.md
├── gdb-guia.md
├── objdump-guia.md
├── referencias.md
├── glosario.md
└── hints-futuros.md
```

---

## 🎯 Objetivos de Esta Estructura

| Objetivo | Cómo se logra |
|----------|---------------|
| **Progresivo** | Ejercicios van de fácil a difícil |
| **Pedagógico** | Teoría → Práctica inmediata |
| **Explícito** | Nada se asume "obvio" |
| **Autodidacta** | Puedes aprender solo sin un profesor |
| **Versionable** | Todo en Git, fácil de actualizar |
| **GitHub-ready** | Renderiza bien en GitHub |

---

## 📊 Estadísticas del Proyecto

| Métrica | Cantidad |
|---------|----------|
| Lecciones de Teoría | 8 |
| Ejercicios Nivel 00 | 4 |
| Ejercicios Nivel 01 | 6 |
| Desafíos Bonus | 3 |
| **Total de Ejercicios** | **13** |
| Horas de Teoría Estimada | ~7.5h |
| Horas de Práctica Estimada | ~10.5h |
| **Tiempo Total** | **~18h** |

---

## ✅ Checklist de Setup

Cuando hayas completado el setup, deberías:

- [ ] Haber leído README.md
- [ ] Haber leído SETUP.md
- [ ] Haber instalado todas las herramientas (gcc, gdb, git, etc.)
- [ ] Haber ejecutado los scripts de verificación
- [ ] Haber compilado y ejecutado el programa de prueba
- [ ] Estar listo para comenzar la Lección 01

---

## 🔗 Links Rápidos

| Quiero... | Click aquí |
|-----------|-----------|
| Empezar el curso | [README.md](README.md) |
| Instalar herramientas | [SETUP.md](SETUP.md) |
| Ver teoría | [01-teoria/INDEX.md](01-teoria/INDEX.md) |
| Hacer ejercicios | [02-practicas/INDEX.md](02-practicas/INDEX.md) |
| Recursos | [recursos/README.md](recursos/README.md) |

---

## 🎓 Notas Finales

Este proyecto está diseñado para **crecimiento progresivo**. No hay prisa. Tómate el tiempo para:

1. ✅ Entender cada concepto
2. ✅ Completar todos los ejercicios
3. ✅ Experimentar y jugar con el código
4. ✅ Documentar lo que aprendiste

Los conceptos de seguridad no se aprenden rápido, se aprenden *bien*.

---

**Última actualización:** Mayo 2026  
**Versión:** 1.0  
**Estado:** 🟢 Estructura lista, contenido en desarrollo

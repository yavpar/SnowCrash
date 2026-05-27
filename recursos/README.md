# 📚 Recursos - Materiales de Apoyo

Esta carpeta contiene materiales de referencia que complementan el curso de ft_snowcrash.

## 📑 Contenido

### Guías de Herramientas
- **[herramientas.md](herramientas.md)** (~4,500 palabras) - Descripción detallada de cada herramienta usada en el curso
  - GCC, GDB, Readelf, Objdump, File, Strings, Hexdump, Strace/Ltrace
  - Exemplos prácticos y flags importantes

- **[gdb-guia.md](gdb-guia.md)** (~5,000 palabras) - Tutorial completo de GDB (el debugger)
  - Debugging fundamentals, breakpoints avanzados, inspección de memoria
  - Stack frames, disassembly, automatización
  - Debugging de vulnerabilidades y troubleshooting

- **[objdump-guia.md](objdump-guia.md)** (~4,500 palabras) - Cómo usar objdump para análisis binario
  - Disassembly y lectura de assembly x86-64
  - Secciones y segmentos ELF
  - Búsqueda de gadgets, análisis de vulnerabilidades
  - Workflow práctico de ingeniería inversa

### Referencias y Conceptos
- **[glosario.md](glosario.md)** (~7,500 palabras) - Términos de ciberseguridad y binarios explicados
  - 100+ términos con definiciones, ejemplos y relaciones
  - ASLR, Assembly, Buffer overflow, ELF, Format strings, Heap, ROP, Stack, etc.

- **[referencias.md](referencias.md)** (~3,500 palabras) - Enlaces a documentación externa y recursos recomendados
  - Documentación oficial (GNU, Linux, ELF)
  - Libros fundamentales
  - Plataformas de práctica (HackTheBox, OverTheWire, etc.)
  - Videos, cursos y papers académicos

### Conceptos Avanzados (Hints para Proyectos Futuros)
- **[hints-futuros.md](hints-futuros.md)** (~3,500 palabras) - Conceptos que volverán en proyectos posteriores
  - ROP chains y gadgets
  - Heap exploitation
  - Bypass de protecciones (ASLR, NX, canaries)
  - Kernel exploitation
  - Malware analysis

### Scripts Auxiliares
Esta carpeta también puede contener scripts de Python u otros que generan ejercicios.

---

## 🎓 Cómo Usar Esta Carpeta

1. **Mientras lees una lección**: Consulta `glosario.md` para términos desconocidos
2. **Si necesitas ayuda con una herramienta**: Ve a `herramientas.md` o a las guías específicas
3. **Cuando busques información externa**: Revisa `referencias.md` para enlaces

---

## 📊 Estadísticas de Contenido

- **Total de archivos de recursos:** 6
- **Total de palabras:** ~28,500
- **Archivos completados:** 6/6 ✅

## 🎯 Cómo Usar Esta Carpeta

### Mientras lees una lección
- Consulta **glosario.md** para términos desconocidos
- Usa **herramientas.md** como referencia rápida

### Cuando te atascas con una herramienta
- **GDB no funciona?** → gdb-guia.md
- **Entender disassembly?** → objdump-guia.md
- **¿Qué es este término?** → glosario.md

### Para prepararte para lo siguiente
- **Al terminar Nivel 01** → Lee hints-futuros.md
- **Para investigar por tu cuenta** → referencias.md

### Orden de lectura recomendado

**Obligatorio (durante el curso):**
1. herramientas.md (como referencia)
2. glosario.md (consultar según necesites)

**Muy recomendado (durante Niveles 01+):**
3. gdb-guia.md (cuando hagas debugging)
4. objdump-guia.md (cuando analices binarios)

**Opcional (después de Nivel 00):**
5. referencias.md (si quieres aprender más)
6. hints-futuros.md (si quieres saber qué viene)

---

Última actualización: Mayo 2026

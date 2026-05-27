# Referencias y Recursos Externos

## Índice
1. [Documentación Oficial](#documentación-oficial)
2. [Libros Fundamentales](#libros-fundamentales)
3. [Plataformas de Práctica](#plataformas-de-práctica)
4. [Tutoriales y Artículos](#tutoriales-y-artículos)
5. [Comunidades y Foros](#comunidades-y-foros)
6. [Herramientas y Frameworks](#herramientas-y-frameworks)
7. [Videos y Cursos](#videos-y-cursos)
8. [Papers Académicos](#papers-académicos)

---

## Documentación Oficial

### GNU Tools
- **GCC Manual** (https://gcc.gnu.org/onlinedocs/)
  - Flags de compilación, optimizaciones, warnings
  - Especialmente importante: `-fno-stack-protector`, `-z execstack`

- **GDB Manual** (https://sourceware.org/gdb/documentation/)
  - Referencia completa de comandos
  - Debugging de core dumps

- **Binutils Documentation** (https://sourceware.org/binutils/docs/)
  - objdump, readelf, nm, strings
  - Assembly syntax y relocaciones

### Linux Kernel
- **Linux Man Pages** (https://man7.org/linux/)
  - Especialmente: `execve(2)`, `mmap(2)`, `signal(7)`

- **x86-64 ABI** (https://gitlab.com/x86-psABI/x86-64-ABI)
  - Convención de llamadas, stack layout
  - **CRÍTICO** para entender calling conventions

- **Intel x86 Manual** (https://www.intel.com/content/dam/develop/public/us/en/documents/manuals/64-ia-32-architectures-software-developers-manual-combined-volumes-1-2a-2b-2c-2d-3a-3b-3c-3d-4.pdf)
  - 5000+ páginas sobre instrucciones x86
  - Referencia definitiva pero abrumadora

### ELF Specification
- **ELF File Format** (https://refspecs.linuxbase.org/elf/)
  - Estructura completa de archivos ELF
  - Headers, sections, segments
  - **Lectura obligatoria** para ingeniería inversa

---

## Libros Fundamentales

### "Hacking: The Art of Exploitation" (Jon Erickson)
- Excelente introducción a exploits
- Buffer overflow desde lo básico
- Format string vulnerabilities
- **Nivel:** Principiante a Intermedio
- **Enfoque:** Práctico, ejemplos reales

### "The Art of Assembly Language" (Randall Hyde)
- Completo tutorial de assembly x86
- Memoria, registros, instrucciones
- **Nivel:** Principiante a Intermedio
- **Desventaja:** Enfocado en x86 32-bits (no x86-64)

### "Secure Coding in C and C++" (Robert C. Seacord)
- Cómo escribir código seguro
- Errores comunes y cómo evitarlos
- **Nivel:** Intermedio
- **Complemento:** Entiende por qué ciertas cosas son vulnerables

### "The Shellcoder's Handbook" (Ormandy, Koziol, Shacham)
- Enciclopedia de técnicas de exploit
- Profundo, detallado, técnico
- **Nivel:** Avanzado
- **Nota:** Actualizado a x86-64

### "Buffer Overflow Attacks" (Michael Stutterheim)
- Especializado en buffer overflows
- Análisis paso a paso
- **Nivel:** Intermedio
- **Ventaja:** Muy específico al tema

---

## Plataformas de Práctica

### Hack The Box (https://www.hackthebox.eu/)
- Máquinas vulnerables para explotar
- Desde fácil hasta imposible
- Enfoque: CTF realista
- **Para ft_snowcrash:** Excelente después de terminar

### OverTheWire (https://overthewire.org/)
- Wargames organizados por dificultad
- Bandit: bash basics
- Natas: web security
- **Para ft_snowcrash:** Empezar con Bandit si no tienes experiencia con Linux

### Tryhackme (https://tryhackme.com/)
- Aprendizaje guiado con máquinas
- Videos + ejercicios
- Muy principiante-friendly
- **Para ft_snowcrash:** Alternativa a HackTheBox

### pwn.college (https://pwn.college/)
- Especializado en binary exploitation
- Progresión clara de dificultad
- Muchos ejercicios ROP
- **Para ft_snowcrash:** Perfecto para después de Nivel 01

### OWASP WebGoat (https://owasp.org/www-project-webgoat/)
- Seguridad web (no binario)
- Enseña vulnerabilidades comunes
- **Para ft_snowcrash:** Después para contexto de seguridad web

---

## Tutoriales y Artículos

### Buffer Overflow
- **Smashing the Stack for Fun and Profit** (Aleph One)
  - Clásico de 1996, todavía relevante
  - Explicación paso a paso de overflow

- **Understanding Buffer Overflows** (Phrack Magazine)
  - Análisis técnico profundo

### Format Strings
- **Exploiting Format String Vulnerabilities** (Scut / Team Teso)
  - Cómo funcionan
  - Lectura y escritura de memoria
  - Ejecución de código

### Return-Oriented Programming (ROP)
- **ROP is Dead. Long Live ROP** (Shacham, 2007)
  - Paper académico sobre ROP chains
  - Matemáticamente riguroso

- **Synthesizing Robust Assembly Code** (Brumley et al.)
  - Automatización de búsqueda de gadgets

### ASLR y DEP
- **Address Space Layout Randomization** (PaX Team)
  - Cómo funciona ASLR
  - Bypass técnicas

- **Bypassing Non-Executable-Stack during Exploitation** (Phrack)
  - DEP/NX bit
  - ROP como workaround

---

## Comunidades y Foros

### Reddit
- **/r/learnprogramming** - General
- **/r/learnhacking** - Seguridad ofensiva
- **/r/binaryexploitation** - Especializado
- **/r/netsec** - Seguridad general
- **/r/reverseengineering** - Ingeniería inversa

### Stack Overflow (https://stackoverflow.com/)
- Preguntas sobre herramientas (GDB, objdump)
- Debugging
- Assembly
- Tags útiles: `gdb`, `assembly`, `x86-64`, `buffer-overflow`

### Phrack Magazine (https://phrack.org/)
- Revista técnica de seguridad
- Artículos profundos
- Gratis, acceso a todos los números históricos
- **Recomendación:** Leer números 49, 52, 60+ para history

### Security StackExchange (https://security.stackexchange.com/)
- Preguntas sobre concepto de seguridad
- Más teórico que Stack Overflow

### OWASP (https://owasp.org/)
- Recursos sobre seguridad ofensiva
- Top 10 vulnerabilidades web
- Testing guides

---

## Herramientas y Frameworks

### Reverse Engineering
- **IDA Pro** (https://www.hex-rays.com/ida-pro/)
  - Disassembler/debugger profesional
  - Caro pero poderoso
  - Alternativa gratis: Ghidra

- **Ghidra** (https://ghidra-sre.org/)
  - NSA's reverse engineering tool
  - Gratis, open source
  - Rival de IDA Pro
  - Excelente para análisis binario

- **Radare2** (https://rada.re/)
  - Framework modular de reversing
  - CLI-based
  - Curva de aprendizaje pronunciada

- **Cutter** (https://cutter.re/)
  - GUI para Radare2
  - Más amigable

### Exploit Development
- **Metasploit Framework** (https://www.metasploit.com/)
  - Framework de exploits listos
  - Generador de shellcode
  - Post-exploitation

- **pwntools** (https://docs.pwntools.com/)
  - Librería Python para exploit writing
  - Automación de comunicación con binarios
  - **Muy recomendada** para proyectos posteriores

- **ROPgadget** (https://github.com/JonathanSalwan/ROPgadget)
  - Buscador automático de ROP gadgets
  - Python script

- **Ropper** (https://github.com/sashs/Ropper)
  - Alternativa a ROPgadget
  - Más funcionalidades

### Debugging y Análisis
- **gdb-peda** (https://github.com/longld/peda)
  - Enhancement de GDB
  - Mejor visualización de memoria/stack
  - Muy útil para debugging de exploits

- **checksec** (https://github.com/slimm609/checksec.sh)
  - Analiza protecciones de binario
  - ASLR, stack canaries, NX bit, RELRO
  - Imprescindible para análisis

- **strace** / **ltrace**
  - Traza system calls / library calls
  - Incluida en Fedora

---

## Videos y Cursos

### YouTube Channels
- **John Hammond**
  - CTF writeups
  - Seguridad general
  - Muy didáctico

- **LiveOverflow**
  - Binary Exploitation series
  - ROP chains, shellcode
  - Muy técnico, muy bueno

- **Ippsec**
  - HackTheBox walkthroughs
  - Excelente para ver metodología
  - Buscar "binary" para exploits

### Cursos Online
- **Coursera - Cybersecurity Specialization**
  - General, no específico a binario

- **edX - CS50 Web Security**
  - Seguridad web principalmente
  - Algún contenido de binario

- **PentesterLab** (https://pentesterlab.com/)
  - Cursos prácticos
  - Algunos gratis, algunos pagos
  - Muy hands-on

---

## Papers Académicos

### Vulnerabilidades Clásicas
- **Aleph One - Smashing the Stack for Fun and Profit** (1996)
  - https://insecure.org/papers/stack-smashing.html
  - Fundacional

- **Xu et al. - Buffer Overflow Protection with Random Insertion of Instructions** (2001)
  - Cómo funcionan las defensas

### Defenses
- **Shacham et al. - On the Effectiveness of Address Space Randomization** (2004)
  - ASLR
  - Cómo bypassearlo

- **Shacham et al. - Executable Code, ROP is NOT Dead** (2007)
  - ROP como Turing completo
  - Prueba que NX+ASLR no es suficiente

### Return-Oriented Programming
- **Roemer et al. - Return-Oriented Programming: Attacks and Defenses** (2012)
  - Profundo análisis de ROP
  - Automatización de búsqueda

---

## Recursos por Proyecto School 42

Esta es la **primera parte** de una serie:

- **ft_snowcrash** (este): Buffer overflow, básico
- **Siguiente proyecto**: ROP chains, ASLR bypass
- **Después:** Heap exploitation, Use-after-free
- **Avanzado:** Kernel exploitation

### Para Prepararse
- Completar ft_snowcrash
- Hacer 10-15 máquinas de HackTheBox
- Intentar algunos retos de pwn.college
- Leer "Hacking: Art of Exploitation"

---

## Checklist de Lectura Recomendada

### Antes de Nivel 00
- [ ] Leer SETUP.md y verificar que todo funciona
- [ ] Leer Lección 01 (Introducción a Seguridad)

### Antes de Nivel 01
- [ ] Leer Lección 02 (Binarios y Memoria)
- [ ] Leer Lección 03 (C Fundamentals)
- [ ] Completar Nivel 00
- [ ] Leer herramientas.md (referencia)
- [ ] Leer glosario.md (referencia)

### Antes de Niveles Avanzados
- [ ] Leer Lección 04 (GDB)
- [ ] Leer Lección 05 (Objdump y Assembly)
- [ ] Leer gdb-guia.md completa
- [ ] Leer objdump-guia.md completa
- [ ] Ver "Smashing the Stack" artículo

### Complementario
- [ ] Un libro de los recomendados (elegir uno)
- [ ] HackTheBox: completar 5 máquinas
- [ ] Phrack Magazine: leer 2-3 artículos relevantes

---

## Siguientes Pasos Después de ft_snowcrash

1. **Práctica intensiva** (2-3 semanas)
   - HackTheBox: 10+ máquinas
   - pwn.college: completar módulos 1-3
   - Crear tus propios exploits

2. **Especialización** (elige uno)
   - **ROP**: Enfocarse en ROP chains
   - **Heap**: Aprender heap exploitation
   - **Web**: Seguridad de aplicaciones web

3. **Proyecto siguiente** en School 42
   - Usará muchos conceptos de aquí
   - Pero escalados en dificultad

---

## Nota Final

La seguridad ofensiva es un campo donde la **práctica es más importante que la teoría**. Estos recursos son para complementar:

1. Lectura de concepto (entiende QUÉ)
2. Práctica (entiende CÓMO)
3. Debugging (entiende POR QUÉ)

No temas pasar horas en GDB. Ese es el tiempo mejor invertido.

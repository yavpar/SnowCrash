# 🔧 Guía Detallada de Herramientas

Este documento proporciona información profunda y práctica sobre cada herramienta usada en el curso.

---

## GCC (GNU C Compiler)

### Descripción General

**GCC** es el compilador estándar de C en Linux. Convierte código fuente C en binarios ejecutables.

### Sintaxis Básica

```bash
gcc [FLAGS] archivo.c -o nombre_binario
```

### Flags Más Importantes

#### Compilación y Enlazado

| Flag | Efecto |
|------|--------|
| `-c` | Solo compila, no enlaza (genera .o) |
| `-o nombre` | Nombre del binario de salida |
| `-I/path` | Directorios adicionales para buscar headers |
| `-L/path` | Directorios adicionales para buscar librerías |
| `-lnombre` | Enlaza con librería (libname.so o libname.a) |

**Ejemplo:**
```bash
# Compilar todo en un paso
gcc main.c funciones.c -o programa

# Compilar a objeto, luego enlazar
gcc -c main.c -o main.o
gcc -c funciones.c -o funciones.o
gcc main.o funciones.o -o programa
```

#### Debug y Optimización

| Flag | Efecto |
|------|--------|
| `-g` | Incluye información de debug |
| `-O0` | Sin optimizaciones (por defecto) |
| `-O1` | Optimización básica |
| `-O2` | Optimización media (recomendada) |
| `-O3` | Optimización máxima (puede cambiar comportamiento) |

**Nota:** Para **aprender seguridad**, SIEMPRE usa `-O0` y `-g`:
```bash
gcc -g -O0 programa.c -o programa
```

#### Warnings y Errores

| Flag | Efecto |
|------|--------|
| `-Wall` | Muestra advertencias comunes |
| `-Wextra` | Más advertencias |
| `-Werror` | Trata advertencias como errores |
| `-Wpedantic` | Rigurosidad máxima |

**Ejemplo:**
```bash
gcc -Wall -g -O0 programa.c -o programa
```

#### Seguridad (Generalmente para DESACTIVAR)

| Flag | Efecto |
|------|--------|
| `-fstack-protector` | Añade stack canaries (defecto) |
| `-fstack-protector-all` | Stack canaries en todas las funciones |
| `-fno-stack-protector` | DESACTIVA stack canaries |
| `-z relro` | Read-only relocation table (defecto) |
| `-z now` | Resuelve símbolos al cargar (defecto) |
| `-z norelro` | Deshabilita relro |
| `-z lazy` | Lazy binding (permite ataques a PLT) |

**Para aprender sin protecciones:**
```bash
gcc -g -O0 -fno-stack-protector -z execstack programa.c -o programa
```

**Explicación de flags de seguridad:**
```
-fno-stack-protector  → Sin detección de stack overflow
-z execstack          → Stack ejecutable (permite shellcode en stack)
-z norelro            → GOT escribible (permite ataques)
-z lazy               → Lazy binding en librerías
```

#### Compilación Estática vs Dinámica

| Flag | Efecto |
|------|--------|
| `-static` | Enlaza librerías estáticamente |
| (default) | Enlaza dinámicamente |

**Diferencia:**
```bash
# Binario dinámica (pequeño, depende de libc.so.6)
gcc programa.c -o programa_dinámico
# Tamaño: ~15 KB

# Binario estático (grande, incluye libc)
gcc -static programa.c -o programa_estático
# Tamaño: ~3 MB
```

### Proceso de Compilación Detallado

```
programa.c (código fuente)
    ↓ [Preprocesador - gcc -E]
programa.i (código expandido, #includes procesados)
    ↓ [Compilador - gcc -S]
programa.s (assembly)
    ↓ [Ensamblador - as]
programa.o (object file, código máquina sin enlazar)
    ↓ [Linker - ld]
programa (binario ejecutable)
```

**Ver cada paso:**
```bash
gcc -E programa.c -o programa.i    # Preprocesar
gcc -S programa.c -o programa.s    # Compilar a assembly
gcc -c programa.c -o programa.o    # Compilar a objeto
gcc programa.o -o programa         # Enlazar
```

### Versión

```bash
gcc --version
```

**Output en Fedora:**
```
gcc (GCC) 13.2.1 20231205 (Red Hat 13.2.1-6)
Copyright (C) 2023 Free Software Foundation, Inc.
```

---

## GDB (GNU Debugger)

### Descripción General

**GDB** es el debugger estándar en Linux. Permite ejecutar un programa paso a paso e inspeccionar su estado.

### Iniciar GDB

```bash
gdb ./programa
gdb --args ./programa arg1 arg2
gdb --pid 1234              # Adjuntar a proceso corriendo
```

### Comandos Fundamentales

#### Control de Ejecución

```bash
run [args]          # Ejecutar el programa
run                 # Ejecutar sin argumentos
kill                # Matar el programa en ejecución
quit                # Salir de GDB
```

#### Breakpoints

```bash
break main                          # Breakpoint en función main
break 42                           # Breakpoint en línea 42
break archivo.c:100                # Breakpoint en archivo específico
break *0x401050                    # Breakpoint en dirección
condition 1 (x == 42)              # Breakpoint condicional
delete 1                           # Borra breakpoint número 1
disable/enable 1                   # Des/habilitar breakpoint
```

**Ejemplo - Breakpoint condicional:**
```bash
(gdb) break contador.c:100
Breakpoint 1 at 0x401050
(gdb) condition 1 (contador > 100)
(gdb) run
# Se pausa solo cuando contador > 100
```

#### Ejecución Paso a Paso

```bash
next                # Siguiente línea (no entra en funciones)
step                # Siguiente línea (entra en funciones)
continue            # Continúa hasta próximo breakpoint
finish              # Ejecuta hasta el fin de la función actual
```

**Diferencia next vs step:**
```c
int resultado = suma(5, 3);  // Línea 10

next:   Ejecuta suma(), baja a línea 11
step:   Entra DENTRO de suma(), ejecuta primera línea de suma()
```

#### Inspeccionar Variables

```bash
print variable                      # Imprime valor
print /x variable                   # Formato hexadecimal
print /t variable                   # Formato binario
print &variable                     # Dirección de la variable
print *ptr                          # Dereferenciar puntero
print (int *)ptr                    # Castear a tipo
```

**Ejemplo:**
```bash
(gdb) print x
$1 = 42
(gdb) print &x
$2 = (int *) 0x7fffffffde8c
(gdb) print /x x
$3 = 0x2a
(gdb) print /t x
$4 = 0b101010
```

#### Inspeccionar Memoria

```bash
x/10x 0x401000          # Examina 10 words (hexadecimal) desde dirección
x/20b 0x401000          # 20 bytes desde dirección
x/5i 0x401000           # 5 instrucciones desde dirección
x/10s 0x602000          # 10 strings desde dirección
```

**Formato: `x/[cantidad][formato] [dirección]`**

```
cantidad:   Número de unidades
formato:    x (hex), d (decimal), s (string), i (instrucción), c (char)

Ejemplo:
x/10x 0x401000      # 10 hex words
x/20b 0x401000      # 20 bytes
x/5i 0x401000       # 5 instrucciones
```

**Ejemplo práctico:**
```bash
(gdb) x/10x 0x7fffffffde8c
0x7fffffffde8c: 0x0000002a  0x00000064  0x00000000  0x00000000
0x7fffffffde9c: 0x7fffffffde00  0x00007fff  0x00000000  0x00000000

(gdb) x/10b 0x7fffffffde8c
0x7fffffffde8c: 0x2a    0x00    0x00    0x00    0x64    0x00    0x00    0x00
```

#### Variables Locales y Parámetros

```bash
info locals             # Muestra variables locales del frame actual
info args              # Muestra parámetros de la función actual
info variables         # Todas las variables (locales + globales)
info registers         # Estado de todos los registros
info registers rax rbx # Registros específicos
```

**Ejemplo:**
```bash
(gdb) info locals
contador = 42
buffer = 0x7fffffffde00 "usuario"

(gdb) info registers
rax            0x0                 0
rbx            0x555555554d90      93824992233872
rcx            0x5555555544b8      93824992227512
...
```

#### Stack Trace

```bash
backtrace               # Muestra llamadas de función (stack)
backtrace full          # Con variables locales
frame 0                 # Selecciona frame 0
frame 1                 # Selecciona frame 1 (función anterior)
up                      # Sube a frame anterior
down                    # Baja a frame siguiente
```

**Ejemplo:**
```bash
(gdb) backtrace
#0  suma (a=5, b=3) at main.c:20
#1  0x0000555555554b8d in main () at main.c:42
#2  0x00007ffff7a05c86 in __libc_start_main () from /lib/libc.so.6
#3  0x0000555555554a4a in _start ()

(gdb) frame 1
#1  0x0000555555554b8d in main () at main.c:42
42      int resultado = suma(5, 3);

(gdb) info locals
resultado = <optimized out>
x = 10
```

#### Escribir en Memoria

```bash
set variable var = valor           # Cambia variable
set *(int *)0x401000 = 0xdeadbeef # Escribe en dirección
```

**Ejemplo:**
```bash
(gdb) print x
$1 = 42
(gdb) set variable x = 999
(gdb) print x
$2 = 999
```

#### Disassembly (Assembly)

```bash
disassemble main                    # Disensambla función main
disassemble 0x401050               # Desensambla desde dirección
disassemble /m main                # Con código fuente intercalado
```

**Ejemplo:**
```bash
(gdb) disassemble main
Dump of assembler code for function main:
   0x0000555555554b8b <+0>:     push   %rbp
   0x0000555555554b8c <+1>:     mov    %rsp,%rbp
   0x0000555555554b8f <+4>:     mov    $0x5,%edi
   0x0000555555554b94 <+9>:     mov    $0x3,%esi
   0x0000555555554b99 <+14>:    callq  0x555555554b4a <suma>
   0x0000555555554b9e <+19>:    mov    %eax,-0x4(%rbp)
```

### Configuración Útil

Crear archivo `.gdbinit` en tu carpeta para cargar configuración automáticamente:

```bash
# En ~/.gdbinit o ./.gdbinit
set pagination off
set disassembly-flavor intel
handle SIGALRM nopass
```

### Debugging Avanzado

#### Watchpoints (Monitorear cambios)

```bash
watch variable                      # Pausa cuando variable cambia
awatch variable                     # Pausa cuando se accede (lectura/escritura)
rwatch variable                     # Pausa cuando se lee
```

**Ejemplo:**
```bash
(gdb) watch contador
Hardware watchpoint 2: contador
(gdb) continue
Hardware watchpoint 2: contador

Old value = 42
New value = 100
```

#### Logging

```bash
set logging on                      # Registra sesión a gdb.txt
set logging file output.log         # Archivo personalizado
```

---

## Readelf

### Descripción General

**Readelf** analiza archivos ELF en detalle. Muestra toda la información sobre binarios.

### Sintaxis

```bash
readelf [OPTIONS] archivo
```

### Opciones Principales

#### ELF Header

```bash
readelf -h archivo      # ELF Header (tipo, arquitectura, etc)
```

**Output:**
```
ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 00 00 00 00 00 00 00 00
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x401050
  Start of program headers:          64 (bytes into file)
  Start of section headers:          13488 (bytes into file)
  Flags:                             0x0
  Size of this header:               64 (bytes)
  Size of program header entry:      56 (bytes)
  Number of program headers:         13
  Size of section header entry:      64 (bytes)
  Number of section headers:         36
  Section header string table index: 35
```

**Información clave:**
- `Type:` EXEC (ejecutable), DYN (librería dinámica), REL (object file)
- `Machine:` Arquitectura (x86-64, ARM, MIPS, etc)
- `Entry point address:` Donde comienza la ejecución
- `Number of program headers:` Cuántos segmentos
- `Number of section headers:` Cuántas secciones

#### Program Headers (Segmentos)

```bash
readelf -l archivo      # Program Headers (para el SO)
```

**Output:**
```
Program Headers:
  Type           Offset             VirtAddr           PhysAddr
                 FileSiz            MemSiz              Flags  Align
  PHDR           0x0000000000000040 0x0000000000400040 0x0000000000400040
                 0x0000000000000268 0x0000000000000268  R E    0x8
  INTERP         0x00000000000002a8 0x00000000004002a8 0x00000000004002a8
                 0x000000000000001c 0x000000000000001c  R      0x1
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x000000000011c874 0x000000000011c874  R E    0x1000
  LOAD           0x000000000011dba0 0x000000000051dba0 0x000000000051dba0
                 0x0000000000004f20 0x0000000000005540  RW     0x1000
```

**Campos:**
- `Type:` LOAD (cargar en memoria), DYNAMIC (info de librerías), INTERP (interpreter), etc
- `Offset:` Posición en el archivo
- `VirtAddr:` Dirección virtual donde cargar
- `FileSiz:` Tamaño en disco
- `MemSiz:` Tamaño en memoria (puede diferir)
- `Flags:` R (read), W (write), E (execute)

**Nota:** `MemSiz > FileSiz` significa que hay espacio "sin inicializar" (como `.bss`)

#### Section Headers (Secciones)

```bash
readelf -S archivo      # Section Headers (para linker/debugger)
```

**Output:**
```
Section Headers:
  [Nr] Name              Type             Address           Offset
       Size              EntSize          Flags  Link  Info  Align
  [ 0]                   NULL             0000000000000000  00000000
       0000000000000000  0000000000000000           0      0     0
  [ 1] .interp           PROGBITS         0000000000400238  00000238
       000000000000001c  0000000000000000   A       0      0     1
  [ 2] .note.ABI-tag     NOTE             0000000000400254  00000254
       0000000000000020  0000000000000000   A       0      0     4
  [ 3] .text             PROGBITS         0000000000401000  00001000
       00000000000002e5  0000000000000000  AX       0      0     16
  [ 4] .rodata           PROGBITS         0000000000402000  00002000
       0000000000000018  0000000000000000   A       0      0     4
  [ 5] .data             PROGBITS         0000000000404000  00004000
       0000000000000010  0000000000000000  WA       0      0     8
  [ 6] .bss              NOBITS           0000000000404010  00004010
       0000000000000008  0000000000000000  WA       0      0     1
```

**Campos importantes:**
- `Type:` PROGBITS (datos), NOBITS (no ocupa espacio), SYMTAB (tabla de símbolos), etc
- `Address:` Dirección en memoria
- `Size:` Tamaño de la sección
- `Flags:` W (write), A (alloc), X (execute)

**Nota:** NOBITS significa que no ocupa espacio en disco (como `.bss`)

#### Tabla de Símbolos

```bash
readelf -s archivo      # Symbol table (funciones, variables globales)
readelf -s archivo | grep main  # Busca símbolo "main"
```

**Output:**
```
Symbol table '.symtab' contains 64 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000400238    0 SECTION LOCAL  DEFAULT    1 
    38: 0000000000401050   139 FUNC    GLOBAL DEFAULT   13 main
    40: 0000000000401020    18 FUNC    GLOBAL DEFAULT   13 suma
    45: 00000000004040d0     8 OBJECT  GLOBAL DEFAULT   27 contador
```

**Campos:**
- `Value:` Dirección del símbolo
- `Type:` FUNC (función), OBJECT (variable), etc
- `Bind:` GLOBAL (visible externamente), LOCAL (local a archivo)
- `Ndx:` Número de sección (13 = .text)

#### Dynamic Section (Librerías Dinámicas)

```bash
readelf -d archivo      # Dynamic section
```

**Output:**
```
Dynamic section at offset 0x3e10 contains 27 entries:
  Tag                Type                 Name/Value
 0x0000000000000001 (NEEDED)             Shared library: [libc.so.6]
 0x0000000000000001 (NEEDED)             Shared library: [ld-linux-x86-64.so.2]
 0x000000006ffffffe (VERNEED)            Address: 0x401cb0
 0x000000006fffffff (VERNEEDNUM)         6
 0x6000000000000010 (RELR)                0x4030a0
```

---

## Objdump

### Descripción General

**Objdump** es parte de binutils. Analiza binarios de forma similar a readelf pero con énfasis en código.

### Sintaxis

```bash
objdump [OPTIONS] archivo
```

### Opciones Principales

#### Disassembly (Código Máquina → Assembly)

```bash
objdump -d archivo                      # Disassembly completo
objdump -d archivo | head -50           # Primeras 50 líneas
objdump -d -M intel archivo             # Sintaxis Intel (más legible)
objdump -D archivo                      # Disassembly de todas las secciones
```

**Output (sintaxis AT&T, por defecto):**
```
0000000000401050 <main>:
  401050: 55                    push   %rbp
  401051: 48 89 e5              mov    %rsp,%rbp
  401054: 48 83 ec 20           sub    $0x20,%rsp
  401058: bf 05 00 00 00        mov    $0x5,%edi
  40105d: be 03 00 00 00        mov    $0x3,%esi
  401062: e8 b9 ff ff ff        callq  401020 <suma>
  401067: 89 45 fc              mov    %eax,-0x4(%rbp)
```

**Output (sintaxis Intel, más legible):**
```
objdump -d -M intel programa

0000000000401050 <main>:
  401050: 55                    push   rbp
  401051: 48 89 e5              mov    rbp,rsp
  401054: 48 83 ec 20           sub    rsp,0x20
  401058: bf 05 00 00 00        mov    edi,0x5
  40105d: be 03 00 00 00        mov    esi,0x3
  401062: e8 b9 ff ff ff        call   401020 <suma>
  401067: 89 45 fc              mov    DWORD PTR [rbp-0x4],eax
```

**Nota:** Usa siempre `-M intel` para mejor legibilidad.

#### Función Específica

```bash
objdump -d -M intel archivo | grep -A 30 "<main>:"
```

Muestra función main y 30 líneas siguientes.

#### Secciones (Headers)

```bash
objdump -h archivo          # Section headers (similar a readelf -S)
```

**Output:**
```
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .interp       0000001c  0000000000400238  0000000000400238  00000238  2**0
  1 .note.ABI-tag 00000020  0000000000400254  0000000000400254  00000254  2**2
  2 .text         000002e5  0000000000401000  0000000000401000  00001000  2**4
  3 .rodata       00000018  0000000000402000  0000000000402000  00002000  2**2
  4 .data         00000010  0000000000404000  0000000000404000  00004000  2**3
  5 .bss          00000008  0000000000404010  0000000000404010  00004010  2**0
```

#### Tabla de Símbolos

```bash
objdump -t archivo          # Symbol table
objdump -T archivo          # Dynamic symbols (librerías)
```

#### Contenido de Secciones

```bash
objdump -s archivo          # Contenido de todas las secciones
objdump -s -j .rodata archivo  # Solo sección .rodata
```

**Output:**
```
Contents of section .rodata:
 402000 01000200 48656c6c 6f2c2057 6f726c64  |....Hello, World|
 402010 00000000 00000000 00000000 00000000  |................|
```

#### Información Relocation

```bash
objdump -r archivo          # Relocation info
```

---

## File

### Descripción General

**File** identifica el tipo de un archivo basado en su contenido.

### Sintaxis

```bash
file archivo
```

### Output Típicos

```bash
$ file /bin/ls
/bin/ls: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), dynamically linked, ...

$ file programa.c
programa.c: C source code, ASCII text

$ file programa
programa: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, ...

$ file /lib/libc.so.6
/lib/libc.so.6: ELF 64-bit LSB shared object, x86-64, version 1 (GNU/Linux), ...

$ file core
core: ELF 64-bit LSB core file, x86-64, version 1 (SYSV), SVR4-style
```

### Información en Output

| Campo | Significado |
|-------|-------------|
| ELF | Formato (vs COFF, a.out, etc) |
| 64-bit | Arquitectura (32-bit, 64-bit) |
| LSB | Little-endian (vs MSB) |
| executable | Ejecutable |
| shared object | Librería compartida |
| dynamically linked | Usa libc dinámica |
| statically linked | Libc compilada dentro |

---

## Strings

### Descripción General

**Strings** extrae todos los strings (texto) de un binario.

### Sintaxis

```bash
strings archivo
strings archivo | grep patron
```

### Uso Práctico

```bash
# Ver todos los strings
strings /bin/ls

# Buscar patterns específicos
strings /bin/ls | grep -i error

# Contar strings
strings programa | wc -l

# Mostrar dirección de string
strings -a programa    # -a = muestra dirección

# Mínimo de 10 caracteres
strings -n 10 programa
```

### Output Típico

```bash
$ strings ./programa
/lib64/ld-linux-x86-64.so.2
libc.so.6
__stack_chk_fail
printf
__libc_start_main
main
GLIBC_2.4
__gmon_start__
Hola, mundo
```

**Nota:** Incluye nombre de librerías, funciones de libc, strings que el programa usa, etc.

---

## Hexdump

### Descripción General

**Hexdump** muestra el contenido de un archivo en hexadecimal y ASCII.

### Sintaxis

```bash
hexdump -C archivo          # Formato canonical (recomendado)
hexdump -C archivo | head   # Primeras líneas
```

### Output

```bash
$ hexdump -C ./programa | head
00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
00000010  02 00 3e 00 01 00 00 00  50 10 40 00 00 00 00 00  |..>.....P.@.....|
00000020  40 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  |@...............|
00000030  00 00 00 00 40 00 38 00  0d 00 40 00 00 00 00 00  |....@.8...@.....|
```

**Formato:**
- Offset (izquierda)
- Hex (16 bytes por línea)
- ASCII (derecha)

---

## Strace

### Descripción General

**Strace** traza syscalls (llamadas del sistema) que un programa realiza.

### Sintaxis

```bash
strace programa              # Traza todas las syscalls
strace -e trace=open programa  # Solo syscalls específicas
strace -o output.txt programa  # Guarda en archivo
```

### Output Típico

```bash
$ strace ./programa
execve("./programa", ["./programa"], 0x7ffc6c5c0df0) = 0
brk(NULL)                               = 0x555555754000
access("/etc/ld.so.nohwcap", F_OK)     = -1 ENOENT (No such file or directory)
openat(AT_FDCWD, "/lib64/ld-linux-x86-64.so.2", O_RDONLY|O_CLOEXEC) = 3
mmap(NULL, 849280, PROT_READ|PROT_EXEC, MAP_PRIVATE|MAP_DENYWRITE, 3, 0) = 0x7ffff7a1e000
write(1, "Hola, mundo\n", 12Hola, mundo
write(1, "Hola, mundo\n", 12)          = 12
exit_group(0)                           = ?
```

**Información útil:**
- `execve()` - Ejecución del programa
- `openat()` - Aperturas de archivos
- `mmap()` - Mapeo de memoria
- `write()` - Escrituras a stdout/stderr
- `exit_group()` - Terminación

---

## Ltrace

### Descripción General

**Ltrace** traza llamadas a función de librerías dinámicas.

### Sintaxis

```bash
ltrace programa              # Traza llamadas a librerías
ltrace -e printf programa    # Solo funciones específicas
```

### Output Típico

```bash
$ ltrace ./programa
malloc(100)                      = 0x5555557572a0
printf("Contador: %d\n", 42)     = 17
free(0x5555557572a0)             = <void>
exit(0)                          = <no return>
```

---

## Comparación: Quién Hace Qué

| Herramienta | Propósito | Tipo de Análisis |
|-------------|----------|-----------------|
| **file** | Identifica tipo | Rápido |
| **readelf** | Info completa de ELF | Estático |
| **objdump** | Disassembly + headers | Estático |
| **strings** | Extrae texto | Estático |
| **hexdump** | Contenido raw | Estático |
| **gdb** | Ejecución paso a paso | Dinámico |
| **strace** | Syscalls | Dinámico |
| **ltrace** | Llamadas a librerías | Dinámico |

---

**Última actualización:** Mayo 2026  
**Palabras totales:** ~4,500  
**Estado:** ✅ Completo


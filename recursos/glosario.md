# 📖 Glosario - Términos de Ciberseguridad y Binarios

Este glosario define términos técnicos usados en el curso. Es una referencia rápida mientras estudias.

**Nota:** Los términos están en orden alfabético. Algunos términos se refieren a otros - sigue los enlaces.

---

## A

### ASLR (Address Space Layout Randomization)
**Definición:** Técnica de defensa que randomiza las direcciones de memoria cada vez que un programa corre.

**Ejemplo:**
```
Ejecución 1: programa cargado en 0x400000
Ejecución 2: programa cargado en 0x562a0000
Ejecución 3: programa cargado en 0x7f5a0000
```

**Por qué importa:** Los exploits antiguos usaban direcciones "hardcodeadas". ASLR hace esto imposible.

**Para aprender:** Se suele deshabilitar ASLR:
```bash
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

**Relacionado:** [NX bit](#nx-bit), [Stack Canary](#stack-canary)

---

### Assembly / Ensamblador
**Definición:** Lenguaje de bajo nivel que representa instrucciones del procesador de forma legible.

**Ejemplo (x86-64):**
```asm
mov $0x2a, %eax     ; Mueve 42 (0x2a) al registro eax
add $0x1, %eax      ; Suma 1 a eax
ret                 ; Retorna
```

**Por qué importa:** Los binarios ejecutables SON assembly. Entender assembly = entender binarios.

**Niveles de abstracción:**
```
C Code:
    int x = 42;

Assembly:
    mov $0x2a, -0x4(%rbp)

Machine Code:
    c7 45 fc 2a 00 00 00
```

**Relacionado:** [Objdump](#objdump), [Disassembly](#disassembly)

---

## B

### Binary
**Definición:** Archivo ejecutable compilado a código máquina.

**Formato en Linux:** [ELF](#elf)

**Ejemplo:**
```
Archivo fuente:  programa.c (texto legible)
      ↓ (compilar con gcc)
Binary:          programa (código máquina)
```

**Tipos:**
- **Ejecutable:** Puede correr directamente (`./programa`)
- **Librería compartida:** `.so` (Shared Object) - reutilizable
- **Objeto:** `.o` (Object file) - intermedio durante compilación

**Relacionado:** [ELF](#elf), [Compilación](#compilación)

---

### Buffer
**Definición:** Área de memoria reservada para almacenar datos (típicamente strings o arrays).

**Ejemplo:**
```c
char buffer[100];  // Buffer de 100 bytes
```

**En memoria:**
```
Dirección  Contenido
0x1000     [b] [u] [f] [f] [e] [r] [.] [.] [.]
0x1008     [.] [.] [.] [.] [...99 bytes más...]
0x1063     [fin del buffer]
```

**Relacionado:** [Buffer Overflow](#buffer-overflow)

---

### Buffer Overflow
**Definición:** Vulnerabilidad donde se escribe MÁS datos en un buffer de los que caben.

**Ejemplo:**
```c
char buffer[10];
strcpy(buffer, "123456789012345");  // 15 caracteres en 10 bytes
```

**En memoria:**
```
ANTES:
buffer:  [0123456789]  (10 bytes)
other:   [xxx]

DESPUÉS (después del overflow):
buffer:  [0123456789012345]  (escrito 15)
other:   [XXX]  ← Sobrescribido (corrupción)
```

**Tipos:**
- **Stack overflow:** Sobrescribe variables locales y return address
- **Heap overflow:** Sobrescribe datos en el heap
- **Off-by-one:** Escribe UN byte más (fácil de pasar por alto)

**Impacto:** Crash, corrupción de datos, **ejecución de código arbitrario**

**Relacionado:** [Stack](#stack), [Heap](#heap), [Exploit](#exploit)

---

## C

### Compilación
**Definición:** Proceso de convertir código fuente (C) a código máquina (binario).

**Pasos:**

```
1. Preprocesamiento
   Fuente C → Directivas (#include, #define)
   
2. Compilación
   Código procesado → Assembly
   
3. Ensamblaje
   Assembly → Código máquina
   
4. Enlazado (Linking)
   Código máquina + Librerías → Binario ejecutable
```

**Comando:**
```bash
gcc programa.c -o programa
```

**Flags útiles:**
```bash
-g              # Incluye debug info
-O0             # Sin optimizaciones (para aprender)
-Wall           # Muestra todas las advertencias
-fno-stack-protector  # Desactiva protecciones (para aprender)
```

**Optimizaciones (peligrosas):**
```bash
-O1, -O2, -O3   # Optimizaciones agresivas
                # Pueden cambiar el comportamiento del programa
```

**Relacionado:** [GCC](#gcc), [Binario](#binary), [ELF](#elf)

---

### Core Dump
**Definición:** Archivo que contiene la memoria de un programa en el momento del crash.

**Cuándo ocurre:** Cuando un programa crashea (segmentation fault).

**Uso:** Análisis postmortem del crash.

**Habilitar:**
```bash
ulimit -c unlimited  # Habilita core dumps
```

**Analizar:**
```bash
gdb programa core    # Carga el core dump en GDB
```

---

## D

### Data Section
**Definición:** Sección del binario que contiene variables inicializadas.

**En ELF:** `.data`

**Ejemplo:**
```c
int contador = 42;        // Aquí va
char nombre[] = "Juan";   // Aquí va
```

**Características:**
- Ocupa espacio en disco (a diferencia de `.bss`)
- Cargada en memoria al inicio
- Permiso: lectura + escritura (RW)

**En memoria:**
```
.data:
Dirección  Contenido
0x601000   [contador = 42]
0x601004   ["Juan\0"]
```

**Relacionado:** [BSS](#bss), [RODATA](#rodata), [ELF](#elf)

---

### Debugger
**Definición:** Herramienta para ejecutar un programa paso a paso e inspeccionar su estado.

**Principal en Linux:** `gdb`

**Capacidades:**
- Pausar ejecución (breakpoints)
- Ejecutar paso a paso (step, next)
- Inspeccionar variables
- Leer/escribir memoria

**Uso básico:**
```bash
gdb ./programa
(gdb) break main
(gdb) run
(gdb) next
(gdb) print variable
```

**Relacionado:** [GDB](#gdb), [Breakpoint](#breakpoint)

---

### Disassembly
**Definición:** Proceso de convertir código máquina a assembly (legible para humanos).

**Herramienta:** `objdump`, `gdb`, `IDA Pro`, `Ghidra`

**Ejemplo:**
```bash
objdump -d ./programa | head -20
```

**Salida:**
```
0000000000401050 <main>:
  401050: 55                    push   %rbp
  401051: 48 89 e5              mov    %rsp,%rbp
  401054: bf 01 00 00 00        mov    $0x1,%edi
  401059: e8 d2 ff ff ff        callq  401030 <exit@plt>
```

**Nota:** Es legible pero requiere aprender assembly.

**Relacionado:** [Assembly](#assembly), [Objdump](#objdump)

---

## E

### Exploit
**Definición:** Código o técnica que usa una [vulnerabilidad](#vulnerabilidad) para causar daño.

**Diferencia crucial:**
```
Vulnerabilidad = Defecto en el código
Exploit = Forma de usar ese defecto
```

**Ejemplo:**
```
Vulnerabilidad: strcpy sin límite
Exploit: Script que envía string muy largo para causar crash/code execution
```

**Tipos:**
- **Local:** Corre en la misma máquina
- **Remoto:** Corre remotamente a través de la red
- **Privilege escalation:** Sube privilegios (user → root)

**Relacionado:** [Vulnerabilidad](#vulnerabilidad), [Payload](#payload)

---

### ELF (Executable and Linkable Format)
**Definición:** Formato estándar para binarios en Linux.

**Estructura básica:**
```
[ELF Header]
[Program Headers] ← Para el SO
[.text]           ← Código
[.data]           ← Datos
[.bss]            ← Datos no inicializados
[Section Headers] ← Para linker/debugger
[Symbol Table]
[String Table]
```

**Herramientas de análisis:**
- `file` - Tipo básico
- `readelf` - Información ELF
- `objdump` - Disassembly
- `strings` - Strings en el binario

**Importante:** Todos los binarios Linux son ELF.

**Relacionado:** [Sección](#sección), [Segmento](#segmento)

---

## F

### Format String Vulnerability
**Definición:** Vulnerabilidad donde un atacante controla el formato de una función como `printf`.

**Ejemplo vulnerable:**
```c
printf(user_input);  // ¡Peligro!
```

**Si user_input = "%x %x %x":**
- Lee valores del stack
- Puede leer secretos

**Si user_input = "%n":**
- Escribe en memoria
- Puede sobreescribir variables

**Impacto:** Lectura y escritura de memoria arbitraria.

**Relacionado:** [Vulnerabilidad](#vulnerabilidad), [Exploit](#exploit)

---

### Frame Pointer
**Definición:** Registro especial (RBP en x86-64) que apunta a la base del stack frame actual.

**Uso:**
```
RBP → [Dirección de retorno]
      [Parámetro 1]
      [Parámetro 2]
      [Variable local 1]
      [Variable local 2]
      ...
ESP → [Tope del stack]
```

**En assembly:**
```asm
push %rbp              ; Guarda RBP anterior
mov %rsp, %rbp         ; RBP = nuevo frame
sub $0x10, %rsp        ; Reserva 16 bytes en stack
```

**Relacionado:** [Stack Frame](#stack-frame), [Return Address](#return-address)

---

## G

### GCC (GNU C Compiler)
**Definición:** Compilador estándar de C en Linux.

**Uso básico:**
```bash
gcc archivo.c -o binario
```

**Flags comunes:**
```bash
-c              # Solo compila (no enlaza)
-o nombre       # Nombre del binario
-g              # Debug info (necesario para GDB)
-O0             # Sin optimizaciones
-Wall           # Advertencias
-fno-stack-protector  # Sin protecciones
-z execstack    # Stack ejecutable
```

**Verión:**
```bash
gcc --version
```

**Relacionado:** [Compilación](#compilación), [Binario](#binary)

---

### GDB (GNU Debugger)
**Definición:** Debugger estándar en Linux para análisis dinámico.

**Comandos básicos:**
```bash
break main           # Breakpoint en main
run                  # Ejecuta hasta breakpoint
next                 # Siguiente línea
step                 # Step into función
continue             # Continúa ejecución
print variable       # Imprime valor
x/10x 0x1000         # Inspecciona memoria
info registers       # Muestra registros
backtrace            # Stack trace
```

**Compilar para GDB:**
```bash
gcc -g -O0 programa.c -o programa
gdb ./programa
```

**Relacionado:** [Debugger](#debugger), [Breakpoint](#breakpoint)

---

## H

### Heap
**Definición:** Región de memoria para asignación dinámica.

**Características:**
- Manual (malloc/free)
- Crecimiento dinámico
- Lento (fragmentación)
- Grande (gigabytes)

**Ejemplo:**
```c
int *ptr = malloc(100);  // Reserva 100 bytes
// ... usa ptr ...
free(ptr);               // Libera memoria
```

**Vulnerabilidades comunes:**
- **Use-after-free:** Usar después de free()
- **Double-free:** Liberar dos veces
- **Heap overflow:** Escribir más bytes de los reservados

**Estructura interna:**
```
[Metadatos] [Datos] [Metadatos] [Datos] ...
```

Los metadatos (chunk headers) contienen size, flags, etc.

**Relacionado:** [Stack](#stack), [Buffer Overflow](#buffer-overflow)

---

## I

### Instruction Pointer / Program Counter
**Definición:** Registro que apunta a la SIGUIENTE instrucción a ejecutar.

**Nombres por arquitectura:**
- x86-64: `RIP` (Register Instruction Pointer)
- x86-32: `EIP`
- ARM: `PC` (Program Counter)

**Ejemplo:**
```
Instrucción actual:  mov $0x42, %eax     (en 0x401000)
RIP apunta a:        0x401005             (siguiente instrucción)
```

**Crítica para exploits:** Controlar RIP = controlar la ejecución.

**Relacionado:** [Return Address](#return-address), [ROP](#rop)

---

### Injection Attack
**Definición:** Ataque donde se "inyecta" código o datos maliciosos.

**Tipos comunes:**
- **SQL Injection:** Inyectar comandos SQL
- **Format String:** Inyectar especificadores de formato
- **Code Injection:** Inyectar código ejecutable
- **Command Injection:** Inyectar comandos del SO

**Ejemplo - Command Injection:**
```
Entrada esperada: "documento.txt"
Entrada maliciosa: "documento.txt; rm -rf /"
Resultado: ¡Borra todo el servidor!
```

---

## L

### LIBC / C Standard Library
**Definición:** Librería de funciones estándar de C (printf, strcpy, malloc, etc).

**En Linux:** `/lib/x86_64-linux-gnu/libc.so.6`

**Funciones comunes:**
- I/O: printf, scanf, fgets
- String: strcpy, strlen, strcmp
- Memory: malloc, free, memcpy
- Otros: exit, sleep, system

**En binarios dinámicos:**
- Las funciones de libc se "importan" en runtime
- Localizadas en la [GOT](#got) / [PLT](#plt)

**Relacionado:** [GOT](#got), [PLT](#plt)

---

## M

### Machine Code
**Definición:** Código binario que el procesador ejecuta directamente.

**Forma:** Bytes hexadecimales

**Ejemplo:**
```
Assembly:   mov $0x42, %eax
Machine:    b8 42 00 00 00
```

**Notas:**
- Intel usa little-endian (bytes invertidos)
- Cada instrucción ocupa diferente cantidad de bytes
- No es legible para humanos (por eso existe assembly)

**Relacionado:** [Assembly](#assembly), [Disassembly](#disassembly)

---

### Memory Layout
**Definición:** Cómo se organiza la memoria de un proceso.

**Layout típico (direcciones altas → bajas):**
```
0xFFFFFFFF  ┌─────────────────┐
            │ Kernel space    │  (inaccesible)
            ├─────────────────┤
            │ Stack (↓)       │  (crece hacia abajo)
            │                 │
            ├─────────────────┤
            │ (vacío)         │
            ├─────────────────┤
            │ Heap (↑)        │  (crece hacia arriba)
            ├─────────────────┤
            │ .bss            │  (datos no inicializados)
            ├─────────────────┤
            │ .data           │  (datos inicializados)
            ├─────────────────┤
            │ .rodata         │  (read-only data)
            ├─────────────────┤
            │ .text           │  (código)
0x00000000  └─────────────────┘
```

**Sin ASLR:** Direcciones fijas.
**Con ASLR:** Direcciones cambian en cada ejecución.

**Relacionado:** [ASLR](#aslr), [Stack](#stack), [Heap](#heap)

---

## N

### NX Bit (No eXecute)
**Definición:** Protección que marca páginas como no ejecutables.

**Impacto:**
- Stack no ejecutable
- Heap no ejecutable
- Datos no ejecutables

**Sin NX:** Puedes ejecutar código en cualquier lugar.
**Con NX:** Solo el código legítimo (.text) es ejecutable.

**Bypass:** [ROP Chains](#rop)

**Relacionado:** [ASLR](#aslr), [Stack Canary](#stack-canary)

---

## O

### Objdump
**Definición:** Herramienta para analizar binarios (parte de binutils).

**Usos principales:**
```bash
objdump -d programa          # Disassembly
objdump -D programa          # Disassembly completo
objdump -h programa          # Secciones
objdump -t programa          # Tabla de símbolos
objdump -s programa          # Contenido de secciones
```

**Ejemplo:**
```bash
$ objdump -d ./programa | head -20

0000000000401050 <main>:
  401050: 55                    push   %rbp
  401051: 48 89 e5              mov    %rsp,%rbp
  401054: bf 01 00 00 00        mov    $0x1,%edi
```

**Relacionado:** [Disassembly](#disassembly), [Readelf](#readelf)

---

### Offset
**Definición:** Distancia desde un punto de referencia.

**Contextos diferentes:**
1. **Offset en archivo:** Posición bytes desde inicio del archivo
2. **Offset en memoria:** Distancia desde base de una sección
3. **Offset en estructura:** Posición de un campo dentro de una estructura

**Ejemplo:**
```
Archivo ELF:
  0x0000: ELF Header
  0x0040: Program Headers
  0x0100: .text section
  Offset de .text = 0x0100
```

---

## P

### Payload
**Definición:** Datos maliciosos enviados para explotar una vulnerabilidad.

**Ejemplo - Buffer Overflow:**
```
Payload: [AAAA...AAAA] [Dirección maliciosa] [Shellcode]
         └─ Llena buffer  └─ Sobrescribe RIP  └─ Código a ejecutar
```

**Ejemplo - Format String:**
```
Payload: "%x %x %x %s"
         └─ Lee stack y parámetros
```

**Relacionado:** [Exploit](#exploit), [Shellcode](#shellcode)

---

### PLT (Procedure Linkage Table)
**Definición:** Tabla de funciones importadas de librerías dinámicas.

**Ubicación:** En .text o sección especial

**Propósito:** Resolver direcciones de funciones en runtime.

**Cómo funciona:**
```
1. Programa llama a printf
2. Salta a PLT[printf]
3. PLT redirige a GOT[printf]
4. GOT contiene dirección real de printf en libc
5. Se ejecuta printf
```

**Nota:** Las direcciones se resuelven la primera vez que se llama (lazy binding).

**Vulnerabilidad:** Modificar GOT = redirigir funciones.

**Relacionado:** [GOT](#got), [LIBC](#libc)

---

### Pointer
**Definición:** Variable que contiene una dirección de memoria.

**Sintaxis C:**
```c
int x = 42;
int *ptr = &x;      // ptr contiene dirección de x

*ptr        // Dereferenciar: obtener valor (42)
&ptr        // Obtener dirección de ptr
ptr + 1     // Aritmética: siguiente elemento
```

**Aritmética de punteros:**
```c
int *p = 0x1000;
p + 1      // 0x1004 (suma sizeof(int) = 4)

char *q = 0x2000;
q + 1      // 0x2001 (suma sizeof(char) = 1)
```

**Riesgos:**
- Punteros nulos
- Punteros dangling (apuntan a memoria liberada)
- Desbordamiento de punteros

**Relacionado:** [Stack](#stack), [Heap](#heap)

---

## R

### Readelf
**Definición:** Herramienta para analizar archivos ELF en detalle.

**Usos:**
```bash
readelf -h archivo           # ELF Header
readelf -l archivo           # Program Headers (segmentos)
readelf -S archivo           # Section Headers (secciones)
readelf -s archivo           # Tabla de símbolos
readelf -d archivo           # Dynamic section
```

**Ejemplo:**
```bash
$ readelf -S ./programa

Section Headers:
  [Nr] Name              Type             Address           Offset
  [ 1] .text             PROGBITS         0000000000401000  00001000
  [ 2] .rodata           PROGBITS         0000000000402000  00002000
  [ 3] .data             PROGBITS         0000000000601000  00101000
  [ 4] .bss              NOBITS           0000000000601020  00101020
```

**Relacionado:** [ELF](#elf), [Objdump](#objdump)

---

### Return Address
**Definición:** Dirección de la instrucción siguiente a una llamada a función.

**En el stack:**
```
push %rbp
mov %rsp, %rbp
... función ...
ret              ← Lee return address del stack, salta allá
```

**Almacenado automáticamente:**
```asm
call función     ← Pushea return address al stack
```

**Crítica para seguridad:**
- Sobrescribir return address = controlar dónde va el programa
- Base de buffer overflow exploits

**Relacionado:** [Stack Frame](#stack-frame), [Buffer Overflow](#buffer-overflow)

---

### ROP (Return-Oriented Programming)
**Definición:** Técnica de exploit que encadena pequeños fragmentos de código existente.

**Idea básica:**
```
Gadget 1: pop %rdi; ret
Gadget 2: pop %rsi; ret
Gadget 3: syscall; ret

Exploit encadena gadgets para ejecutar código sin inyectar nada
```

**Por qué es importante:**
- Bypasea protecciones como NX
- Usa código legítimo del programa
- Muy complejo pero muy poderoso

**Nota:** ROP es avanzado, se cubre en proyectos posteriores.

**Relacionado:** [NX Bit](#nx-bit), [Gadget](#gadget)

---

### RODATA (Read-Only Data)
**Definición:** Sección del binario con datos de solo lectura.

**En ELF:** `.rodata`

**Ejemplo:**
```c
const int MAX = 100;        // Aquí va
"Hola, mundo"               // Strings literales aquí
const char *msg = "Debug";  // El string aquí va
```

**Características:**
- Ocupa espacio en disco
- Cargada en memoria
- Permiso: lectura (R)
- No se puede modificar (protegida por SO)

**Almacenamiento:**
```
.rodata:
Dirección  Contenido
0x402000   ["Hola, mundo\0"]
0x40200d   ["Debug\0"]
```

**Relacionado:** [Data Section](#data-section), [BSS](#bss)

---

## S

### Sección
**Definición:** Parte lógica de un binario ELF con propósito específico.

**Principales:**
- `.text` - Código ejecutable
- `.data` - Datos inicializados
- `.bss` - Datos no inicializados
- `.rodata` - Datos de solo lectura
- `.symtab` - Tabla de símbolos
- `.strtab` - Tabla de strings

**Vista del programador (linker):**
```
Secciones = bloques granulares
Código en .text, datos en .data, etc.
```

**Vista del SO (kernel):**
```
Segmentos = grupos de secciones
.text + .rodata → Segmento LOAD (RX)
.data + .bss → Segmento LOAD (RW)
```

**Nota:** Múltiples secciones pueden estar en un segmento.

**Herramientas:**
```bash
readelf -S archivo      # Ver secciones
objdump -h archivo      # Ver secciones
```

**Relacionado:** [ELF](#elf), [Segmento](#segmento)

---

### Segmento
**Definición:** Vista de un binario desde la perspectiva del SO.

**Tipos principales:**
- `LOAD` - Cargable en memoria
- `DYNAMIC` - Información de librerías dinámicas
- `INTERP` - Interprete (loader)
- `GNU_STACK` - Permisos del stack

**Cada segmento tiene:**
- Dirección virtual donde cargar
- Tamaño en disco
- Tamaño en memoria
- Permisos (read, write, execute)

**Ejemplo LOAD:**
```
Offset en archivo: 0x1000
Dirección virtual: 0x400000
Tamaño disco: 2048
Tamaño memoria: 2048
Permisos: RX (read, execute)
```

**Nota:** El SO no sabe de secciones, solo de segmentos.

**Relacionado:** [Sección](#sección), [ELF](#elf)

---

### Shellcode
**Definición:** Código máquina crudo que realiza una acción (típicamente invocar shell).

**Ejemplo simple (x86-64):**
```asm
mov $0x3b, %rax      ; syscall number para execve
mov $0x400000, %rdi  ; dirección de "/bin/sh"
mov $0x0, %rsi       ; sin argumentos
mov $0x0, %rdx       ; sin env
syscall              ; Ejecuta syscall
```

**En bytes hexadecimales:**
```
b8 3b 00 00 00 48 c7 c7 00 00 40 00 48 c7 c6 00 ...
```

**Uso:** Inyectar en buffer overflow, después de sobrescribir return address.

**Nota:** Cada arquitectura tiene shellcode diferente.

**Relacionado:** [Exploit](#exploit), [Payload](#payload)

---

### Stack
**Definición:** Región de memoria para variables locales y control de llamadas a función.

**Características:**
- Automático (LIFO)
- Rápido (contiguo)
- Limitado (~8MB)
- Crece hacia abajo (en x86)

**Estructura:**
```
Dirección alta
    ↑
    │ [variables de main]
    │ [variables de funcion1]
    │ [variables de funcion2]  ← Aquí ahora
    │
Dirección baja
```

**En x86-64:**
- `RSP` - Stack Pointer (tope del stack)
- `RBP` - Frame Pointer (base del frame actual)

**Vulnerabilidad:** Stack overflow sobrescribe variables vecinas.

**Relacionado:** [Heap](#heap), [Stack Frame](#stack-frame)

---

### Stack Canary
**Definición:** Valor especial escrito en el stack para detectar overflows.

**Funcionamiento:**
```
Stack:
[local variables]
[CANARY = 0x12345678]  ← Valor único por ejecución
[return address]
[parámetros]

Al retornar: Verifica que CANARY sigue siendo 0x12345678
Si cambió → overflow detectado → termina el programa
```

**Tipos:**
- **Stack canaries:** El valor original se deja en el stack
- **Terminator canaries:** Usa bytes que terminan strings (nulo, newline)

**Limitaciones:**
- No previene todos los overflows
- Si el atacante sabe el valor, puede saltarse

**Relacionado:** [NX Bit](#nx-bit), [ASLR](#aslr)

---

### Stack Frame
**Definición:** Área en el stack asignada a una función.

**Contenido:**
```
[Frame de funcion2]
    ├─ Return address (a funcion1)
    ├─ RBP anterior (frame pointer)
    ├─ Variables locales
    └─ Espacio temporario
[Frame de funcion1]
    ├─ Return address (a main)
    ├─ RBP anterior
    ├─ Variables locales
    └─ Espacio temporario
[Frame de main]
    ├─ Parámetros del programa
    ├─ Variables locales
    └─ ...
```

**Ejemplo en C:**
```c
void funcion2() {          // Nuevo frame
    int x = 42;
}                          // Frame destruido

void funcion1() {
    int y = 100;
    funcion2();            // Crea nuevo frame
}

int main() {
    int z = 1000;
    funcion1();
}
```

**Herramientas para ver frames:**
```bash
gdb ./programa
(gdb) break main
(gdb) run
(gdb) backtrace           # Ver stack de frames
(gdb) frame 0             # Ver frame actual
```

**Relacionado:** [Stack](#stack), [Frame Pointer](#frame-pointer), [Return Address](#return-address)

---

### String
**Definición:** Secuencia de caracteres terminada en null ('\0').

**En C:**
```c
char str[] = "Hola";

En memoria:
['H'] ['o'] ['l'] ['a'] ['\0']
 72   111   108   97    0
```

**Importante:** El '\0' es CRÍTICA. Sin él, funciones como strlen no saben dónde termina.

**Funciones peligrosas:**
```c
strcpy(dest, src);    // Sin límite → overflow
strcat(dest, src);    // Sin límite → overflow
gets(buffer);         // Sin límite → overflow
scanf("%s");          // Sin límite → overflow
```

**Funciones seguras:**
```c
strncpy(dest, src, n);     // Copia máximo n caracteres
strncat(dest, src, n);     // Concatena máximo n
fgets(buffer, size, stdin); // Lee máximo size-1
snprintf(buf, size, ...);   // Printf seguro
```

**Vulnerabilidad común:** Si olvidest el '\0' al copiar, ¡lenguaje lee más allá!

**Relacionado:** [Buffer](#buffer), [Buffer Overflow](#buffer-overflow)

---

## T

### Type Confusion
**Definición:** Vulnerabilidad donde el código trata datos como tipo incorrecto.

**Ejemplo:**
```c
int buffer[10];           // 10 ints = 40 bytes
char *ptr = (char *)buffer;  // Trata como chars

ptr[50] = 'A';  // Escribe fuera de bounds!
```

**Ejemplo con malloc:**
```c
void *ptr = malloc(100);
int *int_ptr = (int *)ptr;

int_ptr[100]  // Lee 400 bytes (100 * 4)
              // Pero malloc solo reservó 100
```

**Impacto:** Out-of-bounds access, corrupción de memoria.

**Relacionado:** [Casting](#casting), [Buffer Overflow](#buffer-overflow)

---

## U

### Use-After-Free
**Definición:** Usar memoria después de haberla liberado.

**Ejemplo:**
```c
int *ptr = malloc(10);
free(ptr);
printf("%d\n", *ptr);   // ¡Error! ptr apunta a memoria libre
```

**Impacto:**
- Lectura: Datos impredecibles (puede ser garbage o datos de otro programa)
- Escritura: Corrupción
- Ejecución: Posible code execution

**En el contexto:**
```c
char *ptr = malloc(100);
char *copy = ptr;
free(ptr);
// ...
strcpy(copy, "datos");  // ¡Use-after-free!
```

**Detección en GDB:**
El SO reclama la memoria, accesar causa segmentation fault.

**Relacionado:** [Heap](#heap), [Double-Free](#d)

---

## V

### Virtual Address / Virtual Memory
**Definición:** Sistema de mapeo entre direcciones vistas por el programa (virtual) y físicas (RAM real).

**Concepto:**
```
Cada proceso cree que tiene 0x00000000 → 0xFFFFFFFF
Pero el SO mapea esto a RAM real física
Múltiples procesos pueden coexistir sin interferencias
```

**Tabla de páginas:**
```
Virtual 0x400000 → Física 0x1AF000
Virtual 0x401000 → Física 0x1B0000
Virtual 0x402000 → Física 0x1B1000
...
```

**Por qué importa:**
- ASLR funciona randomizando estos mapeos
- NX protege escribiendo en la tabla de páginas
- Exploits deben eludir este sistema

**Nota:** El MMU (Memory Management Unit) hace las traducciones, muy rápido.

**Relacionado:** [ASLR](#aslr), [Memory Layout](#memory-layout)

---

### Vulnerability
**Definición:** Defecto o debilidad en un programa que permite comportamiento no previsto.

**Diferencia crítica:**
```
Vulnerability = Defecto en el código (existe)
Exploit = Forma de explotar ese defecto (es activo)

Puedes tener una vulnerabilidad sin exploit público
```

**Ejemplos:**
```
Buffer overflow en strcpy
SQL injection en formulario
XSS en comentarios
Format string en printf
```

**Clasificación:**
- **Severidad:** Critical, High, Medium, Low
- **Tipo:** Memory, Logic, Crypto, etc.
- **CWE:** Common Weakness Enumeration (estándar)
- **CVSS:** Scoring system

**Ciclo de vida:**
```
Vulnerabilidad descubierta
    ↓
Vendedor notificado (responsibly)
    ↓
Patch desarrollado
    ↓
CVE asignado
    ↓
Público lo sabe
    ↓
Exploits públicos
```

**Relacionado:** [Exploit](#exploit)

---

## W

### Weak Symbol
**Definición:** Símbolo que puede ser sobrescrito por definiciones más fuertes.

**Uso:**
```c
// En libc:
void malloc_hook() __attribute__((weak));

// Tu programa:
void malloc_hook() {
    printf("malloc llamado!\n");
}
// Esta versión anula la weak
```

**Seguridad:** Puede ser riesgoso si no se usa cuidadosamente.

---

## X

### x86-64
**Definición:** Arquitectura de 64 bits de Intel/AMD (la más común).

**Registros principales:**
- `RAX`, `RBX`, `RCX`, `RDX` - Propósito general
- `RSI`, `RDI` - Índices
- `RSP` - Stack pointer
- `RBP` - Frame pointer
- `RIP` - Instruction pointer

**Convención de llamadas (x86-64 System V ABI):**
- Primer parámetro: `RDI`
- Segundo: `RSI`
- Tercero: `RDX`
- Cuarto: `RCX`
- Quinto: `R8`
- Sexto: `R9`
- Resto en stack

**Instrucciones comunes:**
```asm
mov   # Mover datos
add   # Sumar
sub   # Restar
call  # Llamar función
ret   # Retornar
jmp   # Salto incondicional
je    # Salto si igual
```

**Nota:** Hay x86 (32-bit) pero x86-64 es estándar hoy.

**Relacionado:** [Assembly](#assembly), [Instruction Pointer](#instruction-pointer--program-counter)

---

## Z

### Zero-Day
**Definición:** Vulnerabilidad desconocida por el vendedor, sin patch disponible.

**Línea de tiempo:**
```
Día 0: Vulnerabilidad existe, vendedor no sabe
Día N: Hacker la encuentra, la vende o la usa
Día M: Vendedor se entera, comienza patch
Día M+30: Patch se lanza (típicamente)

Período [Día 0 → Día M]: Vulnerable (zero-day)
```

**Valor en mercado negro:** Caro (vulnerabilidades desconocidas valen mucho).

**Defensa:** Parches de seguridad regulares, monitoreo de comportamiento anómalo.

---

## Índice Cruzado

### Por Categoría

**Arquitectura / Binarios:**
- [ELF](#elf), [Sección](#sección), [Segmento](#segmento), [Binary](#binary)
- [Assembly](#assembly), [Machine Code](#machine-code), [x86-64](#x86-64)

**Memoria:**
- [Stack](#stack), [Heap](#heap), [Virtual Address](#virtual-address--virtual-memory)
- [Memory Layout](#memory-layout), [Stack Frame](#stack-frame)

**Seguridad:**
- [Vulnerability](#vulnerability), [Exploit](#exploit), [Payload](#payload)
- [Buffer Overflow](#buffer-overflow), [Format String Vulnerability](#format-string-vulnerability)
- [ROP](#rop), [Shellcode](#shellcode)

**Protecciones:**
- [ASLR](#aslr), [NX Bit](#nx-bit), [Stack Canary](#stack-canary)

**Herramientas:**
- [GDB](#gdb), [Objdump](#objdump), [Readelf](#readelf)
- [GCC](#gcc), [Debugger](#debugger)

**Conceptos de C:**
- [Pointer](#pointer), [String](#string), [Casting](#casting)
- [Type Confusion](#type-confusion), [Use-After-Free](#use-after-free)

---

**Última actualización:** Mayo 2026  
**Palabras totales:** ~7,500  
**Estado:** ✅ Completo


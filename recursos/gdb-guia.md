# GDB - Guía Profunda de Debugging para Seguridad

## Índice
1. [Introducción a GDB](#introducción-a-gdb)
2. [Conceptos Fundamentales](#conceptos-fundamentales)
3. [Breakpoints Avanzados](#breakpoints-avanzados)
4. [Inspección de Memoria](#inspección-de-memoria)
5. [Stack Frames y Análisis](#stack-frames-y-análisis)
6. [Disassembly y Registros](#disassembly-y-registros)
7. [Scripting y Automatización](#scripting-y-automatización)
8. [Debugging de Vulnerabilidades](#debugging-de-vulnerabilidades)
9. [Troubleshooting Común](#troubleshooting-común)

---

## Introducción a GDB

GDB (GNU Debugger) es la herramienta más poderosa para entender qué hace tu código en tiempo de ejecución. En seguridad, es **fundamental** porque permite:

- Ver exactamente qué valores tienen las variables en cada momento
- Inspeccionar la memoria crudamente (sin abstracciones de C)
- Entender el flujo de ejecución a nivel de instrucción
- Detectar comportamientos inesperados (¡vulnerabilidades!)
- Analizar por qué un exploit funcionó

### Ejemplo: Un bug casi invisible

```c
#include <string.h>
#include <stdio.h>

int main() {
    char buffer[10];
    strcpy(buffer, "hola mundo");  // ¡Buffer overflow!
    printf("Buffer: %s\n", buffer);
    return 0;
}
```

Sin GDB: "segmentation fault" - no sabes qué pasó.
Con GDB: Ves exactamente cuándo se corrompió la memoria.

---

## Conceptos Fundamentales

### 1. Iniciando GDB Correctamente

```bash
# Compilar SIEMPRE con -g para incluir símbolos de debug
gcc -g -o programa programa.c

# Iniciar GDB
gdb ./programa

# O attach a un proceso ya corriendo
gdb ./programa <PID>
```

**¿Por qué -g es crítico?**

Sin -g:
```
(gdb) list
No symbol table is loaded. Use the "file" command.
```

Con -g:
```
(gdb) list
5    int main() {
6        char buffer[10];
7        strcpy(buffer, "hola mundo");
```

### 2. Los Tres Estados de GDB

```
┌─────────────────────────────────────────────┐
│ Inicio: Programa cargado pero no corriendo  │
│ (gdb) run                                   │
│          ↓                                   │
│ Corriendo: Ejecutándose normalmente         │
│ (ejecutar hasta breakpoint)                 │
│          ↓                                   │
│ Parado: En un breakpoint                    │
│ (gdb) next, print, info, etc.              │
└─────────────────────────────────────────────┘
```

### 3. Compilación para Debugging Efectivo

```bash
# MEJOR para debugging
gcc -g -O0 -o programa programa.c

# RAZONABLE (optimización mínima)
gcc -g -O1 -o programa programa.c

# PEOR para debugging (optimización agresiva)
gcc -g -O2 -o programa programa.c
gcc -g -O3 -o programa programa.c
```

**¿Por qué -O0?**

Con `-O2` o `-O3`, el compilador reordena instrucciones y elimina variables "innecesarias". En GDB:

```
(gdb) print miVariable
$1 = <optimized out>
```

---

## Breakpoints Avanzados

### 1. Breakpoints Simples

```bash
# Breakpoint en función
(gdb) break main
Breakpoint 1 at 0x1169

# Breakpoint en línea específica
(gdb) break programa.c:7
Breakpoint 2 at 0x1175: file programa.c, line 7.

# Breakpoint en dirección de memoria (para análisis binario)
(gdb) break *0x400500
Breakpoint 3 at 0x400500
```

### 2. Breakpoints Condicionales

```bash
# Pausar SOLO si una condición es verdadera
(gdb) break programa.c:7 if valor > 100
Breakpoint 1 at 0x1175: file programa.c, line 7.

# Ejemplo: Encontrar cuándo una variable se corrompe
(gdb) break programa.c:10 if strcmp(buffer, "esperado") != 0
```

**Caso de uso en seguridad:**
```c
// Búsqueda de vulnerabilidad
for (int i = 0; i < tamaño; i++) {
    procesar(entrada[i]);  // ¿Dónde se sale de rango?
}
```

```bash
(gdb) break programa.c:5 if i >= 100
```

### 3. Comandos Después de Breakpoint

```bash
# Ejecutar comandos automáticamente al parar
(gdb) break main
Breakpoint 1 at 0x1169

(gdb) commands 1
> print argc
> print argv[0]
> continue
> end
```

Ahora cada vez que se alcance breakpoint 1, se imprime argc y argv[0] automáticamente.

### 4. Breakpoints Temporales

```bash
# Se elimina automáticamente después de usarse una vez
(gdb) tbreak main
Temporary breakpoint 1 at 0x1169

# O convertir uno existente
(gdb) disable 1
(gdb) enable 1 tbreak
```

---

## Inspección de Memoria

### 1. Examinar Memoria Crudamente

```bash
# Formato: x/[N][F][U] <dirección>
# N = número de unidades
# F = formato (x=hex, d=decimal, s=string, i=instrucción)
# U = tamaño (b=byte, h=halfword, w=word, g=giant)

# Ver 16 bytes en hexadecimal a partir de un puntero
(gdb) x/16xb &buffer
0x7ffffffde4c0: 0x68    0x6f    0x6c    0x61    0x20    0x6d    0x75    0x6e
0x7ffffffde4c8: 0x64    0x6f    0x00    0x00    0x00    0x00    0x00    0x00
                 h      o      l      a      sp     m      u      n
                 d      o
```

### 2. Ver String en Memoria

```bash
# Un string en memoria
(gdb) x/s &buffer
0x7ffffffde4c0: "hola mundo"

# Múltiples strings consecutivos
(gdb) x/10s &buffer
```

### 3. Ver Instrucciones en Memoria

```bash
# 10 instrucciones a partir de main
(gdb) x/10i main
   0x400500 <main>:     push   %rbp
   0x400501 <main+1>:   mov    %rsp,%rbp
   0x400504 <main+4>:   sub    $0x10,%rsp
```

### 4. Mapear Región de Memoria Completa

```bash
# En un exploit: "¿Dónde está el shellcode en memoria?"
(gdb) info proc mappings
process 12345
Mapped address spaces:

	Start Addr   End Addr       Size     Offset objfile
	0x400000   0x401000     0x1000        0x0 /home/user/programa
	0x600000   0x601000     0x1000    0x1000 /home/user/programa
	0x601000   0x602000     0x1000    0x2000 /home/user/programa
	0x7ffff7dd8000 0x7ffff7dfd000 0x25000        0x0 /lib64/libc.so.6
	...
```

---

## Stack Frames y Análisis

### 1. Entender la Pila de Llamadas

Cuando paras en un breakpoint, ¿qué función te llamó? ¿Y esa función? GDB mantiene un "stack de frames".

```bash
(gdb) break proceso
(gdb) run
Breakpoint 1, proceso() at programa.c:15

# Ver toda la pila
(gdb) backtrace
#0  proceso() at programa.c:15
#1  0x00400516 in main() at programa.c:8
#2  0x00007ffff7a05f45 in __libc_start_main() from /lib64/libc.so.6
```

**Frame 0 = actual (proceso)**
**Frame 1 = quien llamó a proceso (main)**
**Frame 2 = quien llamó a main (libc)**

### 2. Inspeccionar Cada Frame

```bash
# Información del frame actual
(gdb) info frame
Stack level 0, frame at 0x7ffffffde4c0:
 rip = 0x400509 in proceso (programa.c:15); saved rip 0x400516
 called by frame at 0x7ffffffde4e0
 source language c.
 Arglist at 0x7ffffffde4b0, args: x=5, y=10
 Locals at 0x7ffffffde4b0, Previous frame's sp is 0x7ffffffde4c0
```

### 3. Navegar Entre Frames

```bash
# Ir al frame que llamó a este
(gdb) up
#1  0x00400516 in main() at programa.c:8

# Ver variables en ese frame
(gdb) print argc
$1 = 1
(gdb) print argv
$2 = (char **) 0x7ffffffde9e8

# Volver al frame original
(gdb) down
#0  proceso() at programa.c:15
```

### 4. Ver Argumentos de Función

```bash
# Argumentos automáticamente
(gdb) info args
x = 5
y = 10

# O manualmente
(gdb) print $rbp
$1 = (void *) 0x7ffffffde4c0
(gdb) x/2wd $rbp
0x7ffffffde4c0: 5
0x7ffffffde4c8: 10
```

---

## Disassembly y Registros

### 1. Ver el Código Assembly

```bash
# Disassemble una función completa
(gdb) disassemble main
Dump of assembler code for function main:
   0x0000000000400506 <+0>:     push   %rbp
   0x0000000000400507 <+1>:     mov    %rsp,%rbp
   0x000000000040050a <+4>:     sub    $0x10,%rsp
   0x000000000040050e <+8>:     mov    %edi,-0x4(%rbp)
   0x0000000000400511 <+11>:    mov    %rsi,-0x10(%rbp)
   0x0000000000400515 <+15>:    mov    $0x0,%eax
   0x000000000040051a <+20>:    pop    %rbp
   0x000000000040051b <+21>:    retq
End of assembler dump.
```

### 2. Interpretar los Registros

```bash
# Ver todos los registros
(gdb) info registers
rax            0x0                 0
rbx            0x0                 0
rcx            0x7ffff7dd8000      140737351955456
rdx            0x7ffffffde8a8      140737488348328
rsi            0x7ffffffde898      140737488348312
rdi            0x1                 1
rbp            0x7ffffffde4c0      0x7ffffffde4c0
rsp            0x7ffffffde4b0      0x7ffffffde4b0
r8             0x0                 0
r9             0x0                 0
r10            0x0                 0
r11            0x2222               8738
r12            0x400400            4195328
r13            0x0                 0
r14            0x0                 0
r15            0x0                 0
rip            0x400509            0x400509 <main+3>
```

**Registros importantes en x86-64:**
- `rax`: valor de retorno
- `rsp`: puntero del stack (tope actual)
- `rbp`: base del frame (inicio del frame actual)
- `rip`: instruction pointer (dónde estamos)

### 3. Seguir Paso a Paso a Nivel Assembly

```bash
# Ejecutar UNA instrucción
(gdb) stepi
0x000000000040050a in main ()

(gdb) stepi
0x000000000040050e in main ()

# Ver dónde estás
(gdb) disassemble $rip-10, $rip+10
```

### 4. Relacionar C con Assembly

```bash
# Ver C y assembly juntos (Intel syntax)
(gdb) set disassembly-flavor intel
(gdb) disassemble /m main
...
7    {
   0x0000000000400506 <+0>:     push   rbp
   0x0000000000400507 <+1>:     mov    rbp,rsp
   0x000000000040050a <+4>:     sub    rsp,0x10

8        char buffer[10];
9        strcpy(buffer, "hola");
   0x000000000040050e <+8>:     lea    rax,[rbp-0x10]
   0x0000000000400512 <+12>:    mov    rsi,0x400604
   0x0000000000400519 <+19>:    mov    rdi,rax
   0x000000000040051c <+22>:    call   0x400400 <strcpy@plt>
```

---

## Scripting y Automatización

### 1. Archivos de Comandos GDB

```bash
# Crear archivo de comandos (debug.gdb)
cat > debug.gdb << 'EOF'
break main
run
print argc
print argv
backtrace
continue
EOF

# Ejecutar
gdb -x debug.gdb ./programa
```

### 2. Automatizar Análisis de Memoria

```bash
# analizar.gdb - Examinar región de memoria específica
cat > analizar.gdb << 'EOF'
break main
run

# Ver la pila en el punto de entrada
x/32xw $rsp
x/10s $rsp

# Ver segmento de datos
info proc mappings
EOF

gdb -x analizar.gdb ./programa
```

### 3. Debugging No-Interactivo

```bash
# Ejecutar GDB sin necesidad de input interactivo
gdb -batch \
    -ex "break main" \
    -ex "run" \
    -ex "backtrace" \
    -ex "quit" \
    ./programa
```

Perfecto para scripts de automatización.

### 4. Logging de Sesión de Debug

```bash
# Grabar todo lo que pasa en GDB
gdb --args ./programa arg1 arg2

(gdb) set logging on
Copying output to gdb.txt and GDB output is logged.

(gdb) run
(gdb) backtrace
(gdb) quit

# Ahora tienes gdb.txt con todo el output
cat gdb.txt
```

---

## Debugging de Vulnerabilidades

### 1. Detectar Buffer Overflow

```c
// vulnerable.c
#include <string.h>
void copiar(char *entrada) {
    char buffer[10];
    strcpy(buffer, entrada);  // ¡Vulnerable!
}

int main(int argc, char **argv) {
    if (argc > 1)
        copiar(argv[1]);
    return 0;
}
```

Compilar y debuggear:

```bash
gcc -g -O0 -fno-stack-protector -z execstack -o vulnerable vulnerable.c
gdb ./vulnerable

(gdb) run "AAAAAAAAAA"  # 10 A's

# Con suerte, llegará aquí. Aumentar entrada:
(gdb) run "AAAAAAAAAAAAAAAAAAAAAA"  # 20 A's
Segmentation fault

# ¿Dónde se corrompió?
(gdb) run "AAAAAAAAAAAAAAAAAAAAAA"
(gdb) x/30x $rsp
# Verás que 'A' (0x41) está donde debería estar rip (return address)

(gdb) backtrace
#0  0x0000000000000000 in ?? ()
# Se intentó ejecutar desde 0x0 - ¡crash!
```

### 2. Inspeccionar Return Address

```bash
# Función vulnerable
(gdb) break copiar
(gdb) run "AAAAAAAAAAAAAAAAAAAAAA"
Breakpoint 1, copiar (entrada=0x7ffffffde8b3 "AAAAAAAAAAAAAAAAAAAAAA") at vulnerable.c:5

# ¿Dónde está el return address?
(gdb) info frame
... saved rip 0x41414141

# ¡Está corrompido! Debería ser una dirección legítima como 0x400516

# Ver exactamente dónde en memoria
(gdb) x/20x $rsp
0x7ffffffde4a0: 0x41414141  0x41414141  0x41414141  0x41414141
                ^ Este es el saved rip - corrupto
```

### 3. Localizar Exactamente el Offset

```bash
# ¿Cuántos bytes antes de sobrescribir RIP?
gcc -g -O0 -fno-stack-protector -o vulnerable vulnerable.c
gdb ./vulnerable

# Intentar con patrón único
(gdb) run "AAAABBBBCCCCDDDD"
Segmentation fault

(gdb) x $rip  # O ver backtrace
#0  0x44434443 in ?? ()

# 0x44434443 = DCCD (little-endian)
# "AAAABBBBCCCC" = 12 bytes
# Entonces buffer es 10, saved rbp es 8, total 18
# Controlamos RIP desde offset 16

# Verificar:
(gdb) run "AAAAAAAAAAAAAAAA_CONTROLADO"
# Si RIP se vuelve 0x5f4c4f52... hemos ganado el control
```

### 4. Exploración de Gadgets (ROP)

```bash
# Encontrar dónde está una instrucción útil
objdump -d vulnerable | grep "pop.*rdi"
0x400518 <main+12>:     5f                      pop    %rdi
0x400519 <main+13>:     c3                      retq

# En GDB, confirmar dónde está exactamente
(gdb) disassemble main
   0x0000000000400516 <+16>:    pop    %rdi
   0x0000000000400517 <+17>:    retq

# Usar esta dirección (0x400516) como gadget en exploit
```

---

## Troubleshooting Común

### 1. "No symbol table is loaded"

```bash
# Problema
gcc -o programa programa.c  # Sin -g
gdb programa
(gdb) list
No symbol table is loaded. Use the "file" command.

# Solución
gcc -g -o programa programa.c
gdb programa
```

### 2. "<optimized out>" - Variables Desaparecidas

```bash
# Problema
gcc -g -O2 -o programa programa.c
(gdb) print miVariable
$1 = <optimized out>

# Solución: Usar -O0
gcc -g -O0 -o programa programa.c
```

### 3. Debugging de Binarios Precompilados (Sin Símbolos)

```bash
# Si solo tienes el binario, sin código fuente
objdump -d binario | head -50  # Ver assembly
gdb binario
(gdb) disassemble main
(gdb) break *0x400500  # Breakpoint por dirección
(gdb) x/i $rip  # Ver instrucción actual
```

### 4. Core Dumps - Debugging Post-Mortem

```bash
# Permitir core dumps
ulimit -c unlimited

# Ejecutar programa (si crashea, crea core.XXXXX)
./programa

# Debuggear el crash
gdb ./programa core.12345

(gdb) backtrace  # Ver dónde se crasheó
(gdb) info frame
(gdb) print x  # Ver variables en ese momento
```

### 5. ASLR Interfiere

```bash
# ASLR (Address Space Layout Randomization) cambia direcciones
# Cada run, las direcciones son diferentes

# Desactivar ASLR para debugging consistente
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space

# En GDB (alternativa):
(gdb) set disable-randomization on

# Reactivar después (security)
echo 2 | sudo tee /proc/sys/kernel/randomize_va_space
```

---

## Workflow Práctico: Análisis de Vulnerabilidad

Imaginemos que sospechas de un buffer overflow en una función. Aquí está el workflow completo:

```bash
# 1. Compilar con símbolos y sin protecciones
gcc -g -O0 -fno-stack-protector -z execstack -o target target.c

# 2. Crear script de debug
cat > debug.gdb << 'EOF'
break funcion_vulnerable
run "AAAAAAAAAAAAAAAAAAAAAAAAAAAA"
backtrace
info frame
x/30x $rsp
x/30x $rbp
print nombreVariable
disassemble funcion_vulnerable
quit
EOF

# 3. Ejecutar
gdb -x debug.gdb ./target

# 4. Analizar output
# - ¿Se corrompió RIP?
# - ¿Dónde exactamente?
# - ¿Qué valores necesitamos para explotar?
```

---

## Resumen de Comandos Esenciales

| Comando | Propósito |
|---------|-----------|
| `run [args]` | Ejecutar programa con argumentos |
| `break main` | Poner breakpoint en main |
| `continue` | Reanudar ejecución |
| `next` | Siguiente línea C |
| `step` | Siguiente línea C (entra en funciones) |
| `stepi` | Siguiente instrucción assembly |
| `print var` | Ver valor de variable |
| `x/Nxb addr` | Ver N bytes en hexadecimal |
| `disassemble func` | Ver assembly de función |
| `backtrace` | Ver pila de llamadas |
| `info registers` | Ver valores de registros |
| `set disable-randomization on` | Desactivar ASLR |

Esta guía debería permitirte usar GDB como un experto en seguridad ofensiva.

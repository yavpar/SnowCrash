# Objdump - Guía Profunda de Análisis Binario

## Índice
1. [Introducción y Contexto](#introducción-y-contexto)
2. [Disassembly - El Corazón de Objdump](#disassembly---el-corazón-de-objdump)
3. [Lectura de Assembly x86-64](#lectura-de-assembly-x86-64)
4. [Secciones y Segmentos](#secciones-y-segmentos)
5. [Análisis de Tablas de Símbolos](#análisis-de-tablas-de-símbolos)
6. [Búsqueda de Gadgets y Patrones](#búsqueda-de-gadgets-y-patrones)
7. [Relaciones y Relocaciones](#relaciones-y-relocaciones)
8. [Casos Prácticos en Seguridad](#casos-prácticos-en-seguridad)

---

## Introducción y Contexto

Objdump es una **herramienta de análisis estático**. No ejecuta el binario; lo disecciona.

### ¿Por qué es fundamental en seguridad?

1. **Sin símbolos de debug**: Cuando el atacante no tiene acceso al código fuente
2. **Ingeniería inversa**: Entender qué hace un binario compilado
3. **Encontrar vulnerabilidades**: Localizar patrones peligrosos en assembly
4. **Búsqueda de gadgets**: Construir exploits ROP
5. **Análisis de protecciones**: Ver qué defensas implementó el compilador

### Diferencia entre Objdump y GDB

| Aspecto | Objdump | GDB |
|--------|---------|-----|
| Tipo | Análisis estático | Debugging dinámico |
| Requiere ejecución | No | Sí |
| Ver valores en runtime | No | Sí |
| Analizar sin ejecutar | Sí | No |
| Rendimiento | Muy rápido | Más lento |
| Ideal para | Entender estructura | Encontrar bugs |

---

## Disassembly - El Corazón de Objdump

### 1. Disassembly Básico

```bash
# Ver TODO el código ejecutable
objdump -d binario > disassembly.txt

# Análisis de 50 líneas de salida típico:
objdump -d ./hola_mundo | head -50

8048400 <_start>:
 8048400:       31 ed                   xor    %ebp,%ebp
 8048402:       5e                      pop    %rsi
 8048403:       89 e7                   mov    %esp,%edi
 8048405:       83 e4 f0                and    $0xfffffff0,%esp
 8048408:       50                      push   %rax
 8048409:       54                      push   %rsp
 804840a:       52                      push   %rdx
 804840b:       e8 f0 00 00 00          callq  8048500 <__libc_start_main@plt>
```

**Estructura de cada línea:**
```
<dirección>: <bytes> <instrucción>    <operandos>

8048400:     31 ed  xor              %ebp,%ebp
│            │      │                │
│            │      │                └─ Operandos (qué se opera)
│            │      └──────────────────── Instrucción (qué hacer)
│            └────────────────────────── Códigos de máquina (bytes crudos)
└─────────────────────────────────────── Dirección de memoria
```

### 2. Disassembly de Función Específica

```bash
# Solo una función
objdump -d binario | grep -A 30 "<nombreFuncion>"

# Ejemplo: función "main"
objdump -d ./binario | grep -A 50 "<main>:"

8048506 <main>:
 8048506:       55                      push   %rbp
 8048507:       48 89 e5                mov    %rsp,%rbp
 804850a:       48 83 ec 10             sub    $0x10,%rsp
 804850e:       bf 00 86 04 08          mov    $0x8048600,%edi
 8048513:       e8 e8 fe ff ff          callq  8048400 <puts@plt>
 8048518:       b8 00 00 00 00          mov    $0x0,%eax
 804851d:       c9                      leaveq
 804851e:       c3                      retq
```

### 3. Intel vs AT&T Syntax

Por defecto, objdump usa **AT&T syntax** (confuso). Usa **Intel syntax** (claro):

```bash
# Intel syntax (RECOMENDADO)
objdump -d -M intel binario | head -20

8048400 <_start>:
 8048400:       31 ed                   xor    ebp,ebp
 8048402:       5e                      pop    rsi
 8048403:       89 e7                   mov    edi,esp

# vs AT&T syntax (confuso)
8048400 <_start>:
 8048400:       31 ed                   xor    %ebp,%ebp
 8048402:       5e                      pop    %rsi
 8048403:       89 e7                   mov    %esp,%edi
```

Las diferencias:
- Intel: `mov dst, src` | AT&T: `mov src, dst`
- Intel: `$valor` | AT&T: `$valor` (igual)
- Intel: registros sin % | AT&T: registros con %

**Siempre usa Intel syntax** en análisis de seguridad.

---

## Lectura de Assembly x86-64

### 1. Registros Importantes

```
Propósito General (64 bits):
rax, rbx, rcx, rdx, rsi, rdi, r8-r15

Stack y Base:
rsp = registro puntero del stack (tope)
rbp = registro base del frame (inicio)

Instruction Pointer:
rip = dónde estamos en el código

Flags:
rflags = banderas de condiciones (ZF, CF, SF, etc.)
```

### 2. Instrucciones Comunes en Exploits

```assembly
; Movimiento de datos
mov rax, 0x400500        ; rax = 0x400500
mov rax, [rsp]           ; rax = valor en memoria [rsp]
mov [rsp], rax           ; memoria[rsp] = rax
lea rax, [rsp + 8]       ; rax = dirección (rsp + 8)

; Aritmética
add rax, 5               ; rax += 5
sub rsp, 16              ; rsp -= 16 (expandir stack)
xor rax, rax             ; rax = 0 (limpiar registro)

; Control de flujo
call 0x400500            ; push rip, saltar a 0x400500
ret                      ; pop rip (retornar)
jmp 0x400500             ; saltar sin retorno
je 0x400500              ; jump if equal (si ZF=1)
jne 0x400500             ; jump if not equal (si ZF=0)

; Stack
push rax                 ; rsp -= 8; memoria[rsp] = rax
pop rax                  ; rax = memoria[rsp]; rsp += 8
```

### 3. Convención de Llamadas (x86-64 System V ABI)

Cuando se llama una función:

```
Primer argumento:   rdi
Segundo:            rsi
Tercero:            rdx
Cuarto:             rcx
Quinto:             r8
Sexto:              r9
Séptimo+:           stack

Valor retornado:    rax (o rdx:rax para 128 bits)
```

Ejemplo:

```c
int suma(int a, int b, int c) {
    return a + b + c;
}

suma(10, 20, 30);
```

Assembly correspondiente:

```assembly
mov edi, 10              ; primer arg (a) en rdi
mov esi, 20              ; segundo arg (b) en rsi
mov edx, 30              ; tercer arg (c) en rdx
call suma

suma:
; Al entrar:
; rdi = 10 (a)
; rsi = 20 (b)
; rdx = 30 (c)
; rsp = dirección de retorno (automática)

    add edi, esi         ; a + b
    add edi, edx         ; + c
    mov eax, edi         ; valor retorno en rax
    ret                  ; pop rip
```

### 4. Stack Frame - Estructura en Memoria

```
Stack durante ejecución de función:

        ┌──────────────────┐
Altos   │ arg7 (si existe) │
        │ arg6             │
        │ arg5             │
        ├──────────────────┤
        │ return address   │ <─ saved rip (dónde volver)
        ├──────────────────┤
        │ saved rbp        │ <─ guardado por push %rbp
rbp ──→ ├──────────────────┤
        │ variable local 1 │
        │ variable local 2 │
Bajos   │ ...              │ <─ rsp (tope actual)
        └──────────────────┘
```

En assembly:

```assembly
push rbp                 ; guardar rbp anterior
mov rbp, rsp            ; rbp = nuevo frame
sub rsp, 16             ; reservar 16 bytes para locales

; Trabajo...

mov rsp, rbp            ; desmontar frame
pop rbp                 ; restaurar rbp anterior
ret                     ; volver
```

---

## Secciones y Segmentos

### 1. Ver Todas las Secciones

```bash
objdump -h binario

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .interp       0000001c  0000000000400238  0000000000400238  00000238  2**0
  1 .note.ABI-tag 00000020  0000000000400254  0000000000400254  00000254  2**2
  2 .note.gnu.build-id 00000024  0000000000400274  0000000000400274  00000274  2**2
  3 .gnu.hash     0000003c  0000000000400298  0000000000400298  00000298  2**3
  4 .dynsym       000000f0  00000000004002d8  00000000004002d8  000002d8  2**3
  5 .dynstr       000000a8  00000000004003c8  00000000004003c8  000003c8  2**0
  6 .gnu.version  00000014  0000000000400470  0000000000400470  00000470  2**1
  7 .gnu.version.r 00000030  0000000000400488  0000000000400488  00000488  2**3
  8 .rela.dyn     00000018  00000000004004b8  00000000004004b8  000004b8  2**3
  9 .rela.plt     00000060  00000000004004d0  00000000004004d0  000004d0  2**3
 10 .init         00000017  0000000000400530  0000000000400530  00000530  2**2
 11 .plt          00000050  0000000000400550  0000000000400550  00000550  2**4
 12 .plt.got      00000008  00000000004005a0  00000000004005a0  000005a0  2**3
 13 .text         00000182  00000000004005b0  00000000004005b0  000005b0  2**4
 14 .fini         0000000e  0000000000400732  0000000000400732  00000732  2**2
 15 .rodata       00000015  0000000000400748  0000000000400748  00000748  2**3
 16 .eh_frame_hdr 00000034  0000000000400760  0000000000400760  00000760  2**2
 17 .eh_frame     000000f0  0000000000400794  0000000000400794  00000794  2**3
 18 .init_array   00000008  0000000000600df0  0000000000600df0  00000df0  2**3
 19 .fini_array   00000008  0000000000600df8  0000000000600df8  00000df8  2**3
 20 .jcr          00000008  0000000000600e00  0000000000600e00  00000e00  2**3
 21 .dynamic      000001d0  0000000000600e08  0000000000600e08  00000e08  2**3
 22 .got          00000008  0000000000600fd0  0000000000600fd0  00000fd0  2**3
 23 .got.plt      00000038  0000000000600fd8  0000000000600fd8  00000fd8  2**3
 24 .data         00000010  0000000000601010  0000000000601010  00001010  2**3
 25 .bss          00000008  0000000000601020  0000000000601020  00001020  2**0
 26 .comment      0000002d  0000000000000000  0000000000000000  00001020  2**0
 27 .symtab       00000588  0000000000000000  0000000000000000  0000104d  2**3
 28 .strtab       000001a8  0000000000000000  0000000000000000  000015d5  2**0
 29 .shstrtab     0000010f  0000000000000000  0000000000000000  0000177d  2**0
```

### 2. Contenido de Secciones Específicas

```bash
# Ver strings en .rodata (solo lectura)
objdump -s -j .rodata binario

Contents of section .rodata:
 400748 01000200 48656c6c 6f2c2057 6f726c64  ....Hello, World
 400758 00                                   .

# Ver datos inicializados en .data
objdump -s -j .data binario

Contents of section .data:
 601010 08103000 00000000 78000000 00000000  ..0.....x.......
```

### 3. GOT (Global Offset Table) - Para Funciones Dinámicas

```bash
# Ver la tabla GOT (dónde están las funciones importadas)
objdump -R binario

DYNAMIC RELOCATION RECORDS
OFFSET           TYPE              SYMBOL
0000000000600fd8 R_X86_64_GLOB_DAT  __gmon_start__
0000000000600fe0 R_X86_64_JUMP_SLOT  puts
0000000000600fe8 R_X86_64_JUMP_SLOT  __libc_start_main
```

**Importante:** La dirección en GOT contiene la dirección real de la función.

En exploit: si puedes escribir en GOT, puedes redirigir funciones.

---

## Análisis de Tablas de Símbolos

### 1. Ver Todos los Símbolos

```bash
objdump -t binario

SYMBOL TABLE:
0000000000400530 l     F .init  00000017 call_gmon_start
0000000000400546 l     F .init  00000000 frame_dummy
0000000000600e00 l     O .jcr   00000000 __JCR_LIST__
0000000000400640 l     F .text  00000000 deregister_tm_clones
0000000000400660 l     F .text  00000000 register_tm_clones
0000000000400690 l     F .text  00000000 __do_global_dtors_aux
00000000004006b0 l     F .text  00000000 frame_dummy
00000000004006b5 l     F .text  00000000 main.cold
00000000004006db l     F .text  00000000 main.cold.1
0000000000600df8 l     O .fini_array 00000000 __do_global_dtors_aux_fini_array_entry
0000000000600e00 l     O .jcr   00000000 __JCR_LIST__
00000000004005b0  w    F .text  00000000 __libc_csu_fini
00000000004005d0 g     F .text  0000005f main
00000000004005f0 g     F .text  00000043 __libc_csu_init
0000000000601010 g     O .data  00000000 data_start
```

### 2. Filtrar Símbolos (Dinámicos)

```bash
# Solo símbolos que vienen de libc (funciones importadas)
objdump -T binario | grep -i puts

0000000000400550 w   DF *UND*  000000000 puts

# Explicación:
# 0000000000400550 = dirección en GOT
# w = weak (débil, puede ser reemplazado)
# DF = símbolo dinámico/función
# *UND* = indefinido (viene de afuera)
# puts = nombre
```

### 3. Símbolos Locales vs Globales

```bash
objdump -t binario | grep -E "F .text"

0000000000400530 l     F .text  00000017 _init          # l = local
0000000000400560 l     F .text  00000000 _start         # l = local
00000000004005d0 g     F .text  0000005f main          # g = global
```

- **l (local)**: No visible desde otros archivos
- **g (global)**: Visible desde otros archivos

---

## Búsqueda de Gadgets y Patrones

### 1. Encontrar Gadgets ROP

Un "gadget" es una secuencia de instrucciones que termina en `ret`, útil para construir exploits.

```bash
# Buscar todos los "pop rdi; ret"
objdump -d binario | grep -B1 -A0 "pop.*rdi" | grep "ret"

# Más específico: encontrar la dirección exacta
objdump -d binario | grep -B2 "retq$" | grep -A2 "pop.*rdi"

0000000000400550 <puts@plt>:
 400550:       ff 25 b2 0a 20 00       jmpq   *0x200ab2(%rip)
 400556:       68 00 00 00 00          pushq  $0x0
 40055b:       e9 e0 ff ff ff          jmpq   400540 <.plt>

# Ejemplo: encontrar "pop rdi" + "ret"
objdump -d vulnerable | grep -E "(pop|ret)" | head -20

400546:       5f                      pop    %rdi
400547:       c3                      retq
400548:       90                      nop
400549:       c3                      retq

# Gadget encontrado en 0x400546-0x400547
```

### 2. Buscar Patrones Peligrosos

```bash
# Buscar strcpy (vulnerable)
objdump -T binario | grep strcpy

# Buscar llamadas sin ASLR protection
objdump -d binario | grep -i "call.*main"

# Ver si hay `system` (útil para ROP)
objdump -T binario | grep system
```

### 3. Gadget Finder Automático (Script)

```bash
#!/bin/bash
# encontrar_gadgets.sh
binario=$1
patron=$2

echo "Buscando gadget: $patron"
objdump -d "$binario" | grep -B5 "retq$" | grep -A5 "$patron"
```

Uso:

```bash
chmod +x encontrar_gadgets.sh
./encontrar_gadgets.sh ./vulnerable "pop.*rdi"
```

---

## Relaciones y Relocaciones

### 1. Ver Relocaciones (Cómo se enlazan símbolos)

```bash
objdump -R binario

DYNAMIC RELOCATION RECORDS
OFFSET           TYPE              SYMBOL
0000000000600fd8 R_X86_64_GLOB_DAT  __gmon_start__
0000000000600fe0 R_X86_64_JUMP_SLOT  puts@GLIBC_2.2.5
0000000000600fe8 R_X86_64_JUMP_SLOT  __libc_start_main@GLIBC_2.2.5
```

Esto significa:
- En dirección 0x600fe0, necesita la dirección real de `puts`
- En dirección 0x600fe8, necesita la dirección real de `__libc_start_main`

**En exploit:** Si controlas 0x600fe0, redirige `puts` a tu código.

---

## Casos Prácticos en Seguridad

### 1. Análisis de Buffer Overflow

```c
// vulnerable.c
void copiar(char *entrada) {
    char buffer[10];
    strcpy(buffer, entrada);  // ¡Vulnerable!
}
```

```bash
# Compilar sin protecciones para ver claramente
gcc -fno-stack-protector -z execstack -o vulnerable vulnerable.c

# Disassemble la función vulnerable
objdump -d vulnerable -M intel | grep -A 20 "<copiar>:"

0000000000400556 <copiar>:
 400556:   55                      push   rbp
 400557:   48 89 e5                mov    rbp,rsp
 40055a:   48 83 ec 10             sub    rsp,0x10
 40055e:   48 89 45 f8             mov    QWORD PTR [rbp-0x8],rdi
 400562:   48 8b 45 f8             mov    rax,[rbp-0x8]
 400566:   48 8b 15 b3 0a 20 00    mov    rdx,[rip+0x200ab3]
 40056d:   48 89 c6                mov    rsi,rax
 400570:   bf e0 86 04 00          mov    edi,0x4086e0
 400575:   e8 c6 fe ff ff          call   400440 <strcpy@plt>
 40057a:   90                      nop
 40057b:   c9                      leaveq
 40057c:   c3                      retq
```

Análisis:
- Línea 400556-400557: Guardar frame anterior
- Línea 40055a: **sub rsp,0x10** = reservar 16 bytes (10 para buffer + 6 para alineación)
- Línea 400575: Llamada a strcpy (sin validación de tamaño)
- Línea 40057b-40057c: Retorno (¡aquí se corrompe si hay overflow!)

### 2. Encontrar Direcciones de Funciones Importantes

```bash
# Dónde está main
objdump -t vulnerable | grep main
00000000004005d0 g     F .text  0000005f main

# Dónde está system (si está enlazada)
objdump -t vulnerable | grep system
# Si no aparece, está en libc (dirección dinámica)

# PLT entries (para funciones importadas)
objdump -d vulnerable | grep "<puts@plt>:"
0000000000400430 <puts@plt>:
 400430:   ff 25 02 0a 20 00       jmpq   *0x200a02(%rip)
```

### 3. Análisis Anti-Debug

Algunos binarios tienen protecciones:

```bash
# Buscar ptrace (detecta debugging)
objdump -t binario | grep ptrace

# Buscar anti-ASLR
objdump -d binario | grep -i "randomize"

# Ver flags de seguridad compilados
readelf -l binario | grep -i "stack\|relro"
```

### 4. Extracción de Shellcode

Si encuentras un payload ya en el binario:

```bash
# Ver secciones
objdump -h binario | grep -i ".data"

# Extraer bytes crudos de una sección
objdump -s -j .data binario | hexdump -C

# Convertir a formato útil
objdump -s -j .data binario | xxd
```

---

## Workflow Práctico: Análisis Completo

Tienes un binario sospechoso. Aquí está el plan:

```bash
# 1. Ver estructura global
objdump -h binario
objdump -t binario | head -20

# 2. Ver funciones
objdump -d binario | grep "^[0-9a-f]*<.*>:"

# 3. Encontrar función principal
objdump -d binario -M intel | grep -A 50 "<main>:"

# 4. Buscar vulnerabilidades típicas
objdump -T binario | grep -E "strcpy|strcat|sprintf"

# 5. Buscar gadgets útiles
objdump -d binario | grep -B2 "retq$" | grep -A2 "pop"

# 6. Ver strings en el código
objdump -s -j .rodata binario | hexdump -C

# 7. Analizar tabla GOT (si es dinámico)
objdump -R binario
```

---

## Resumen de Comandos Esenciales

| Comando | Propósito |
|---------|-----------|
| `objdump -d binario` | Disassemble todo |
| `objdump -d -M intel binario` | Con Intel syntax |
| `objdump -h binario` | Ver secciones |
| `objdump -s binario` | Ver contenido de secciones |
| `objdump -t binario` | Ver tabla de símbolos |
| `objdump -T binario` | Símbolos dinámicos |
| `objdump -R binario` | Relocaciones |
| `objdump -x binario` | Todo (headers, símbolos, etc.) |

Con estas herramientas y conocimientos, puedes analizar cualquier binario a nivel de máquina.

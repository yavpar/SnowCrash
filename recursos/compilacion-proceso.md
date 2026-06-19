# Compilación: El Proceso Underground

## Introducción: GCC no es una sola herramienta

Cuando ejecutas:

```bash
gcc programa.c -o programa
```

No estás ejecutando un solo programa. GCC es un **driver** que orquesta 4 herramientas distintas en secuencia:

```
programa.c
    │
    ▼
[1] cpp      → preprocesador     → produce: programa.i
    │
    ▼
[2] cc1      → compilador        → produce: programa.s
    │
    ▼
[3] as       → ensamblador       → produce: programa.o
    │
    ▼
[4] ld       → linker            → produce: programa (ejecutable ELF)
```

Cada etapa transforma el archivo de una forma. Puedes detener GCC en cualquier etapa y ver el resultado intermedio.

---

## Etapa 1: El Preprocesador (cpp)

### ¿Qué es?

El preprocesador **no entiende C**. Es un motor de sustitución de texto puro. Su trabajo es preparar el código fuente antes de que el compilador real lo vea.

Opera sobre **directivas**: líneas que comienzan con `#`.

### ¿Qué hace con `#include`?

```c
#include <stdio.h>
```

El preprocesador localiza el archivo `stdio.h` en el sistema (normalmente en `/usr/include/stdio.h`), y **copia literalmente su contenido completo** en ese punto del archivo.

`stdio.h` a su vez tiene sus propios `#include`, que también se expanden. Y esos tienen los suyos. El resultado final puede tener miles de líneas donde tú tenías una.

Ejemplo real:

```bash
# Contar líneas del archivo original
wc -l hola.c
# → 5 líneas

# Ejecutar solo el preprocesador
gcc -E hola.c -o hola.i

# Contar líneas del resultado
wc -l hola.i
# → ~800 líneas (todo el contenido de stdio.h y sus dependencias)
```

### ¿Qué hace con `#define`?

```c
#define TAMAÑO 100
char buffer[TAMAÑO];
```

El preprocesador reemplaza cada ocurrencia de `TAMAÑO` por `100` en el texto. El compilador nunca ve `TAMAÑO`, solo ve `100`.

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))
int resultado = MAX(x, y);
```

Se convierte literalmente en:

```c
int resultado = ((x) > (y) ? (x) : (y));
```

Es sustitución de texto. No hay verificación de tipos, no hay lógica. Solo texto.

### ¿Qué hace con `#ifdef` / `#ifndef`?

```c
#ifdef DEBUG
    printf("Valor de x: %d\n", x);
#endif
```

Si `DEBUG` está definido (con `-DDEBUG` al compilar), incluye el bloque. Si no, lo elimina completamente. El compilador nunca verá ese `printf`.

Esto es cómo las librerías incluyen código diferente según el sistema operativo:

```c
#ifdef __linux__
    // código para Linux
#elif defined(__APPLE__)
    // código para macOS
#endif
```

### ¿Qué elimina?

Los comentarios. El preprocesador elimina todos los comentarios del código. El compilador nunca los ve.

### Cómo ver el resultado

```bash
gcc -E programa.c -o programa.i
cat programa.i
```

El archivo `.i` es C puro sin directivas `#`, listo para el compilador.

---

## Etapa 2: El Compilador (cc1)

### ¿Qué es cc1?

`cc1` es el compilador real de GCC. Recibe el `.i` (C puro) y produce `.s` (assembly).

Esta es la etapa más compleja. cc1 realiza varias fases internas:

### Fase 2a: Análisis Léxico (tokenización)

El compilador lee el texto del `.i` y lo convierte en **tokens**: unidades mínimas con significado.

```c
int x = 5 + 3;
```

Se convierte en la secuencia de tokens:

```
[keyword: int] [identifier: x] [operator: =] [integer: 5]
[operator: +] [integer: 3] [punctuation: ;]
```

Ya no es texto. Es una lista de objetos con tipo y valor.

### Fase 2b: Análisis Sintáctico (parsing)

Los tokens se organizan en un **AST** (Abstract Syntax Tree - Árbol Sintáctico Abstracto).

```c
int x = 5 + 3;
```

El AST se ve conceptualmente así:

```
AssignDeclaration
├── type: int
├── name: x
└── value: BinaryOperation
              ├── operator: +
              ├── left: IntegerLiteral(5)
              └── right: IntegerLiteral(3)
```

Esta estructura de árbol representa la lógica del programa de forma que el compilador puede manipularla.

### Fase 2c: Análisis Semántico

El compilador verifica que el código tiene sentido:

- ¿Está `x` declarada antes de usarse?
- ¿Los tipos son compatibles? (`int x = "hola"` → error)
- ¿La función llamada existe?
- ¿El número de argumentos es correcto?

Si algo no tiene sentido semántico, aquí se genera el error de compilación.

### Fase 2d: Generación de código intermedio

El AST se convierte en una representación intermedia (IR) independiente de la arquitectura. GCC usa su propio IR llamado GIMPLE y luego RTL.

Esto permite que GCC soporte múltiples arquitecturas (x86, ARM, RISC-V) usando el mismo frontend.

### Fase 2e: Optimizaciones

Con `-O0`: sin optimizaciones. El código assembly es directo y predecible.
Con `-O2`: el compilador puede:
- Eliminar variables que nunca se usan
- Desenrollar loops
- Hacer inlining de funciones pequeñas
- Reordenar instrucciones para aprovechar el pipeline del CPU

Esto explica por qué con `-O2` GDB muestra `<optimized out>` para ciertas variables: fueron eliminadas.

### Fase 2f: Generación de Assembly

Finalmente, el IR se convierte en instrucciones assembly específicas para la arquitectura objetivo (x86-64 en tu caso).

```c
int suma(int a, int b) {
    return a + b;
}
```

Se convierte en:

```asm
suma:
    push    rbp
    mov     rbp, rsp
    mov     DWORD PTR [rbp-4], edi   ; guardar argumento a
    mov     DWORD PTR [rbp-8], esi   ; guardar argumento b
    mov     edx, DWORD PTR [rbp-4]   ; edx = a
    mov     eax, DWORD PTR [rbp-8]   ; eax = b
    add     eax, edx                  ; eax = a + b
    pop     rbp
    ret
```

### Cómo ver el resultado

```bash
gcc -S -O0 programa.c -o programa.s
cat programa.s
```

---

## Etapa 3: El Ensamblador (as)

### ¿Qué hace?

`as` (GNU Assembler) convierte el archivo `.s` (texto assembly) en un archivo `.o` (objeto binario).

Cada instrucción assembly tiene un **opcode**: un número que el CPU entiende.

```asm
push rbp      →   55
mov rbp, rsp  →   48 89 e5
add eax, edx  →   01 d0
ret           →   c3
```

El archivo `.o` contiene estos bytes directamente.

### ¿Qué es un archivo objeto (.o)?

Un archivo `.o` es binario ejecutable **incompleto**. Tiene el código de máquina de tus funciones, pero con **huecos** donde deberían ir las direcciones de funciones externas.

Por ejemplo, si tu código llama a `printf`, el `.o` tiene:

```
call  0x00000000   ← dirección desconocida (hueco)
```

El ensamblador no sabe dónde estará `printf` en memoria. Eso lo resolverá el linker.

### Símbolos

El archivo `.o` tiene una **tabla de símbolos**: un directorio de nombres y sus direcciones (o la indicación de que son externos y aún desconocidos).

```bash
nm programa.o
```

Output típico:

```
0000000000000000 T suma       ← T = está en .text, dirección 0x0 (relativa)
                 U printf     ← U = undefined (externo, sin resolver)
```

### Cómo ver el resultado

```bash
gcc -c programa.c -o programa.o

# Ver tabla de símbolos
nm programa.o

# Ver el código de máquina
objdump -d programa.o

# Ver en hexadecimal crudo
hexdump -C programa.o | head -30
```

---

## Etapa 4: El Linker (ld)

### ¿Qué hace?

El linker es quien crea el ejecutable final. Sus responsabilidades:

1. **Recopila** todos los archivos `.o` (los tuyos + los de librerías)
2. **Resuelve símbolos**: encuentra dónde está `printf` (en libc.so)
3. **Asigna direcciones** finales a cada función y variable
4. **Rellena los huecos**: reemplaza los `0x00000000` con las direcciones reales
5. **Genera el ELF**: crea el archivo ejecutable con el formato correcto

### Librerías estáticas vs dinámicas

**Librería estática (.a):**
- Es un archivo que contiene múltiples `.o`
- El linker copia el código de la librería dentro de tu ejecutable
- El ejecutable es autónomo (no depende de archivos externos)
- Resultado: ejecutable más grande

**Librería dinámica (.so):**
- El código de la librería NO se copia en tu ejecutable
- El linker solo anota "este programa necesita libc.so.6"
- En tiempo de ejecución, el loader carga la librería
- Resultado: ejecutable más pequeño, pero depende de .so en el sistema

```bash
# Ver qué librerías dinámicas necesita tu ejecutable
ldd programa

# Output típico:
#   linux-vdso.so.1 (0x00007fff...)
#   libc.so.6 => /lib64/libc.so.6 (0x00007f...)
#   /lib64/ld-linux-x86-64.so.2 (0x00007f...)
```

### Relocation: Cómo rellena los huecos

El archivo `.o` tiene una tabla de **relocations**: lista de todos los lugares donde hay huecos y qué símbolo debe ir en cada uno.

```bash
objdump -r programa.o

# Output:
# RELOCATION RECORDS FOR [.text]:
# OFFSET           TYPE              VALUE
# 000000000000001a R_X86_64_PLT32    printf-0x4
```

Esto dice: "en el offset 0x1a de .text, pon la dirección de printf".

El linker lee esto, encuentra la dirección de `printf` en libc, y rellena el valor correcto.

### PLT y GOT (para funciones dinámicas)

Cuando el linker encuentra una función de librería dinámica (como `printf`), no puede poner su dirección directamente porque la librería se carga en una dirección aleatoria en tiempo de ejecución (ASLR).

En su lugar, usa dos estructuras:

**PLT (Procedure Linkage Table):**
Un pequeño stub de código en tu ejecutable. Cuando llamas a `printf`, realmente saltas a `printf@plt`.

**GOT (Global Offset Table):**
Una tabla de punteros. `printf@plt` lee la dirección real de `printf` desde la GOT.

La primera vez que se llama a `printf`, el loader resuelve la dirección real y la escribe en la GOT. Las llamadas siguientes son directas.

```bash
# Ver la PLT
objdump -d programa | grep -A 4 "<printf@plt>"

# Ver la GOT
objdump -R programa
```

### El ELF resultante

```bash
# Ver estructura general
readelf -h programa

# Ver todas las secciones
readelf -S programa

# Ver segmentos (cómo se carga en memoria)
readelf -l programa
```

---

## El Formato ELF

ELF (Executable and Linkable Format) es el formato de los binarios en Linux.

### Estructura del archivo ELF

```
Archivo ELF en disco:

┌─────────────────────────────┐
│ ELF Header                  │ ← magia, arquitectura, punto de entrada
├─────────────────────────────┤
│ Program Headers             │ ← cómo cargar en memoria (segmentos)
├─────────────────────────────┤
│ .text                       │ ← código ejecutable (instrucciones)
│ .rodata                     │ ← strings literales, constantes
│ .data                       │ ← variables globales inicializadas
│ .bss                        │ ← variables globales no inicializadas
│ .plt                        │ ← stubs para funciones dinámicas
│ .got                        │ ← tabla de punteros a funciones externas
│ .symtab                     │ ← tabla de símbolos (debug)
│ .strtab                     │ ← nombres de los símbolos
│ .dynamic                    │ ← info para el loader dinámico
├─────────────────────────────┤
│ Section Headers             │ ← índice de secciones
└─────────────────────────────┘
```

### ¿Por qué existen estas secciones?

**.text:**
Contiene el código compilado (los opcodes). Es **solo lectura** y **ejecutable**. Si intentas escribir en .text, el kernel lanza una falla de segmentación.

**.rodata (read-only data):**
Strings literales y constantes. Cuando escribes `printf("Hola\n")`, el string `"Hola\n"` vive aquí. También es solo lectura.

**.data:**
Variables globales con valor inicial:
```c
int contador = 5;  // va a .data (tiene valor inicial)
```

**.bss (Block Started by Symbol):**
Variables globales sin valor inicial. No ocupa espacio real en el archivo ELF (solo se anota el tamaño). El kernel las inicializa a cero al cargar el proceso.
```c
int contador;      // va a .bss (sin valor inicial, el kernel pone 0)
```

**.plt y .got:**
Mecanismo para llamar funciones de librerías dinámicas (explicado arriba).

---

## Resumen: Qué contiene cada archivo

| Archivo | Tipo | Contiene |
|---------|------|----------|
| `.c` | texto | código fuente C |
| `.i` | texto | C expandido (sin `#`) |
| `.s` | texto | assembly |
| `.o` | binario | código máquina incompleto (con huecos) |
| ejecutable | binario | código máquina completo (ELF) |

---

## Lecturas Relacionadas

- `recursos/compilacion-ejemplo.md` - Ver cada etapa con comandos reales
- `01-teoria/leccion_02.md` - ELF format y secciones
- `01-teoria/leccion_05.md` - Assembly y registros

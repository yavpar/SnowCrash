# 📚 Lección 02: Binarios, Archivos ELF y Memoria

## 🎯 Objetivos de Aprendizaje

Al terminar esta lección, entenderás:

- ✅ Qué es un archivo ELF y por qué es importante
- ✅ La estructura interna de un binario ELF
- ✅ Las diferencias entre código, datos y símbolos
- ✅ Cómo se organiza la memoria cuando un programa corre
- ✅ Las secciones de un binario: .text, .data, .bss, etc.
- ✅ Cómo el SO carga un ELF en memoria
- ✅ La diferencia entre dirección de archivo y dirección de memoria

**Tiempo estimado:** 60-75 minutos

---

## 📖 Introducción

En la Lección 01 aprendimos conceptos generales de seguridad.

En el Ejercicio 01 compilamos un programa C.

Ahora necesitas entender **qué pasa internamente** cuando ese programa se compila.

**Pregunta clave:** Cuando ejecutas un binario, ¿cómo sabe el SO qué hacer?

La respuesta está en el **formato ELF**.

---

## 1️⃣ ¿QUÉ ES UN ARCHIVO ELF?

### Definición

**ELF = Executable and Linkable Format**

Es el **formato estándar** para binarios en Linux (y otros sistemas Unix).

Piensa en ELF como un **contenedor** que organiza todo lo que un binario necesita.

### Un Poco de Historia

```
En los 80s, diferentes sistemas Unix tenían diferentes formatos:
  • BSD → a.out
  • System V → COFF
  • Etc...

Problema: No había estandarización.

Solución (1991): Nace ELF
  ✓ Estándar para todos los sistemas Unix
  ✓ Flexible y extensible
  ✓ Se usa TODAVÍA hoy en día
```

### ¿Por Qué Importa?

Porque en seguridad ofensiva, **todos los binarios Linux son ELF**.

Entender ELF significa:

- ✅ Saber dónde vive el código
- ✅ Saber dónde viven los datos
- ✅ Identificar vulnerabilidades
- ✅ Debuggear programas
- ✅ Crear exploits

---

## 2️⃣ ESTRUCTURA DE UN ARCHIVO ELF

### Visual del Contenedor ELF

```
ARCHIVO ELF:
┌─────────────────────────────────────┐
│ ELF Header                          │  ← Metadata del archivo
│ (Tipo, arquitectura, puntos entrada)│
├─────────────────────────────────────┤
│ Program Headers                     │  ← Cómo cargar en memoria
│ (Segmentos para el OS)              │
├─────────────────────────────────────┤
│ .text (Código)                      │
│ .data (Datos inicializados)         │
│ .bss  (Datos no inicializados)      │
│ .rodata (Datos de solo lectura)     │
│ (... más secciones ...)             │
├─────────────────────────────────────┤
│ Section Headers                     │  ← Metadata de secciones
│ (Info para el linker y debuggers)   │
├─────────────────────────────────────┤
│ Symbol Table                        │  ← Nombres de funciones/variables
│ String Table                        │  ← Strings
│ Relocation Info                     │  ← Para parchear direcciones
└─────────────────────────────────────┘
```

Ahora vamos a explicar cada parte.

---

## 3️⃣ ELF HEADER (Cabecera)

### ¿Qué es?

La **cabecera ELF** es lo primero en el archivo. Contiene metadata importante.

### Contenido Básico

```
ELF Header:
├─ Magic Number (0x7f 'E' 'L' 'F')   ← Identifica como ELF
├─ Clase (32-bit o 64-bit)
├─ Endianness (little-endian, big-endian)
├─ Versión de ELF
├─ Tipo de archivo (ejecutable, librería, etc)
├─ Arquitectura (x86, x86_64, ARM, etc)
├─ Punto de entrada (donde comienza la ejecución)
├─ Offset del Program Header
├─ Offset del Section Header
└─ Números de entradas
```

### Ejemplo Real

Cuando ejecutas `file` en un binario:

```bash
$ file mi_programa
mi_programa: ELF 64-bit LSB executable, x86-64, ...
```

Eso es información del **ELF Header**.

```
ELF           → Formato
64-bit        → Clase (64-bit)
LSB           → Little-endian (byte order)
executable    → Tipo (ejecutable)
x86-64        → Arquitectura
```

---

## 4️⃣ PROGRAM HEADERS Y SEGMENTOS

### ¿Qué es un Segmento?

Un **segmento** es una **vista de alto nivel** del binario para el OS.

Dice: "Cuando carges este programa en memoria, necesitas..."

### Los Segmentos Principales

```
SEGMENTOS (Lo que ve el OS):

1. LOAD Segment 1 (Readable, Executable)
   ├─ Carga en memoria
   ├─ Dirección base: 0x400000
   ├─ Tamaño en disco: 2048 bytes
   ├─ Tamaño en memoria: 2048 bytes
   └─ Contiene: .text (código)

2. LOAD Segment 2 (Readable, Writable)
   ├─ Carga en memoria
   ├─ Dirección base: 0x600000
   ├─ Tamaño en disco: 512 bytes
   ├─ Tamaño en memoria: 1024 bytes
   └─ Contiene: .data y .bss

3. DYNAMIC (Readable)
   ├─ Información de librerías dinámicas
   └─ Dirección de la PLT (Procedure Linkage Table)
```

### ¿Por Qué Dos Vistas (Secciones vs Segmentos)?

**Secciones:**
- Vista del **linker** (herramienta que arma el binario)
- Granularidad fina (.text, .data, .bss)
- Usadas para debugging y análisis

**Segmentos:**
- Vista del **SO** (kernel que carga el programa)
- Granularidad gruesa (bloques cargables)
- Usados para cargar en memoria

Un **segmento puede contener múltiples secciones**.

---

## 5️⃣ SECCIONES (Sections)

### ¿Qué es una Sección?

Una **sección** es un **bloque lógico** del binario con propósito específico.

### Las Secciones Más Importantes

#### 1. `.text` (Código del Programa)

```
.text:
├─ Contiene: El código ejecutable
├─ Permisos: Read + Execute (RX)
├─ Tamaño: Varía (típicamente KB-MB)
└─ Ejemplo: Tus funciones main(), printf(), etc.
```

**Analógía:** Es el "cerebro" del programa. Las instrucciones del procesador.

#### 2. `.data` (Datos Inicializados)

```
.data:
├─ Contiene: Variables globales inicializadas
├─ Permisos: Read + Write (RW)
├─ Ejemplo:
│   int contador = 42;           ← Aquí va
│   char nombre[] = "Juan";      ← Aquí va
│   int no_inicializado;         ← NO aquí (.bss)
└─ Tamaño: Depende de cuántas variables tengas
```

**Analógía:** Son "datos preparados". Cuando el programa corre, esos valores ya están listos.

#### 3. `.bss` (Datos No Inicializados)

```
.bss:
├─ Contiene: Variables globales SIN inicializar
├─ Permisos: Read + Write (RW)
├─ Ejemplo:
│   int contador;                ← Aquí va
│   static int buffer[1000];     ← Aquí va
└─ Truco: NO ocupa espacio en disco (se inicializa a 0 en runtime)
```

**Diferencia Importante:**

```
DISCO:
┌──────────────┐
│ .text        │  1000 bytes
│ .data        │   500 bytes
│ .bss         │   0 bytes!  ← No ocupa espacio en disco
└──────────────┘
Total: 1500 bytes en disco

MEMORIA (en runtime):
┌──────────────┐
│ .text        │  1000 bytes
│ .data        │   500 bytes
│ .bss         │  10000 bytes ← Aquí SÍ se reserva
└──────────────┘
Total: 11500 bytes en RAM
```

¿Por qué? Porque `.bss` es solo "espacio reservado a cero". No necesita almacenarse en disco.

#### 4. `.rodata` (Read-Only Data)

```
.rodata:
├─ Contiene: Datos de solo lectura
├─ Permisos: Read (R)
├─ Ejemplo:
│   const int MAX = 100;         ← Aquí va
│   "Hola, mundo"                ← String literal aquí va
└─ Uso: Protege constantes de modificación accidental
```

#### 5. `.symtab` (Tabla de Símbolos)

```
.symtab:
├─ Contiene: Nombres de funciones y variables
├─ Ejemplo:
│   main       → Dirección 0x401000
│   printf     → Dirección 0x401100 (referencia externa)
│   contador   → Dirección 0x601000
└─ Uso: Para debugging y análisis
```

#### 6. `.strtab` (Tabla de Strings)

```
.strtab:
├─ Contiene: Strings usados en todo el binario
├─ Ejemplo: Nombres de funciones, mensajes de error
└─ Uso: Comparte memoria para strings duplicados
```

---

## 6️⃣ CÓMO EL SO CARGA UN ELF

### Paso a Paso

```
1. Usuario ejecuta: ./mi_programa

2. SO lee el ELF Header
   ├─ ¿Es un binario válido?
   ├─ ¿Es x86-64?
   ├─ ¿Es ejecutable?
   └─ ¿Cuál es el punto de entrada?

3. SO crea un nuevo proceso
   ├─ Asigna memoria virtual
   ├─ Inicializa registros
   └─ Configura la pila (stack)

4. SO lee los Program Headers
   ├─ "Carga el segmento LOAD 1 en 0x400000"
   ├─ "Carga el segmento LOAD 2 en 0x600000"
   ├─ "Este segmento es READ+EXECUTE"
   └─ "Este otro es READ+WRITE"

5. SO "mapea" el archivo en memoria
   ├─ .text → 0x400000 (READ+EXECUTE)
   ├─ .data → 0x600000 (READ+WRITE)
   └─ .bss → 0x601000 (READ+WRITE, sin contenido)

6. SO salta al punto de entrada
   └─ Comienza a ejecutar instrucciones desde ahí
```

### Memory Layout Resultante

```
MEMORIA DEL PROCESO (después de cargar):

┌─────────────────────────────────────┐
│ Kernel space                        │  ← Inaccesible para el programa
├─────────────────────────────────────┤
│ Stack                               │  ← Variables locales, parámetros
│ (crece hacia abajo)                 │
│                                     │
├─────────────────────────────────────┤
│ Heap                                │  ← Memoria dinámica (malloc)
│ (crece hacia arriba)                │
│                                     │
├─────────────────────────────────────┤
│ .bss                                │  ← Datos no inicializados
├─────────────────────────────────────┤
│ .data                               │  ← Datos inicializados
├─────────────────────────────────────┤
│ .rodata                             │  ← Datos de solo lectura
├─────────────────────────────────────┤
│ .text                               │  ← Código
├─────────────────────────────────────┤
│ Program Headers / Headers           │
├─────────────────────────────────────┤
│ 0x0 (No asignado)                   │
└─────────────────────────────────────┘
```

---

## 7️⃣ DIRECCIÓN DE ARCHIVO VS DIRECCIÓN DE MEMORIA

### Concepto Crítico

**¡Estas NO son lo mismo!**

```
ARCHIVO ELF (en disco):
┌──────────────────────────┐
│ ELF Header               │  Offset 0x0
├──────────────────────────┤
│ Program Headers          │  Offset 0x40
├──────────────────────────┤
│ .text                    │  Offset 0x1000
│ "int main() { ... }"     │
│ Tamaño: 2048 bytes       │
├──────────────────────────┤
│ .data                    │  Offset 0x3000
└──────────────────────────┘

MEMORIA DEL PROCESO (en RAM):
┌──────────────────────────┐
│ .text                    │  Dirección 0x400000
│ "int main() { ... }"     │
│ Tamaño: 2048 bytes       │
├──────────────────────────┤
│ .data                    │  Dirección 0x601000
└──────────────────────────┘
```

### ¿Por Qué La Diferencia?

**En el archivo:**
- Offset 0x1000 = Posición en el archivo (en disco)
- Es relativa al inicio del archivo

**En la memoria:**
- Dirección 0x400000 = Dirección virtual (en RAM)
- El OS decide dónde cargar

### Relación

```
Dirección en memoria = Base del segmento + (Offset en archivo)

Ejemplo:
  Base del segmento = 0x400000
  Offset en archivo = 0x1000
  ─────────────────────────────
  Dirección en memoria = 0x400000 + 0x1000 = 0x401000
```

---

## 8️⃣ ASLR (Address Space Layout Randomization)

### ¿Qué es?

**ASLR = Randomización de direcciones**

Cada vez que ejecutas un programa, las direcciones **cambian**.

### Ejemplo

```
Ejecución 1:
  .text → 0x400000
  .data → 0x601000

Ejecución 2:
  .text → 0x562a0000
  .data → 0x562c1000

Ejecución 3:
  .text → 0x7f5a0000
  .data → 0x7f5c1000
```

### ¿Por Qué?

Para dificultar exploits que dependen de direcciones específicas.

**Importante:** Para aprender seguridad, a menudo **desactivamos ASLR** en ejercicios.

---

## 9️⃣ HERRAMIENTAS PARA INSPECCIONAR ELF

### 1. `file`

```bash
$ file /bin/ls
/bin/ls: ELF 64-bit LSB executable, x86-64, version 1
```

Dice: "Es un binario ELF de 64 bits, arquitectura x86-64"

### 2. `readelf`

```bash
$ readelf -h /bin/ls
ELF Header:
  Magic:   7f 45 4c 46 01 01 01 00 ...
  Class:                             ELF64
  Data:                              2's complement, little endian
  Version:                           1 (current)
  OS/ABI:                            UNIX - System V
  ABI Version:                       0
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Version:                           0x1
  Entry point address:               0x401040
  Start of program headers:          64 (bytes into file)
  ...
```

Muestra toda la información del ELF Header.

### 3. `readelf -l` (Program Headers)

```bash
$ readelf -l /bin/ls

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
  ...
```

Muestra cómo cargar el programa en memoria.

### 4. `readelf -S` (Secciones)

```bash
$ readelf -S /bin/ls

Section Headers:
  [Nr] Name              Type             Address           Offset
  [ 1] .text             PROGBITS         0000000000401000  00001000
       ALLOC EXECINSTR
  [ 2] .rodata           PROGBITS         0000000000580000  00180000
       ALLOC
  [ 3] .data             PROGBITS         00000000007f3220  003f3220
       WRITE ALLOC
  [ 4] .bss              NOBITS           00000000007f9f60  003f9f60
       WRITE ALLOC
  ...
```

Muestra cada sección, su dirección y tamaño.

### 5. `objdump`

```bash
$ objdump -h /bin/ls

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .interp       00000001c  0000000000400000  0000000000400000  00001000  2**0
  1 .note.ABI-tag 00000020  000000000040001c  000000000040001c  0000101c  2**2
  2 .text         00000100  0000000000401000  0000000000401000  00002000  2**4
  ...
```

Similar a `readelf`, pero con otro formato.

---

## 🔟 EJEMPLO PRÁCTICO: DISECCIONAR UN BINARIO

Vamos a analizar un binario de verdad.

### Paso 1: Compilar un Programa Simple

```bash
cat > ejemplo.c << 'EOF'
#include <stdio.h>

int contador = 10;           // .data (inicializado)
int no_inicializado;         // .bss (no inicializado)
const char *msg = "Hola";    // .rodata (constante)

int main() {
    int x = 42;              // stack (local)
    printf("msg: %s\n", msg);
    return 0;
}
EOF

gcc -g -o ejemplo ejemplo.c
```

### Paso 2: Ver la Estructura ELF

```bash
$ readelf -h ejemplo

ELF Header:
  Magic:   7f 45 4c 46 02 01 01 00 ...
  Class:                             ELF64
  Type:                              EXEC (Executable file)
  Machine:                           Advanced Micro Devices X86-64
  Entry point address:               0x401050
  Start of program headers:          64 (bytes into file)
  Start of section headers:          13488 (bytes into file)
```

### Paso 3: Ver los Segmentos (Program Headers)

```bash
$ readelf -l ejemplo

Program Headers:
  Type           Offset             VirtAddr           PhysAddr
  LOAD           0x0000000000000000 0x0000000000400000 0x0000000000400000
                 0x0000000000000a4c 0x0000000000000a4c  R E
  LOAD           0x0000000000000e00 0x0000000000401e00 0x0000000000401e00
                 0x0000000000000230 0x0000000000000350  RW
  ...
```

Dice: "Hay 2 segmentos LOAD"
1. Primero: Carga .text (readable + executable)
2. Segundo: Carga .data y .bss (readable + writable)

### Paso 4: Ver las Secciones

```bash
$ readelf -S ejemplo

Section Headers:
  [Nr] Name              Type             Address           Size
  [ 1] .text             PROGBITS         0000000000401050  000002b5
  [ 2] .rodata           PROGBITS         0000000000402000  000000xx
  [ 3] .data             PROGBITS         0000000000404000  000000yy
  [ 4] .bss              NOBITS           0000000000404020  000000zz
```

Aquí vemos:
- `.text` en 0x401050 (código)
- `.rodata` en 0x402000 (datos const, incluyendo "Hola")
- `.data` en 0x404000 (contador = 10)
- `.bss` en 0x404020 (no_inicializado)

### Paso 5: Ver el Código Desensablado

```bash
$ objdump -d ejemplo | head -20

0000000000401050 <main>:
  401050:	55                   	push   %rbp
  401051:	48 89 e5             	mov    %rsp,%rbp
  401054:	48 83 ec 10          	sub    $0x10,%rsp
  401058:	c7 45 fc 2a 00 00 00 	movl   $0x2a,-0x4(%rbp)
  40105f:	48 8d 35 9a 0f 00 00 	lea    0xf9a(%rip),%rsi
  401066:	bf 00 20 40 00       	mov    $0x402000,%edi
  40106b:	b8 00 00 00 00       	mov    $0x0,%eax
  401070:	e8 bb ff ff ff       	callq  401030 <printf@plt>
  ...
```

Eso es el código máquina de `main()`.

---

## 🔮 HINT FUTURO

Los conceptos de esta lección serán **CRÍTICOS** para:

1. **Lección 03** - Entenderemos cómo se organizan las funciones
2. **Lección 04** - Usaremos GDB para navegar estas secciones
3. **Lección 05** - Disassembly con objdump (lo que ves arriba)
4. **Lección 06** - Buffer overflow: explotaremos el .text o la pila
5. **Proyectos futuros** - Evasión de protecciones, ROP chains

---

## 📊 Resumen Visual

```
PROGRAMA EN C:
┌─────────────────┐
│ int x = 10;     │  → .data (en memoria)
│ int y;          │  → .bss (en memoria)
│ const int z;    │  → .rodata
│ int main() { }  │  → .text (código)
└─────────────────┘

COMPILADOR (gcc):
        ↓

ARCHIVO ELF (disco):
┌──────────────────┐
│ ELF Header       │
│ Program Headers  │
│ .text section    │
│ .data section    │
│ .bss section     │
│ .rodata section  │
│ .symtab          │
│ ... metadata     │
└──────────────────┘

KERNEL (SO):
        ↓

MEMORIA (RAM):
┌──────────────────┐
│ Stack            │
│ Heap             │
│ .bss             │
│ .data            │
│ .rodata          │
│ .text            │
└──────────────────┘

PROGRAMA EJECUTÁNDOSE:
        ↓

OUTPUT: "Hello, world!"
```

---

## ✅ Checklist de Comprensión

Antes de continuar, deberías poder responder:

- [ ] ¿Qué es ELF?
- [ ] ¿Cuál es la diferencia entre un segmento y una sección?
- [ ] ¿Dónde viven las variables globales inicializadas?
- [ ] ¿Dónde viven las variables globales no inicializadas?
- [ ] ¿Por qué .bss no ocupa espacio en disco?
- [ ] ¿Cuál es la diferencia entre offset en archivo y dirección en memoria?
- [ ] ¿Qué es ASLR y por qué importa?
- [ ] ¿Qué hace el SO cuando ejecutas un ELF?

Si respondiste "no" a alguna, **relée esa sección** antes de continuar.

---

## 🚀 Próximos Pasos

Ahora que entiendes la estructura interna:

1. 👉 Practica con las herramientas (`readelf`, `objdump`)
2. 👉 Analiza binarios del sistema (`/bin/ls`, `/bin/cat`)
3. 👉 Crea tu propio programa y desecciona su binario

---

## 📚 Recursos Complementarios

- 📖 [recursos/glosario.md](../../recursos/glosario.md) - "ELF", "Sección", "Segmento"
- 📖 [recursos/herramientas.md](../../recursos/herramientas.md) - readelf, objdump
- 📖 [recursos/referencias.md](../../recursos/referencias.md) - ELF specification
- 🎥 Próximamente: Tutorial interactivo de ELF

---

**Última actualización:** Mayo 2026  
**Tiempo total de lectura:** ~60 minutos  
**Dificultad:** ⭐⭐ (intermedio)  
**Estado:** ✅ Completa

---

## 🎊 Resumen

Has aprendido:

✓ Qué es ELF y por qué es importante  
✓ Estructura interna (headers, secciones, segmentos)  
✓ Cómo el SO carga un binario en memoria  
✓ Diferencia entre archivo y memoria  
✓ Herramientas para inspeccionar binarios  

**Ahora estás listo para analizar binarios a nivel técnico.**

Próxima parada: [Lección 03: C Fundamentals para Seguridad](leccion_03.md)

🚀

# Ejecución de un Binario: Lo que Hace el Kernel

## Introducción

Cuando escribes `./programa` en la terminal, no es GCC quien actúa. Ya terminó su trabajo. Ahora entran en juego:

1. **El shell** (bash/zsh) que interpreta tu comando
2. **El kernel de Linux** que crea el proceso
3. **El loader dinámico** (`ld.so`) que carga las librerías
4. **El CPU** que ejecuta las instrucciones

Este documento explica cada paso.

---

## Paso 1: El Shell Llama a execve()

Cuando escribes `./programa arg1 arg2`, el shell:

1. Hace `fork()` → crea un proceso hijo (copia del shell)
2. En el proceso hijo, llama a `execve()`

`execve()` es una **syscall**: una petición directa al kernel.

```c
// Lo que el shell hace internamente
execve("./programa", argv, envp);
//      ↑ ruta       ↑ args  ↑ variables de entorno
```

A partir de aquí, el kernel toma el control.

---

## Paso 2: El Kernel Verifica el Archivo

El kernel recibe la syscall `execve` y:

### 2a. Verifica permisos

```
¿Tiene el bit de ejecución? (chmod +x)
¿El usuario tiene permiso?
```

Si no → error `Permission denied`.

### 2b. Lee el "magic number"

Los primeros bytes del archivo identifican su tipo. El kernel lee los primeros 4 bytes:

```
ELF:      7f 45 4c 46  (0x7f seguido de "ELF")
Script:   23 21        ("#!/" - shebang)
```

Si empieza con `7f 45 4c 46`, es un ELF → el kernel sabe cómo cargarlo.

```bash
# Ver los primeros bytes de un binario
hexdump -C ./programa | head -1
# 00000000  7f 45 4c 46 02 01 01 00  00 00 00 00 00 00 00 00  |.ELF............|
```

### 2c. Lee el ELF Header

El kernel parsea el ELF Header para obtener:

- **e_type**: tipo de ELF (ET_EXEC = ejecutable, ET_DYN = librería)
- **e_machine**: arquitectura (EM_X86_64 = 0x3e)
- **e_entry**: dirección del punto de entrada (donde empezar a ejecutar)
- **e_phoff**: offset de los Program Headers en el archivo

```bash
readelf -h programa

# ELF Header:
#   Magic:   7f 45 4c 46 02 01 01 00 ...
#   Class:   ELF64
#   Machine: Advanced Micro Devices X86-64
#   Entry point address: 0x401050    ← aquí empezará la ejecución
```

---

## Paso 3: El Kernel Crea el Proceso

### 3a. Espacio de direcciones virtual

El kernel crea un **espacio de direcciones virtual** para el nuevo proceso. Es un mapa de memoria privado y aislado.

```
Espacio virtual del proceso (64 bits = 0x0 a 0xffffffffffffffff):

Dirección alta
0xffffffffffffffff
    ┌──────────────────────────────┐
    │  kernel space                │ ← solo el kernel puede acceder
    │  (proceso no puede tocar)    │
0xffff800000000000
    ├──────────────────────────────┤
    │  stack                       │ ← crece hacia abajo
    │  ↓                           │
    │                              │
    │  (espacio libre)             │
    │                              │
    │  ↑                           │
    │  heap                        │ ← crece hacia arriba (malloc)
    ├──────────────────────────────┤
    │  librerías (.so)             │ ← ld.so las carga aquí
    ├──────────────────────────────┤
    │  .bss  (globals sin init)    │
    │  .data (globals con init)    │
    │  .rodata (strings, const)    │
    │  .text (código)              │ ← punto de entrada aquí
0x400000 (aprox)
    ├──────────────────────────────┤
    │  (zona nula - sin mapear)    │
0x0
    └──────────────────────────────┘
Dirección baja
```

Esta es la memoria **virtual**. El kernel mapea páginas físicas de RAM según las necesita. No toda la memoria virtual existe físicamente.

### 3b. Lee los Program Headers

Los Program Headers describen **segmentos**: cómo cargar el ELF en memoria.

```bash
readelf -l programa

# Program Headers:
#   Type      Offset   VirtAddr           PhysAddr           FileSiz  MemSiz   Flg Align
#   LOAD      0x000000 0x0000000000400000 0x0000000000400000 0x000570 0x000570 R E 0x200000
#   LOAD      0x000df0 0x0000000000600df0 0x0000000000600df0 0x000228 0x000230 RW  0x200000
#   DYNAMIC   0x000e08 0x0000000000600e08 0x0000000000600e08 0x0001d0 0x0001d0 RW  0x8
```

El kernel ve dos segmentos LOAD:

**Primer LOAD (R E = Read + Execute):**
- Carga desde el offset 0x000000 del archivo
- En la dirección virtual 0x400000
- Contiene: `.text`, `.rodata`, `.plt`
- Permisos: lectura + ejecución (NO escritura)

**Segundo LOAD (RW = Read + Write):**
- Carga desde el offset 0x000df0 del archivo
- En la dirección virtual 0x600df0
- Contiene: `.data`, `.bss`, `.got`, `.dynamic`
- Permisos: lectura + escritura (NO ejecución)

Estos permisos son los que implementan el NX bit: el segmento de datos es RW pero no ejecutable. El segmento de código es RE pero no escribible.

### 3c. Mapea los segmentos con mmap()

Para cada segmento LOAD, el kernel hace una llamada interna a `mmap()`:

```c
// Internamente el kernel hace algo así:
mmap(0x400000,   // dirección virtual
     tamaño,     // cuántos bytes
     PROT_READ | PROT_EXEC,  // permisos
     MAP_PRIVATE | MAP_FIXED,
     fd_del_archivo,
     offset_en_archivo);
```

**Esto no copia la memoria todavía.** Solo crea el mapping. La RAM física se asigna cuando el proceso accede a esa dirección por primera vez (page fault → el kernel carga la página).

### 3d. Inicializa .bss

Las variables globales sin valor inicial (en `.bss`) deben empezar en cero. El kernel mapea estas páginas como "zero pages": páginas de RAM llenas de ceros.

### 3e. Crea el stack inicial

El kernel crea el stack del proceso. En la cima del stack coloca:

```
Stack inicial:

┌──────────────────────────────┐ ← dirección alta
│  variables de entorno        │  HOME=/home/yavier, PATH=..., etc.
│  (strings)                   │
├──────────────────────────────┤
│  argumentos                  │  "programa", "arg1", "arg2" (strings)
├──────────────────────────────┤
│  punteros a env vars         │  envp[] → apunta a strings arriba
│  NULL                        │
├──────────────────────────────┤
│  punteros a argumentos       │  argv[] → apunta a strings arriba
│  NULL                        │
├──────────────────────────────┤
│  argc                        │  número de argumentos (3 en este caso)
└──────────────────────────────┘ ← rsp apunta aquí al inicio
```

Esto es lo que recibe `main(int argc, char **argv)`.

### 3f. Configura los registros iniciales

El kernel configura los registros del CPU para el nuevo proceso:

```
rsp = dirección del tope del stack
rip = e_entry del ELF header (punto de entrada)
```

El resto de registros se inicializan a 0.

---

## Paso 4: El Loader Dinámico (ld.so)

Si el ejecutable usa librerías dinámicas (casi siempre), el kernel no salta directamente a `main()`. Primero ejecuta el **loader dinámico**.

### ¿Qué es ld.so?

Es una librería especial del sistema: `/lib64/ld-linux-x86-64.so.2`.

El ELF header tiene un campo que apunta a ella:

```bash
readelf -l programa | grep interpreter
#   [Requesting program interpreter: /lib64/ld-linux-x86-64.so.2]
```

El kernel carga `ld.so` en memoria y salta a ella **antes** que a tu código.

### ¿Qué hace ld.so?

**4a. Lee la sección `.dynamic`**

Contiene la lista de librerías que el programa necesita:

```bash
readelf -d programa | grep NEEDED
#   (NEEDED)  Shared library: [libc.so.6]
```

**4b. Encuentra cada librería**

Busca en:
1. `LD_LIBRARY_PATH` (variable de entorno)
2. `/etc/ld.so.cache` (caché del sistema)
3. Rutas por defecto (`/lib64`, `/usr/lib64`)

**4c. Carga cada librería con mmap()**

Mapea `libc.so.6` en el espacio de direcciones del proceso:

```
Espacio virtual:
    ├──────────────────────────────┤
    │  libc.so.6                   │ ← mapeada aquí (ASLR: dirección aleatoria)
    │  (código de printf, malloc..)│
    ├──────────────────────────────┤
```

**4d. Resuelve los símbolos (lazy binding)**

Actualiza la GOT con las direcciones reales de las funciones:

```
Antes:
GOT[printf] = dirección de plt_resolver

Primera llamada a printf:
1. Salta a printf@plt
2. plt_resolver busca la dirección real de printf en libc
3. Escribe esa dirección en GOT[printf]
4. Salta a printf

Llamadas siguientes:
1. Salta a printf@plt
2. Lee GOT[printf] directamente (ya tiene la dirección real)
3. Salta a printf
```

**4e. Llama a los inicializadores**

El ELF puede tener funciones que deben ejecutarse antes de `main()` (registradas en `.init_array`). ld.so las llama en orden.

**4f. Llama a `__libc_start_main()`**

ld.so finalmente llama a una función de libc que:
1. Inicializa el entorno de C (locale, señales, etc.)
2. Llama a tu `main(argc, argv, envp)`
3. Cuando `main()` retorna, llama a `exit()`

---

## Paso 5: Tu main() se Ejecuta

En este punto:

- La memoria está completamente configurada
- Las librerías están cargadas
- Los registros están listos
- `rip` apunta a la primera instrucción de `main()`

El CPU entra en su ciclo fundamental:

### El Ciclo Fetch-Decode-Execute

Esto ocurre miles de millones de veces por segundo:

```
┌─────────────────────────────────────────┐
│                                         │
│   1. FETCH: leer instrucción en rip     │
│      rip → dirección → opcode bytes     │
│                                         │
│   2. DECODE: interpretar el opcode      │
│      0x55 → "push rbp"                  │
│      ¿qué hace? ¿qué operandos?         │
│                                         │
│   3. EXECUTE: ejecutar                  │
│      push rbp → rsp -= 8               │
│                 memoria[rsp] = rbp      │
│                                         │
│   4. rip += tamaño de instrucción       │
│      apunta a la siguiente instrucción  │
│                                         │
│   → volver a 1                          │
│                                         │
└─────────────────────────────────────────┘
```

### Page Faults (carga perezosa)

Recuerda: el kernel mapeó la memoria pero no la cargó en RAM todavía. Cuando el CPU intenta acceder a una dirección que tiene mapping pero no página física:

1. CPU lanza una **page fault** (excepción)
2. El kernel intercepta la excepción
3. El kernel asigna una página física de RAM
4. Copia el contenido del archivo (o ceros para .bss)
5. El CPU reintenta la instrucción
6. Ahora funciona

Esto pasa de forma transparente. Tu programa nunca lo nota.

---

## Paso 6: main() Retorna y el Proceso Termina

Cuando `main()` ejecuta `return 0`:

1. El valor `0` queda en `rax`
2. Retorna a `__libc_start_main()`
3. libc llama a `exit(rax)`
4. `exit()` ejecuta los handlers registrados con `atexit()`
5. Llama a la syscall `exit_group(código)`
6. El kernel:
   - Libera toda la memoria del proceso
   - Cierra todos los file descriptors
   - Envía señal SIGCHLD al proceso padre (el shell)
   - El proceso desaparece

El shell recibe SIGCHLD, obtiene el código de salida con `wait()`, y vuelve a mostrar el prompt.

---

## Resumen: La Cadena Completa

```
./programa
    │
    ▼
Shell hace fork() + execve()
    │
    ▼
Kernel verifica permisos y magic number
    │
    ▼
Kernel parsea ELF Header y Program Headers
    │
    ▼
Kernel mapea segmentos con mmap() (código, datos)
    │
    ▼
Kernel crea stack con argc, argv, envp
    │
    ▼
Kernel configura rip = punto de entrada de ld.so
    │
    ▼
ld.so carga librerías dinámicas con mmap()
    │
    ▼
ld.so resuelve símbolos en GOT
    │
    ▼
ld.so llama __libc_start_main()
    │
    ▼
__libc_start_main() llama main()
    │
    ▼
CPU ejecuta ciclo Fetch-Decode-Execute
(con page faults transparentes cuando necesita RAM)
    │
    ▼
main() retorna → exit() → syscall exit_group()
    │
    ▼
Kernel libera recursos, notifica al shell
```

---

## Lecturas Relacionadas

- `recursos/compilacion-proceso.md` - Cómo GCC crea el binario
- `recursos/compilacion-ejemplo.md` - Ver todo esto con comandos reales
- `01-teoria/leccion_02.md` - ELF format
- `01-teoria/leccion_05.md` - Assembly y registros

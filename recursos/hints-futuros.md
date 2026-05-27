# Hints para Proyectos Futuros en la Serie

## Introducción

Este documento contiene pistas y conceptos que te permitirán una transición suave hacia los proyectos siguientes en la serie de seguridad de School 42.

**Importante:** No leas esto antes de terminar ft_snowcrash. Está aquí para cuando necesites prepararte para el siguiente proyecto.

---

## Índice
1. [Transición a ROP y Gadgets](#transición-a-rop-y-gadgets)
2. [Heap Exploitation](#heap-exploitation)
3. [Format Strings Avanzado](#format-strings-avanzado)
4. [Bypass de Protecciones](#bypass-de-protecciones)
5. [Kernel Exploitation](#kernel-exploitation)
6. [Malware Analysis](#malware-analysis)

---

## Transición a ROP y Gadgets

### Concepto Fundamental

En ft_snowcrash, aprendiste a controlar `rip` (instruction pointer). El siguiente paso es **no ejecutar tu código**, sino **reutilizar el que ya existe**.

```
ft_snowcrash: Escribir shellcode
     ↓
ROP: Escribir direcciones de código existente
     ↓
Resultado final: Turing completo sin ejecutar código nuevo
```

### Por Qué Esto Importa

**Con NX bit activado:**
- Tu shellcode no puede ejecutarse (es datos, no código)
- Solución: usar instrucciones que ya están en memoria
- Estas instrucciones terminan en `ret`

### Qué Aprender Primero

1. **Gadget Chains**
   - Múltiples `ret` seguidos forman una cadena
   - `pop rdi; ret` + dirección de cadena = primer argumento

2. **Salvando Registros**
   - Antes de cada `call`, el llamante debe guardar registros
   - ROP aprovecha esto

3. **Construcción de Stack**
   - Tu exploit forma un stack falso en memoria
   - GDB te mostrará exactamente cómo

### Actividades de Preparación

```bash
# Dentro de ft_snowcrash o después:

# 1. En tu exploit de buffer overflow, localiza gadgets útiles
objdump -d ./tu_binario | grep "pop.*rdi"
objdump -d ./tu_binario | grep "pop.*rsi"

# 2. Encuentra "syscall" o "call <funcion>"
objdump -d ./tu_binario | grep -i "call\|syscall"

# 3. Intenta una cadena simple:
# - Overflow hasta RIP
# - En lugar de shellcode, pon dirección de gadget
# - Ese gadget ejecutará una instrucción
```

### Primer ROP Challenge

Después de ft_snowcrash, intenta esto:

```c
// simple_rop.c
#include <stdio.h>

void funcion_util() {
    printf("ROP funcionó!\n");
}

void vulnerable(char *input) {
    char buffer[10];
    strcpy(buffer, input);  // Overflow
}

int main(int argc, char **argv) {
    if (argc > 1) {
        vulnerable(argv[1]);
    }
    return 0;
}
```

Tu reto: **sin usar shellcode**, llama a `funcion_util()`.

Pista: necesitas `rdi`, `rsi`, `rdx` en los valores correctos para una syscall.

---

## Heap Exploitation

### El Salto Conceptual

**Stack:** Lineal, predecible
**Heap:** Complejo, dinámico, más potente

```
Stack exploitation: Sobrescribir variables locales
     ↓
Heap exploitation: Sobrescribir estructuras de datos complejas
     ↓
Resultado: Control total del programa
```

### Conceptos Clave

1. **Use-After-Free**
   - Liberar memoria pero seguir usándola
   - El allocator puede reasignar esa memoria a objeto diferente

2. **Double Free**
   - Liberar dos veces la misma memoria
   - Corrompe el allocator

3. **Heap Overflow**
   - Similar a buffer overflow pero en heap
   - Puedes sobrescribir metadatos del allocator

### Qué Necesitas Entender Primero

```c
// Concepto básico de heap
int *ptr = malloc(sizeof(int));  // Memoria en heap
*ptr = 42;                        // Usar memoria
free(ptr);                        // Liberar
ptr = NULL;                       // Buena práctica

// PERO, qué pasa si no pones NULL:
free(ptr);                        // Segundo free - PELIGROSO
*ptr = 50;                        // Use-after-free - PELIGROSO
```

### El Allocator Importa

**ptmalloc** (glibc malloc):
- Mantiene metadatos en memoria
- Estos metadatos son vulnerables
- Corruption → Control

```bash
# Inspeccionar heap en GDB
(gdb) break en_funcion_con_malloc
(gdb) run
(gdb) x/30x $rsp           # Ver heap
```

### Preparación

Después de ft_snowcrash:

```bash
# 1. Leer sobre allocadores
#    - Cómo malloc/free funcionan
#    - Linked lists en memoria

# 2. Crear programa con malloc/free
#    - Usar GDB para ver heap layout

# 3. Intentar heap overflow
gcc -g -O0 -o heap_test heap_test.c
gdb ./heap_test
(gdb) break donde_esta_malloc
(gdb) x/100x &estructura
```

---

## Format Strings Avanzado

### Hay Dos Niveles

**Nivel 1 (ft_snowcrash):** Lectura de memoria con `%x`, `%s`

**Nivel 2 (Siguiente):** Escritura de memoria con `%n`

### Cómo Funciona `%n`

```c
int count = 0;
printf("Hola%n", &count);  // count = 4 (caracteres impresos)
```

Esto escribe en memoria. Con control del stack:

```bash
# Tu input:
"AAAA%n"

# printf ve:
# - AAAA (4 bytes)
# - %n = "escribe cantidad en dirección dada"
# - Pero ¿de dónde toma la dirección?
# - Del stack (porque es vulnerable)
```

### Exploración Avanzada

```bash
# Encontrar offset exacto:
python3
>>> for i in range(1, 100):
...     print(f"AAAA %{i}$x")
# Cuando veas AAAAAAA, encontraste tu offset
```

### Ataque Completo (Próximo Nivel)

```python
# Pseudo-código
target_addr = 0x601234   # Dirección a escribir
value = 0x4141          # Valor a escribir

payload = pack(target_addr)
payload += f"%{value}$n"  # Format string

# Enviar a programa vulnerable
# Resultado: memoria en 0x601234 ahora = 0x4141
```

---

## Bypass de Protecciones

### Stack Canaries

**ft_snowcrash:** Probablemente compilaste sin canaries

**Siguiente nivel:** Están habilitados

```bash
# Ver si programa tiene canaries
checksec ./programa
# Stack canary : Enabled

# Cómo funciona:
# 1. Programa pone valor mágico en stack antes de RIP
# 2. Antes de retornar, verifica que siga ahí
# 3. Si cambió, crash

# Bypass métodos:
# - Leak el canario (leerlo antes de sobrescribir)
# - Format strings para extraerlo
# - Use-after-free en ciertas circunstancias
```

### ASLR (Address Space Layout Randomization)

**ft_snowcrash:** Probablemente lo desactivaste

**Siguiente nivel:** Está activado

```bash
# Ver estado
cat /proc/sys/kernel/randomize_va_space
# 0 = off, 1 = parcial, 2 = total

# Cómo afecta:
# Cada run, direcciones cambian
# Tu hardcoded 0x400500 no funciona siempre

# Bypass métodos:
# - Relative addressing en vez de absolute
# - Leak direcciones usando format strings
# - RIP relativos (RIP-relative addressing)
```

### NX Bit

**ft_snowcrash:** Probablemente lo desactivaste con `-z execstack`

**Siguiente nivel:** Está activado

```bash
# Ver si está activado
checksec ./programa
# NX : Enabled

# Consecuencia:
# No puedes ejecutar datos que escribas
# Solución: ROP chains

# Qué practicar:
# - Entender permisos de página (mmap)
# - Cómo os.h asigna permisos
# - ROP como workaround
```

### Protección Combinada

El desafío final: **todo activado**

- Stack Canaries (leak + bypass)
- ASLR (relative addressing)
- NX Bit (ROP chains)

```bash
# Configuración desafiante
gcc -g -O2 \
    -fstack-protector-strong \  # Canaries
    -D_FORTIFY_SOURCE=2 \       # Más protecciones
    -Wl,-z,relro \              # RELRO
    -Wl,-z,now \                # Lazy binding off
    -o programa programa.c

# Ahora es DIFÍCIL. Pero posible.
```

---

## Kernel Exploitation

### El Próximo Salto Cualitativo

**User-space:** Controlar un programa

**Kernel-space:** Controlar el sistema operativo

### Conceptos Base

1. **syscalls**
   - Interfaz programa ↔ kernel
   - Algunos syscalls son vulnerables

2. **Privileges**
   - UID/GID
   - Capabilities (más fino que UID/GID)

3. **Device Drivers**
   - Código en kernel mode
   - A veces inseguro

### Mínimo para Empezar

```c
// Llamar directamente un syscall
#include <unistd.h>
#include <sys/syscall.h>

int main() {
    // write() es un syscall
    syscall(SYS_write, 1, "Hola\n", 5);
    return 0;
}
```

La exploración comienza aquí. 

**En GDB, cuando haces `syscall`, ves:**
- Cambio a kernel mode
- strace muestra qué syscall fue

### Preparación

```bash
# 1. Entender syscalls
man 2 open
man 2 read
man 2 write

# 2. Usar strace para verlas
strace ./programa

# 3. Leer asm de syscall
objdump -d programa | grep syscall

# 4. Intentar exploit de kernel:
#    - Buscar CVE en Linux
#    - Entender exploitación
#    - (No se espera que hagas eso todavía)
```

---

## Malware Analysis

### Cuando Cambias de Perspectiva

**Así far:** Análisis ofensivo (cómo explotar)

**Futuro:** Análisis defensivo (cómo detectar)

Pero ambos usan las mismas herramientas:

- objdump
- strings
- GDB
- strace
- ltrace

### Qué Buscar en un Binario Sospechoso

```bash
# 1. Strings sospechosas
strings ./binario | grep -i "reverse\|shell\|nc\|virus"

# 2. Network connections
strace ./binario | grep -i "socket\|connect\|bind"

# 3. Archivos sospechosos
strace ./binario | grep -i "open\|write"

# 4. Comportamiento oculto
strace ./binario | wc -l  # Demasiadas syscalls?

# 5. Métodos anti-análisis
strings ./binario | grep -i "gdb\|strace\|ptrace"
```

### Práctica

Después de completar ft_snowcrash:

```bash
# Descarga algunos malware samples (SAFE ENVIRONMENT ONLY):
# - VirusShare (requiere registración)
# - Contagio
# - Malware Traffic Analysis

# Ejecuta en VM, nunca en tu máquina real
# Analiza:
# - Qué hace
# - Cómo lo oculta
# - Cómo lo detectarías
```

---

## Mapa de Ruta Recomendado

```
ft_snowcrash (AQUÍ)
├── Dominio: Buffer overflow, básico de seguridad
├── Tiempo estimado: 3-4 semanas
└── Después: Choose your path

Camino 1: BINARIO AVANZADO
├── ROP chains (2 semanas)
├── Heap exploitation (2 semanas)
├── Format strings (1 semana)
└── Proyecto siguiente (3-4 semanas)

Camino 2: SEGURIDAD GENERAL
├── Web exploitation (2 semanas)
├── Scripting Python/Bash (2 semanas)
├── CTFs en HackTheBox (2 semanas)
└── Elección de especialidad

Camino 3: KERNEL
├── User-space fundamentals (este)
├── Syscalls y privilegios (2 semanas)
├── Kernel drivers (3 semanas)
├── Kernel exploitation (4 semanas)

(Recomendación: Camino 1 para la serie 42, complementa con 2)
```

---

## Preguntas Frecuentes del "Siguiente Nivel"

### "¿Necesito aprender assembly completo?"

**Respuesta:** Para ROP: sí, al menos 50 instrucciones.
Para heap: no, más importante es entender memoria.
**Recurso:** "The Art of Assembly Language" (x86-64)

### "¿Cómo busco gadgets eficientemente?"

**Respuesta:** Herramientas automatizadas:
- ROPgadget
- Ropper
- radare2's search

pero primero entiende manualmente

### "¿ASLR se puede siempre bypassear?"

**Respuesta:** Depende de las protecciones combinadas.
- Solo ASLR: sí (información leaks)
- ASLR + NX + Canaries: difícil pero posible
- ASLR + NX + Canaries + CFI: muy difícil

### "¿Debo aprender Python?"

**Respuesta:** Para exploit development después: **sí**.
**Librería:** pwntools (el estándar)

### "¿Cuánto tiempo para dominar ROP?"

**Respuesta:** 
- Conceptos: 1 semana
- Simple ROP: 2 semanas
- Complicadas: 1-2 meses

---

## Checklist de Preparación

Cuando termines ft_snowcrash y antes de comenzar el siguiente proyecto:

- [ ] Comprende cómo compilador inserta protecciones
- [ ] Puedes bypassear buffer overflow básico manualmente
- [ ] GDB no tiene secretos para ti
- [ ] Entiendes convención de llamadas x86-64
- [ ] Puedes localizar gadgets en un binario
- [ ] Sabes leer/escribir exploits en Python
- [ ] Has completado 5+ máquinas de HackTheBox
- [ ] Entiendes ASLR, NX bit, canaries (conceptualmente)

Si checkeaste 6+ de estos, estás listo.

---

## Última Nota

Cada concepto en esta sección se enseña formalmente en el siguiente proyecto. Esto es solo para que sepas qué esperar y qué *opcionalmente* puedas empezar a explorar.

**No es requisito** aprender todo esto antes. Pero si tienes curiosidad, estos hints te guían hacia los recursos correctos.

El aprendizaje en seguridad es iterativo: aprenderás, practicarás, harás proyectos, aprenderás más avanzado, etc.

¡Enfócate en terminar ft_snowcrash bien. El resto vendrá naturalmente!

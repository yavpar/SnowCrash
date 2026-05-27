# 📚 Lección 03: C Fundamentals para Seguridad

## 🎯 Objetivos de Aprendizaje

Al terminar esta lección, entenderás:

- ✅ Por qué C es el lenguaje de seguridad ofensiva
- ✅ Cómo funciona la memoria en C (stack, heap, variables)
- ✅ Qué son y cómo usan C los punteros (crítico para exploits)
- ✅ Arrays y buffers (dónde ocurren los overflows)
- ✅ Funciones y stack frames (para ROP chains)
- ✅ Strings en C (terminados en null)
- ✅ Tipos de datos y tamaños (bytes, words, etc)
- ✅ Operadores peligrosos en C

**Tiempo estimado:** 75-90 minutos

---

## 📖 Introducción

Lecciones 01 y 02 fueron sobre conceptos generales.

Ahora necesitas entender **C a fondo**, porque:

1. **Todos los binarios vulnerables** están escritos en C (o C++)
2. **Entender C = Entender vulnerabilidades**
3. **Crear exploits** requiere pensar como un programa C

Este es un momento **crítico** del curso. Domina esto. 🎯

---

## 1️⃣ ¿POR QUÉ C Y NO OTRO LENGUAJE?

### La Historia

```
1972: Nace C (Ken Thompson, Dennis Ritchie)
      
Características:
  ✓ Muy cercano al hardware (bajo nivel)
  ✓ Control total de la memoria
  ✓ Mínimo overhead (muy rápido)
  ✗ El programador es responsable de TODO

Resultado:
  C se convirtió en el lenguaje de sistemas
  ├─ Unix/Linux kernel
  ├─ Casi todos los servidores
  ├─ Binarios críticos del sistema
  └─ Muchísimo software legado
```

### ¿Por Qué C en Seguridad?

```
Python, Java, Rust (lenguajes modernos):
  ✓ Memory safe (protegen el programador)
  ✗ Menos vulnerable

C (lenguaje antiguo):
  ✓ Extremadamente vulnerable
  ✗ Sin protecciones automáticas

Realidad: El mundo corre en C
  → Millones de líneas de código C antiguo
  → Bugs de seguridad esperando ser encontrados
```

**Conclusión:** Si quieres encontrar vulnerabilidades, necesitas entender C.

---

## 2️⃣ MEMORIA EN C: STACK vs HEAP

### Stack (Pila)

```
¿Qué es?
  La memoria "automática" para variables locales

Características:
  ✓ Rápido (direcciones contiguas)
  ✓ Automático (se libera al salir de función)
  ✗ Limitado (típicamente 8MB)
  ✗ LIFO (Last In First Out)

Ejemplo:
void funcion() {
    int x = 42;           // Aquí va en stack
    char buffer[100];     // Aquí va en stack
}
// Al terminar, x y buffer se liberan automáticamente
```

### Heap (Montículo)

```
¿Qué es?
  La memoria "manual" para datos dinámicos

Características:
  ✓ Grande (gigabytes disponibles)
  ✓ Durable (mientras exista el programa)
  ✗ Lento (fragmentación)
  ✗ Manual (necesitas free())

Ejemplo:
int *ptr = malloc(100);   // Reserva 100 bytes en heap
// ... usa ptr ...
free(ptr);                // Debes liberar manualmente
```

### Diferencia Visual

```
MEMORIA:

              STACK                HEAP
            ┌────────┐           ┌────────┐
            │ x = 42 │           │ malloc │
            ├────────┤           ├────────┤
            │buffer  │           │ datos  │
            │[100]   │           │ datos  │
            └────────┘           └────────┘
            Automático            Manual

Cuando función termina:
  Stack: Todo se libera
  Heap: Solo si llamaste free()
```

### En Seguridad

```
Buffer Overflow en Stack:
  int buffer[100];
  strcpy(buffer, input);    // ¡Peligro! Sin límite
  
  Si input > 100 caracteres:
    → Sobrescribe variables locales
    → Sobrescribe el retorno de la función
    → ¡Ejecución de código!

Buffer Overflow en Heap:
  int *buffer = malloc(100);
  strcpy(buffer, input);    // ¡Peligro! Sin límite
  
  Si input > 100 caracteres:
    → Sobrescribe datos en heap
    → Puede corromper metadatos
    → ¡Ejecución de código!
```

---

## 3️⃣ PUNTEROS: LO MÁS IMPORTANTE

### ¿Qué es un Puntero?

**Un puntero es una variable que contiene una dirección de memoria.**

```c
int x = 42;
int *ptr = &x;   // ptr apunta a x

// Ahora:
*ptr     // 42 (el valor en la dirección que ptr apunta)
&ptr     // Dirección donde vive ptr (algo como 0x1000)
ptr      // Dirección que contiene ptr (algo como 0x2000)
```

### Visualizar Punteros

```
MEMORIA:
┌──────────────────────────────────────┐
│ Dirección 0x2000: x = 42             │
│ Dirección 0x3000: ptr = 0x2000       │  ← ptr "apunta a" x
└──────────────────────────────────────┘

En código:
  x             → 42
  &x            → 0x2000
  ptr           → 0x2000
  *ptr          → 42
  &ptr          → 0x3000
```

### Aritmética de Punteros

```c
int array[5] = {10, 20, 30, 40, 50};
int *ptr = array;        // Apunta al primer elemento

ptr       // 0x1000 (dirección del array[0])
*(ptr)    // 10 (array[0])
*(ptr+1)  // 20 (array[1])
*(ptr+2)  // 30 (array[2])

ptr[0]    // Equivalente a *(ptr)
ptr[1]    // Equivalente a *(ptr+1)
```

**Nota:** `ptr+1` no suma 1 byte, suma el tamaño del tipo.

```c
int *ptr;
ptr+1     // suma 4 bytes (sizeof(int) = 4)

char *ptr2;
ptr2+1    // suma 1 byte (sizeof(char) = 1)
```

### Por Qué Son Peligrosos

```c
void funcion(int *ptr) {
    *ptr = 999;  // Modifica lo que apunta ptr
}

int main() {
    int x = 42;
    int y = 100;
    funcion(&x);   // Modificar x: bien
    
    int *ptr = malloc(10);
    free(ptr);
    funcion(ptr);  // Use-after-free: peligro
    
    int buffer[10];
    funcion(buffer - 100);  // Desbordamiento: peligro
}
```

---

## 4️⃣ ARRAYS Y BUFFERS

### Arrays en C

```c
int array[5];     // Espacio para 5 ints
char buffer[100]; // Espacio para 100 chars

// En memoria:
// array:    [?, ?, ?, ?, ?]  (20 bytes: 5*4)
// buffer:   [?, ?, ..., ?]   (100 bytes)
```

### Arrays como Punteros

```c
int array[5] = {10, 20, 30, 40, 50};

// Estas son EQUIVALENTES:
array[2]        // 30
*(array + 2)    // 30

// array es un puntero al primer elemento
int *ptr = array;  // Válido
```

### El Problema: Sin Límites

```c
char buffer[10];
scanf("%s", buffer);  // ¡Peligro!

Si el usuario escribe "123456789012345":
  Se escribe más de 10 caracteres
  → Sobrescribe memoria vecina
  → Buffer overflow
```

### Strings en C

```c
char str[] = "Hola";

// En memoria:
// Dirección  Valor
// 0x1000     'H' (72)
// 0x1001     'o' (111)
// 0x1002     'l' (108)
// 0x1003     'a' (97)
// 0x1004     '\0' (0) ← NULL TERMINATOR ¡CRÍTICO!

// Sin el '\0', C no sabe dónde termina el string
// ¡Esto causa lectura de memoria más allá del string!
```

### Funciones Peligrosas con Strings

```c
strcpy(dest, src);   // ¡NUNCA USES ESTO!
// Copia src a dest sin verificar tamaño
// Buffer overflow garantizado

strcat(dest, src);   // ¡NUNCA USES ESTO!
// Concatena sin verificar tamaño

gets(buffer);        // ¡NUNCA USES ESTO!
// Lee input sin límite

scanf("%s");         // ¡NUNCA USES ESTO!
// Lee string sin límite
```

### Funciones Seguras (más o menos)

```c
strncpy(dest, src, n);    // Copia máximo n caracteres
strncat(dest, src, n);    // Concatena máximo n caracteres
fgets(buffer, size, stdin); // Lee máximo size caracteres
snprintf(buffer, size, fmt, ...); // Printf seguro
```

**Nota:** "Seguro" es relativo. Incluso estas pueden tener bugs.

---

## 5️⃣ FUNCIONES Y STACK FRAMES

### ¿Qué es un Stack Frame?

**Un stack frame es el espacio en el stack para una función.**

```c
int suma(int a, int b) {     // Aquí comienza nuevo frame
    int resultado = a + b;    // Variable local en frame
    return resultado;
}                             // Frame se destruye

int main() {                  // Frame de main
    int x = suma(5, 3);       // Crea frame de suma
    return 0;
}                             // Frame de main se destruye
```

### Visualización del Stack

```
Cuando suma(5, 3) está ejecutándose:

STACK (crece hacia abajo):

┌────────────────────────┐
│ ESP → resultado = 8    │  ← Stack frame de suma()
│       b = 3            │
│       a = 5            │
│       Return address   │  ← ¡CRÍTICO para ROP!
│       x = ?            │  ← Stack frame de main()
│       ...              │
└────────────────────────┘
```

### Return Address

**La dirección de retorno es la dirección de la instrucción siguiente a la llamada.**

```c
// Dirección 0x401000: call suma
// Dirección 0x401006: x = suma(...)

Cuando suma() termina:
  ├─ Lee el return address del stack
  └─ Salta a esa dirección (0x401006)
  
Si modifica el return address:
  ├─ Puede saltar a código malicioso
  └─ ¡Ejecución de código!
```

### Este es el Concepto Clave para ROP

En proyectos futuros, modificaremos el return address para ejecutar código.

---

## 6️⃣ TIPOS DE DATOS Y TAMAÑOS

### Tamaños Comunes (en x86-64)

```c
sizeof(char)                    // 1 byte
sizeof(short)                   // 2 bytes
sizeof(int)                     // 4 bytes
sizeof(long)                    // 8 bytes (en 64-bit)
sizeof(long long)               // 8 bytes
sizeof(float)                   // 4 bytes
sizeof(double)                  // 8 bytes
sizeof(void *)                  // 8 bytes (en 64-bit)
```

### ¿Por Qué Importan los Tamaños?

```
En explotación:

Cuando haces aritmética de punteros:
  char *p;
  p + 1    // suma 1 byte
  
  int *p;
  p + 1    // suma 4 bytes
  
Cuando sobrescribes memoria:
  int x;        // 4 bytes
  char buffer[10];
  int y;
  
  // Si sobrescribes buffer con 50 bytes:
  buffer: [XXXX][XXXX][XXXX]
  x:      [XXXX][XXXX][XXXX]  ← Sobrescribado
  y:      [XXXX]              ← Parcialmente
```

---

## 7️⃣ VARIABLES GLOBALES vs LOCALES

### Globales (en .data o .bss)

```c
int global = 10;           // .data

int main() {
    return 0;
}
```

Características:
- ✓ Viven en .data (si inicializada) o .bss
- ✓ Existen toda la vida del programa
- ✓ Accesibles desde cualquier función
- ✗ Pueden ser sobrescritas por overflows

### Locales (en stack)

```c
int main() {
    int local = 10;        // Stack
    return 0;
}
```

Características:
- ✓ Viven en el stack
- ✓ Solo existen dentro de la función
- ✗ Se destruyen al salir de la función
- ✗ Muy vulnerables a overflows (vecinas en stack)

### En Seguridad

```
Variables globales:
  → Localizadas en memoria
  → Conocidas direcciones (sin ASLR)
  → Blancos para modificar

Variables locales:
  → Impredecibles (en el stack)
  → Pueden sobrescribirse con overflow
  → Incluyen return address (crítica)
```

---

## 8️⃣ OPERADORES PELIGROSOS

### 1. Acceso a Memoria Nula

```c
int *ptr = NULL;
*ptr = 42;           // Segmentation fault (crash)
```

### 2. Use-After-Free

```c
int *ptr = malloc(10);
free(ptr);
printf("%d\n", *ptr); // ¡El puntero apunta a memoria liberada!
```

### 3. Double Free

```c
int *ptr = malloc(10);
free(ptr);
free(ptr);           // ¡Liberar dos veces causa corrupción!
```

### 4. Buffer Overflow

```c
char buffer[10];
strcpy(buffer, "123456789012345"); // Escribe 15 caracteres en 10 bytes
```

### 5. Integer Overflow

```c
unsigned int size = 1000;
unsigned int extra = 4000;
unsigned int total = size + extra;  // 5000 (OK)

unsigned int total2 = UINT_MAX + 1; // Desborda a 0
```

### 6. Type Confusion

```c
int *ptr = (int *)malloc(10);
// malloc devolvió 10 bytes
// Pero ptr es un int* (asume 4 bytes por elemento)
// ptr[2] accede más allá de los 10 bytes
```

---

## 9️⃣ CASTING (Conversión de Tipos)

### ¿Qué es?

**Casting es decirle al compilador "trata esto como otro tipo".**

```c
int x = 42;
char *ptr = (char *)&x;  // Trata la dirección como char*

// Ahora puedes leer x byte por byte:
printf("%d %d %d %d\n", ptr[0], ptr[1], ptr[2], ptr[3]);
```

### Peligroso

```c
void *ptr = malloc(100);

int *int_ptr = (int *)ptr;      // Trata como int*
char *char_ptr = (char *)ptr;   // Trata como char*

int_ptr[0]   // Lee 4 bytes
char_ptr[0]  // Lee 1 byte

// Si lo haces mal, ¡lees basura!
```

---

## 🔟 COMPILACIÓN Y OPTIMIZACIONES

### Compilación Básica

```bash
gcc -c archivo.c    # Compila a objeto
gcc -o programa *.c # Compila y enlaza
```

### Flags Útiles para Seguridad

```bash
gcc -g -O0 programa.c    # Debug info, sin optimizaciones
gcc -Wall programa.c      # Muestra advertencias
gcc -fno-stack-protector # Deshabilita protecciones (para aprender)
gcc -z execstack          # Permite código en stack (para aprender)
```

### Optimizaciones Peligrosas

```bash
gcc -O2 programa.c
gcc -O3 programa.c
```

Las optimizaciones pueden hacer que el compilador:
- Elimine variables "no usadas"
- Reordene instrucciones
- Cambie el comportamiento

Para **aprender**, siempre usa `-O0`.

---

## 🔮 HINT FUTURO

Todo en esta lección volverá en:

1. **Lección 04** - GDB: Inspeccionar frames y punteros
2. **Lección 05** - Assembly: Ver cómo se compiló el código C
3. **Lección 06** - Buffer Overflow: Explotar arrays
4. **Lección 07** - Format Strings: Leer/escribir memoria
5. **Proyectos Futuros** - ROP Chains: Explotar return addresses

**Es crítico dominar C completamente.**

---

## ✅ Checklist de Comprensión

- [ ] Entiendo diferencia entre stack y heap
- [ ] Sé qué es un puntero y cómo usarlo
- [ ] Entiendo arrays y buffers
- [ ] Sé qué es un null terminator en strings
- [ ] Entiendo stack frames y return address
- [ ] Conozco los tamaños de tipos de datos
- [ ] Sé qué es variable global vs local
- [ ] Identifico operadores peligrosos
- [ ] Entiendo casting de punteros

Si respondiste "no", **relée esa sección**.

---

## 🚀 Próximos Pasos

1. Escribe programas C pequeños y análizalos
2. Crea arrays y buffers, experimenta
3. Usa GDB para ver frames (Lección 04)
4. Practica aritmética de punteros

---

## 📚 Recursos Complementarios

- 📖 [recursos/glosario.md](../../recursos/glosario.md) - Términos C
- 📖 [recursos/herramientas.md](../../recursos/herramientas.md) - Compiladores
- 📚 [Ejercicios](../../02-practicas/) - Practica todo esto

---

**Última actualización:** Mayo 2026  
**Tiempo total de lectura:** ~75 minutos  
**Dificultad:** ⭐⭐⭐ (intermedio)  
**Estado:** ✅ Completa

---

## 🎊 Resumen

Ahora entiendes:

✓ Por qué C y cómo funciona  
✓ Stack vs Heap  
✓ Punteros (lo más importante)  
✓ Arrays y buffers (donde ocurren los bugs)  
✓ Funciones y stack frames  
✓ Tipos de datos  
✓ Operadores peligrosos  

**Estás listo para analizar binarios en profundidad.**

Próxima parada: [Lección 04: GDB - El Debugger](leccion_04.md)

🚀

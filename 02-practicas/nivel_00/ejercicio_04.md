# 🎯 Ejercicio 04: Mi Primer Puntero

## 📋 Objetivo de Aprendizaje

Después de completar este ejercicio, podrás:

- ✅ Entender qué es un puntero
- ✅ Crear punteros en C
- ✅ Usar el operador `&` (dirección)
- ✅ Usar el operador `*` (dereferenciación)
- ✅ Modificar valores a través de punteros
- ✅ Entender por qué los punteros son críticos en seguridad

**Tiempo estimado:** 40-50 minutos  
**Dificultad:** ⭐⭐ (fácil-intermedio)  
**Pre-requisito:** [Ejercicio 03](ejercicio_03.md)

---

## 📖 Introducción

En el ejercicio anterior vimos **direcciones de memoria**.

Ahora vamos a aprender a **trabajar con ellas** usando **punteros**.

Los punteros son probablemente el concepto **más importante** en seguridad informática. Debes dominarlos completamente.

---

## 🎯 Parte 1: Entender Punteros

### ¿Qué Es un Puntero?

**Un puntero es una variable que almacena una dirección de memoria.**

```
Variable normal:          Variable puntero:
┌─────────────┐          ┌─────────────┐
│ numero = 42 │          │ ptr = 0x... │  ← Dirección de otra variable
└─────────────┘          └─────────────┘
```

### Analogía: Una Dirección Postal

```
Variable normal:
┌──────────────────────────┐
│ Tu casa: 123 Oak Street  │  ← El lugar real
└──────────────────────────┘

Puntero:
┌──────────────────────────┐
│ "123 Oak Street"         │  ← No es la casa, es la DIRECCIÓN
└──────────────────────────┘
```

Un puntero **apunta a** una dirección. No es el dato, es donde vive el dato.

### Sintaxis en C

```c
int numero = 42;      // Variable normal
int *ptr = &numero;   // Puntero a numero

// Explicación:
int *ptr              // Declara un puntero a int
    = &numero         // Inicializa con la dirección de numero
```

---

## 🎯 Parte 2: Los Dos Operadores Clave

### Operador 1: `&` (Dirección)

```c
int numero = 42;
printf("%p\n", &numero);   // &numero = la dirección de numero
```

**Resultado:**
```
0x7ffeefbff4ec
```

`&numero` = "Dame la dirección donde vive `numero`"

### Operador 2: `*` (Dereferenciación)

```c
int *ptr = &numero;       // ptr apunta a numero
printf("%d\n", *ptr);     // *ptr = el valor en esa dirección
```

**Resultado:**
```
42
```

`*ptr` = "Dame el valor en la dirección que apunta `ptr`"

### Visual

```
MEMORIA:
┌──────────────────────────────────────┐
│ 0x7ffeefbff4ec: [42]                 │  ← numero = 42
├──────────────────────────────────────┤
│ 0x7ffeefbff4f0: [0x7ffeefbff4ec]     │  ← ptr = &numero
└──────────────────────────────────────┘

En el código:
numero             → 42                    (el valor)
&numero            → 0x7ffeefbff4ec       (la dirección)
ptr                → 0x7ffeefbff4ec       (lo que contiene ptr)
*ptr               → 42                    (el valor que apunta ptr)
```

---

## 🎯 Parte 3: Crear un Programa con Punteros

```bash
cd /Users/yavier/SnowCrash/02-practicas/nivel_00

cat > ejercicio_04.c << 'EOF'
#include <stdio.h>

int main() {
    // Parte 1: Variable normal
    int numero = 42;
    
    // Parte 2: Crear un puntero
    int *ptr = &numero;
    
    // Parte 3: Acceder al valor a través del puntero
    printf("numero = %d\n", numero);
    printf("*ptr = %d\n", *ptr);
    printf("&numero = %p\n", &numero);
    printf("ptr = %p\n", ptr);
    
    // Parte 4: Modificar el valor a través del puntero
    *ptr = 999;
    printf("\nDespués de *ptr = 999:\n");
    printf("numero = %d\n", numero);
    printf("*ptr = %d\n", *ptr);
    
    return 0;
}
EOF
```

### Compila y Ejecuta

```bash
gcc -g -o ejercicio_04 ejercicio_04.c
./ejercicio_04
```

**Salida esperada:**

```
numero = 42
*ptr = 42
&numero = 0x7ffeefbff4ec
ptr = 0x7ffeefbff4ec

Después de *ptr = 999:
numero = 42
*ptr = 999
```

¡Espera! ¿Por qué `numero` sigue siendo 42 si lo modificamos a través del puntero?

(Esto es porque no compilamos con `-g` y hay optimizaciones. Compila así:)

```bash
gcc -g -O0 -o ejercicio_04 ejercicio_04.c
./ejercicio_04
```

Ahora debería mostrar:

```
numero = 42
*ptr = 42
&numero = 0x7ffeefbff4ec
ptr = 0x7ffeefbff4ec

Después de *ptr = 999:
numero = 999
*ptr = 999
```

**¡Ahora funcionó!** Modificamos `numero` **a través del puntero**.

---

## 💡 Pistas (Si Te Atrapas)

### "No entiendo el `*` y el `&`"

Recuerda:

```
&   = "Dirección de"
*   = "Valor que apunta"

int numero = 42;
int *ptr = &numero;

&numero  → "Dirección de numero" → 0x...
*ptr     → "Valor que apunta ptr" → 42
```

### "Mis valores no cambian"

Necesitas compilar sin optimizaciones:

```bash
gcc -g -O0 -o ejercicio_04 ejercicio_04.c
```

El `-O0` desactiva optimizaciones que confunden punteros.

### "Me sale un error de compilación"

```
error: cannot assign to variable of type 'int *' with an expression of type 'int'
```

Asegúrate de declarar el puntero correctamente:

```c
int *ptr;      // Correcto (puntero a int)
int ptr;       // Incorrecto (int normal)
```

---

## ✅ Solución Completa

### Paso 1: Crear el archivo
```bash
cat > ejercicio_04.c << 'EOF'
#include <stdio.h>

int main() {
    int numero = 42;
    int *ptr = &numero;
    
    printf("numero = %d\n", numero);
    printf("*ptr = %d\n", *ptr);
    printf("&numero = %p\n", &numero);
    printf("ptr = %p\n", ptr);
    
    *ptr = 999;
    printf("\nDespués de *ptr = 999:\n");
    printf("numero = %d\n", numero);
    printf("*ptr = %d\n", *ptr);
    
    return 0;
}
EOF
```

### Paso 2: Compilar
```bash
gcc -g -O0 -o ejercicio_04 ejercicio_04.c
```

### Paso 3: Ejecutar
```bash
./ejercicio_04
```

### Paso 4: Analizar con GDB (Opcional pero Recomendado)
```bash
gdb ./ejercicio_04
(gdb) break main
(gdb) run
(gdb) next
(gdb) print numero
(gdb) print ptr
(gdb) print &numero
(gdb) print *ptr
```

---

## 📚 Explicación Detallada

### ¿Por Qué Los Punteros Son Importantes?

Los punteros permiten:

1. **Pasar variables por referencia** - Modificar una variable dentro de una función
2. **Asignación dinámica** - Crear memoria durante la ejecución
3. **Estructuras complejas** - Listas, árboles, grafos
4. **Acceso bajo nivel** - Manipular memoria directamente

En seguridad, los punteros son **críticos** porque:

- **Buffer overflow:** Sobreescribir punteros cambia el flujo del programa
- **Return pointer:** El puntero de retorno determina dónde vuelve la función
- **Heap corruption:** Cambiar punteros en el heap compromete todo

### Ejemplo: Punteros a Caracteres

```bash
cat > ejemplo_ptr_char.c << 'EOF'
#include <stdio.h>

int main() {
    char nombre[] = "Juan";
    char *ptr = nombre;
    
    printf("nombre[0] = %c\n", nombre[0]);
    printf("*ptr = %c\n", *ptr);
    
    // Avanza el puntero
    ptr++;
    printf("*(ptr+1) = %c\n", *ptr);
    
    return 0;
}
EOF

gcc -O0 -o ejemplo_ptr_char ejemplo_ptr_char.c
./ejemplo_ptr_char
```

**Salida:**
```
nombre[0] = J
*ptr = J
*(ptr+1) = u
```

Nota: `ptr++` avanza el puntero a la siguiente dirección.

### Aritmética de Punteros

```c
int array[5] = {10, 20, 30, 40, 50};
int *ptr = array;

*ptr         // 10 (elemento 0)
*(ptr + 1)   // 20 (elemento 1)
*(ptr + 2)   // 30 (elemento 2)
ptr[2]       // 30 (equivalente a *(ptr + 2))
```

---

## 🔍 Visualizar Punteros con GDB

Para entender mejor, usa GDB:

```bash
gdb ./ejercicio_04

(gdb) break main
(gdb) run
(gdb) next
(gdb) next    # Ejecuta la línea del puntero

(gdb) print numero
$1 = 42

(gdb) print ptr
$2 = (int *) 0x7ffeefbff4ec

(gdb) print &numero
$3 = (int *) 0x7ffeefbff4ec

(gdb) print *ptr
$4 = 42
```

Nota que `ptr` y `&numero` **apuntan a la misma dirección**.

---

## 🎯 Variaciones (Experimenta)

### Variación 1: Múltiples Punteros

```bash
cat > var1.c << 'EOF'
#include <stdio.h>

int main() {
    int a = 10;
    int b = 20;
    
    int *ptr_a = &a;
    int *ptr_b = &b;
    
    printf("a = %d, b = %d\n", a, b);
    printf("*ptr_a = %d, *ptr_b = %d\n", *ptr_a, *ptr_b);
    
    *ptr_a = 100;
    *ptr_b = 200;
    
    printf("Después de modificar:\n");
    printf("a = %d, b = %d\n", a, b);
    
    return 0;
}
EOF

gcc -O0 -o var1 var1.c
./var1
```

### Variación 2: Puntero a Puntero

```bash
cat > var2.c << 'EOF'
#include <stdio.h>

int main() {
    int numero = 42;
    int *ptr = &numero;
    int **ptr_ptr = &ptr;      // Puntero a puntero
    
    printf("numero = %d\n", numero);
    printf("*ptr = %d\n", *ptr);
    printf("**ptr_ptr = %d\n", **ptr_ptr);  // Dos * = acceso doble
    
    return 0;
}
EOF

gcc -O0 -o var2 var2.c
./var2
```

### Variación 3: Puntero a Array

```bash
cat > var3.c << 'EOF'
#include <stdio.h>

int main() {
    int array[5] = {10, 20, 30, 40, 50};
    int *ptr = array;          // Puntero al primer elemento
    
    printf("array[0] = %d\n", array[0]);
    printf("*ptr = %d\n", *ptr);
    printf("*(ptr + 2) = %d\n", *(ptr + 2));
    printf("ptr[3] = %d\n", ptr[3]);
    
    return 0;
}
EOF

gcc -O0 -o var3 var3.c
./var3
```

---

## 🔮 Conexión con lo Siguiente

### Próximas Lecciones:

**Lección 02** - **Binarios y Memoria**:
- Cómo el compilador organiza punteros en el binario
- Reubicación de punteros en runtime

**Lección 03** - **C Fundamentals**:
- Punteros a funciones
- Punteros generales (void *)
- Arreglos y punteros

### Vulnerabilidades Futuras:

**Buffer Overflow:**
Sobrescribir un puntero en el stack para apuntar a código malicioso.

**Heap Corruption:**
Corromper punteros en el heap para redirigir la ejecución.

**Return-Oriented Programming (ROP):**
Cambiar punteros de retorno para ejecutar gadgets (pequeñas secuencias de código).

---

## ✨ Checklist de Completitud

- [ ] Entiendo qué es un puntero
- [ ] Entiendo el operador `&` (dirección)
- [ ] Entiendo el operador `*` (dereferenciación)
- [ ] Creé el archivo `ejercicio_04.c`
- [ ] Compilé con `gcc -g -O0`
- [ ] Ejecuté y vi la salida
- [ ] Modifiqué un valor a través de un puntero
- [ ] Usé GDB para inspeccionar punteros
- [ ] Completé al menos una variación
- [ ] Puedo explicar la diferencia entre `ptr` y `*ptr`

Si checaste todo: ✅ **¡Ejercicio completado!**

---

## 🚀 Próximo Paso

¡Felicidades! Completaste el **Nivel 00** (los 4 ejercicios básicos).

Ya sabes:
- ✅ Compilar programas
- ✅ Analizar binarios estáticamente
- ✅ Entender memoria y direcciones
- ✅ Usar punteros

Ahora estás listo para el **Nivel 01** (intermedio).

👉 Ve a [Nivel 01 - Ejercicio 01: GDB Básico](../nivel_01/ejercicio_01.md)

---

## 📚 Recursos Relacionados

- 📖 [recursos/herramientas.md](../../recursos/herramientas.md) - Compiladores, debugging
- 📖 [recursos/gdb-guia.md](../../recursos/gdb-guia.md) - Tutorial GDB completo
- 📖 [recursos/glosario.md](../../recursos/glosario.md) - "Puntero", "Referencia", "Dereferenciación"
- 📚 [Lección 03](../leccion_03.md) - C Fundamentals (próximo a crear)

---

**Dificultad:** ⭐⭐  
**Tiempo real:** ~45 minutos  
**Conceptos clave:** Punteros, operadores & y *, dereferenciación, aritmética de punteros  
**Estado:** ✅ Completo

---

¡Excelente! Dominaste los punteros. Ahora estás listo para lo intermedio. 🚀

**Siguiente:** [Nivel 01 - Ejercicio 01](../nivel_01/ejercicio_01.md)

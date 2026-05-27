# 🎯 Ejercicio 03: Entendiendo la Memoria

## 📋 Objetivo de Aprendizaje

Después de completar este ejercicio, podrás:

- ✅ Entender cómo se organiza la memoria en un programa
- ✅ Ver direcciones de memoria en acción
- ✅ Comprender la diferencia entre Stack y datos
- ✅ Usar `gdb` para inspeccionar variables en memoria
- ✅ Entender cómo el código se almacena en la memoria

**Tiempo estimado:** 30-45 minutos  
**Dificultad:** ⭐⭐ (fácil)  
**Pre-requisito:** [Ejercicio 02](ejercicio_02.md)

---

## 📖 Introducción

Hasta ahora has **compilado y analizado** un binario.

Pero ¿qué pasa **adentro** cuando el programa se ejecuta?

¿Dónde viven las variables? ¿Cómo se almacenan los datos? ¿Cuál es la diferencia entre el código y los datos?

Eso es lo que aprenderás en este ejercicio.

---

## 🎯 Parte 1: Crear un Programa con Variables

Vamos a crear un programa que **declara y usa variables**.

```bash
cd /Users/yavier/SnowCrash/02-practicas/nivel_00

cat > ejercicio_03.c << 'EOF'
#include <stdio.h>

int main() {
    // Variable 1: Un número
    int numero = 42;
    
    // Variable 2: Otro número
    int otro = 100;
    
    // Variable 3: Un carácter
    char letra = 'A';
    
    // Imprime las variables
    printf("numero = %d\n", numero);
    printf("otro = %d\n", otro);
    printf("letra = %c\n", letra);
    
    return 0;
}
EOF
```

### Compila el Programa

```bash
gcc -g -o ejercicio_03 ejercicio_03.c
```

**Nota:** El flag `-g` es **muy importante**. Incluye información de debugging en el binario, que necesitaremos para `gdb`.

### Ejecuta el Programa

```bash
./ejercicio_03
```

**Salida esperada:**

```
numero = 42
otro = 100
letra = A
```

Bien, funciona. Ahora vamos a ver qué **realmente** está pasando en la memoria.

---

## 🎯 Parte 2: Entender la Memoria

### Concepto Importante: La Memoria de un Programa

Cuando un programa corre, la memoria se divide en secciones:

```
MEMORIA DE UN PROGRAMA:
┌─────────────────────────────────────┐
│         CÓDIGO (Code)               │  ← Las instrucciones del programa
├─────────────────────────────────────┤
│    DATOS INICIALIZADOS (Data)       │  ← Variables globales inicializadas
├─────────────────────────────────────┤
│   DATOS NO INICIALIZADOS (BSS)      │  ← Variables globales no inicializadas
├─────────────────────────────────────┤
│          HEAP (Dinámico)            │  ← Memoria asignada dinamicamente
├─                                   -┤
│          (vacío)                    │
├─                                   -┤
│        STACK (Estático)             │  ← Variables locales
└─────────────────────────────────────┘

Dirección más baja (0x0)   →
                          →
Dirección más alta        →
```

### En Nuestro Programa:

```c
int main() {
    int numero = 42;        // ← Aquí va al STACK
    int otro = 100;         // ← Aquí también al STACK
    char letra = 'A';       // ← Aquí también al STACK
    ...
}
```

Las variables locales (dentro de una función) van al **STACK**.

---

## 🎯 Parte 3: Usar GDB para Ver Direcciones

Ahora usaremos `gdb` (el debugger) para ver **exactamente dónde están** las variables en memoria.

### Abre GDB

```bash
gdb ./ejercicio_03
```

Verás algo como:

```
GNU gdb (GDB) 14.1
...
(gdb)
```

### Establece un Breakpoint

Un **breakpoint** es un punto donde el programa se detiene para que lo analices.

```
(gdb) break main
Breakpoint 1 at 0x100003e7a
```

Esto dice: "Detente en la función `main`".

### Ejecuta el Programa Hasta el Breakpoint

```
(gdb) run
Starting program: ./ejercicio_03
[...]
Breakpoint 1, main () at ejercicio_03.c:3
3     int numero = 42;
```

El programa se detuvo **antes de ejecutar la línea 3**.

### Ve la Siguiente Línea

```
(gdb) next
```

Esto ejecuta la siguiente línea (la del `int numero = 42;`).

### Inspecciona una Variable

```
(gdb) print numero
$1 = 42
```

¡Ahí está! El valor de `numero` es 42.

### Ahora Mira la Dirección de Memoria

```
(gdb) print &numero
$2 = (int *) 0x7ffeefbff4ec
```

**¡IMPORTANTE!** El `&` significa "dirección de". `&numero` es la **dirección en memoria** donde se almacena `numero`.

```
$2 = (int *) 0x7ffeefbff4ec
     ↑          ↑
    Resultado   Dirección de memoria (en hexadecimal)
```

### Inspecciona Más Variables

```
(gdb) print &otro
$3 = (int *) 0x7ffeefbff4e8

(gdb) print &letra
$4 = (char *) 0x7ffeefbff4e7
```

### Mira Todas las Variables Locales

```
(gdb) info locals
numero = 42
otro = 100
letra = 65
```

Nota: `letra` aparece como 65 porque internamente los caracteres son números. (En ASCII, 'A' = 65)

### Sal de GDB

```
(gdb) quit
```

---

## 💡 Pistas (Si Te Atrapas)

### "No funciona `gdb`"
```bash
# Intenta con la ruta completa
/usr/bin/gdb ./ejercicio_03

# O instálalo
sudo dnf install gdb
```

### "El breakpoint no funciona"
```
# Asegúrate de haber compilado CON -g
gcc -g -o ejercicio_03 ejercicio_03.c

# Luego intenta de nuevo
gdb ./ejercicio_03
(gdb) break main
(gdb) run
```

### "Estoy confundido con los comandos"
```
En gdb, puedes escribir:
(gdb) help           # Ver ayuda general
(gdb) help print     # Ayuda específica
(gdb) help break     # Ayuda de breakpoints
```

---

## ✅ Solución Completa

### Paso 1: Crear el archivo
```bash
cat > ejercicio_03.c << 'EOF'
#include <stdio.h>

int main() {
    int numero = 42;
    int otro = 100;
    char letra = 'A';
    
    printf("numero = %d\n", numero);
    printf("otro = %d\n", otro);
    printf("letra = %c\n", letra);
    
    return 0;
}
EOF
```

### Paso 2: Compilar CON -g
```bash
gcc -g -o ejercicio_03 ejercicio_03.c
```

### Paso 3: Ejecutar normalmente
```bash
./ejercicio_03
```

### Paso 4: Analizar con gdb
```bash
gdb ./ejercicio_03
(gdb) break main
(gdb) run
(gdb) next
(gdb) print numero
(gdb) print &numero
(gdb) print &otro
(gdb) print &letra
(gdb) info locals
(gdb) quit
```

---

## 📚 Explicación Detallada

### ¿Qué Es una Dirección de Memoria?

```
MEMORIA:
┌─────────────────────────────────────┐
│ 0x7ffeefbff4e7: [65]               │  ← letra (carácter 'A' = 65)
│ 0x7ffeefbff4e8: [100, 0, 0, 0]     │  ← otro (int = 4 bytes)
│ 0x7ffeefbff4ec: [42, 0, 0, 0]      │  ← numero (int = 4 bytes)
└─────────────────────────────────────┘
```

Cada dirección es una **ubicación específica en la RAM**.

```
0x7ffeefbff4ec = Una dirección hexadecimal
                = Una ubicación específica en la RAM
```

### ¿Por Qué Importan las Direcciones?

En seguridad ofensiva, necesitas saber dónde están las cosas en memoria para:

1. **Leerlas** - Extraer datos secretos
2. **Modificarlas** - Cambiar valores (overflow)
3. **Saltarlas** - Cambiar el flujo del programa

### ¿Qué Es el Stack?

El **stack** es una estructura de memoria especial:

```
STACK (crece hacia abajo):

┌─────────────────────────────────────┐
│ Top del stack                       │  ← Aquí van las variables nuevas
├─────────────────────────────────────┤
│ variable_1                          │
├─────────────────────────────────────┤
│ variable_2                          │
├─────────────────────────────────────┤
│ ...                                 │
└─────────────────────────────────────┘
```

Cuando entras en una función:
- ✅ Se reserva espacio en el stack para variables locales
- ✅ Cuando la función termina, se libera ese espacio

---

## 🎯 Variaciones (Experimenta)

### Variación 1: Más Variables

```bash
cat > ejercicio_03.c << 'EOF'
#include <stdio.h>

int main() {
    int a = 1;
    int b = 2;
    int c = 3;
    int d = 4;
    int e = 5;
    
    printf("a=%d, b=%d, c=%d, d=%d, e=%d\n", a, b, c, d, e);
    
    return 0;
}
EOF

gcc -g -o ejercicio_03 ejercicio_03.c
gdb ./ejercicio_03
(gdb) break main
(gdb) run
(gdb) next
(gdb) print &a
(gdb) print &b
(gdb) print &c
```

**¿Qué ves?** Las direcciones están todas cerca unas de otras (en el stack).

### Variación 2: Variables de Diferentes Tipos

```bash
cat > ejercicio_03.c << 'EOF'
#include <stdio.h>

int main() {
    char c = 'X';         // 1 byte
    int i = 999;          // 4 bytes
    long long ll = 123;   // 8 bytes
    
    printf("char=%c, int=%d, long long=%lld\n", c, i, ll);
    
    return 0;
}
EOF

gcc -g -o ejercicio_03 ejercicio_03.c
gdb ./ejercicio_03
(gdb) break main
(gdb) run
(gdb) next
(gdb) print &c
(gdb) print &i
(gdb) print &ll
```

**¿Qué ves?** Las direcciones varían según el tamaño del tipo.

---

## 🔮 Conexión con lo Siguiente

### Próximamente (Ejercicio 04):

Usaremos estas direcciones de memoria en un programa que explora punteros.

### Lección 02:

Aprenderemos sobre la **estructura ELF** y cómo el SO organiza la memoria.

### Lección 04:

Usaremos `gdb` avanzadamente para explorar **stack frames** y **llamadas de función**.

### Vulnerabilidades Futuras:

Los buffer overflows aprovechan el hecho de que el stack es **una zona continua en memoria**. Si escribes más datos de los que caben, sobrescribes variables vecinas o la dirección de retorno.

---

## ✨ Checklist de Completitud

- [ ] Creé el archivo `ejercicio_03.c`
- [ ] Compilé con `gcc -g -o ejercicio_03 ejercicio_03.c`
- [ ] Ejecuté `./ejercicio_03` y vi la salida
- [ ] Abrí gdb con `gdb ./ejercicio_03`
- [ ] Establecí un breakpoint en `main`
- [ ] Ejecuté `run` para llegar al breakpoint
- [ ] Ejecuté `next` para avanzar línea
- [ ] Usé `print numero` para ver el valor
- [ ] Usé `print &numero` para ver la dirección
- [ ] Vi las direcciones de múltiples variables
- [ ] Usé `info locals` para ver todas
- [ ] Salí de gdb con `quit`

Si checaste todo: ✅ **¡Ejercicio completado!**

---

## 🚀 Próximo Paso

Ahora que entiendes cómo se almacenan las variables en memoria:

👉 Ve a [Ejercicio 04: Mi Primer Puntero](ejercicio_04.md)

Ahí aprenderás a **manipular direcciones de memoria** usando punteros.

---

## 📚 Recursos Relacionados

- 📖 [recursos/herramientas.md](../../recursos/herramientas.md) - GDB basics
- 📖 [recursos/gdb-guia.md](../../recursos/gdb-guia.md) - Tutorial completo de GDB
- 📖 [recursos/glosario.md](../../recursos/glosario.md) - "Stack", "Dirección", "Pointer"
- 📚 [Lección 01](../leccion_01.md) - Conceptos generales

---

**Dificultad:** ⭐⭐  
**Tiempo real:** ~35 minutos  
**Conceptos clave:** Stack, direcciones de memoria, GDB, variables locales  
**Estado:** ✅ Completo

---

¡Excelente! Ya entiende cómo funciona la memoria. 🧠

**Siguiente:** [Ejercicio 04](ejercicio_04.md)

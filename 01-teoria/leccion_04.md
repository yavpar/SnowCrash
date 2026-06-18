# Lección 04: GDB - El Debugger

## Objetivos de Aprendizaje

- Entender qué es un debugger y por qué es esencial
- Aprender comandos fundamentales de GDB
- Usar GDB para inspeccionar código en ejecución
- Detectar comportamientos inesperados (bugs, vulnerabilidades)

## Tiempo Estimado

60-90 minutos de lectura

## Dificultad

⭐⭐ Intermedio

---

## Parte 1: ¿Qué es GDB y Por Qué es Crítico?

### El Problema sin Debugger

Imagina que tienes un programa que:
- Imprime valores extraños
- Se crashea sin razón clara
- Tiene comportamiento inesperado

**Sin debugger:** Solo ves la salida final y tienes que adivinar qué salió mal.

**Con debugger:** Ves exactamente qué está sucediendo en cada paso.

### Ejemplo Real

```c
int main() {
    int x = 5;
    int y = 0;
    int resultado = x / y;  // ¡División por cero!
    printf("Resultado: %d\n", resultado);
    return 0;
}
```

**Sin GDB:**
```
$ ./programa
Floating point exception (core dumped)
```

¿Qué línea? ¿Por qué? No sabes.

**Con GDB:**
```
(gdb) run
Floating point exception, first instance

(gdb) backtrace
#0  0x00005555554011ad in main() at programa.c:5
```

¡Sabes exactamente que es la línea 5!

---

## Parte 2: Compilación para Debugging

### Flag Fundamental: -g

Para que GDB funcione, **debes compilar con -g**:

```bash
# CON símbolos de debug (necesario para GDB)
gcc -g -O0 -o programa programa.c

# SIN símbolos de debug (GDB no funciona bien)
gcc -o programa programa.c
```

**¿Qué hace -g?**

Agrega "símbolos de debug" al binario. Esto permite que GDB:
- Sepa qué línea de C corresponde a cada instrucción
- Muestre nombres de variables (en lugar de direcciones)
- Entienda la estructura del programa

**¿Qué hace -O0?**

Compila SIN optimizar. Esto es importante porque:
- Las optimizaciones pueden reorganizar código
- Variables pueden desaparecer (el compilador las elimina)
- El flujo de ejecución puede cambiar

**Resultado:**

```bash
gcc -g -O0 -o programa programa.c
```

Con esto, GDB puede mostrarte el programa exactamente como escribiste.

---

## Parte 3: Conceptos Fundamentales de GDB

### 1. Los Tres Estados de GDB

GDB tiene tres modos de operación:

```
ESTADO 1: CARGADO (el programa está en memoria, pero no corriendo)
    ↓
(gdb) run
    ↓
ESTADO 2: CORRIENDO (ejecutándose normalmente)
    ↓
(si hay breakpoint o se crashea)
    ↓
ESTADO 3: PARADO (en un breakpoint, puedes inspeccionar)
    ↓
(gdb) continue
    ↓
Vuelve a ESTADO 2
```

### 2. Breakpoints - Pausar la Ejecución

Un **breakpoint** es un "punto de pausa" que le dices a GDB:

"Cuando llegues a esta línea, PAUSA la ejecución."

```bash
# Crear breakpoint en línea 5
(gdb) break programa.c:5

# Crear breakpoint en función main
(gdb) break main

# Ver todos los breakpoints
(gdb) info breakpoints
```

**¿Por qué es útil?**

Cuando el programa está parado en un breakpoint, puedes:
- Ver el valor de variables
- Ver dónde estás en el código
- Inspeccionar memoria
- Continuar línea por línea

### 3. Stepping - Ejecutar Línea por Línea

Una vez parado en un breakpoint, puedes avanzar lentamente:

```bash
# Ejecutar siguiente línea de C
(gdb) next

# Ejecutar siguiente línea, entrando en funciones
(gdb) step

# Ejecutar siguiente línea de assembly (más bajo nivel)
(gdb) stepi
```

**Diferencia entre next y step:**

```c
void funcion() { printf("Hola\n"); }

int main() {
    funcion();    // Línea 5
    printf("Fin\n");
    return 0;
}
```

Con `next` en línea 5:
- Salta SOBRE funcion()
- Va directo a línea 6

Con `step` en línea 5:
- ENTRA en funcion()
- Ve el printf dentro

---

## Parte 4: Workflow Práctico Básico

### Ejemplo: Encontrar un Bug

Código con bug:

```c
#include <stdio.h>

int sumar(int a, int b) {
    int resultado = a + b;
    return resultado;
}

int main() {
    int x = 5;
    int y = 3;
    int z = sumar(x, y);
    printf("5 + 3 = %d\n", z);
    return 0;
}
```

Este código es correcto, pero simularemos debugging:

### Paso 1: Compilar con -g

```bash
gcc -g -O0 -o programa programa.c
```

### Paso 2: Iniciar GDB

```bash
gdb ./programa
```

Verás:

```
GNU gdb (GDB) 12.1
...
(gdb) _
```

### Paso 3: Poner Breakpoint en main

```bash
(gdb) break main
Breakpoint 1 at 0x1169
```

### Paso 4: Ejecutar

```bash
(gdb) run
Starting program: ./programa

Breakpoint 1, main() at programa.c:10
10    int x = 5;
```

**¿Qué pasó?**

- El programa se paró en línea 10 (donde está main)
- GDB muestra qué línea es

### Paso 5: Ver Dónde Estás

```bash
(gdb) list
10    int x = 5;
11    int y = 3;
12    int z = sumar(x, y);
13    printf("5 + 3 = %d\n", z);
14    return 0;
```

Ves el contexto alrededor del breakpoint.

### Paso 6: Avanzar Línea por Línea

```bash
(gdb) next
11    int y = 3;

(gdb) next
12    int z = sumar(x, y);
```

### Paso 7: Ver Valor de Variable

```bash
(gdb) print x
$1 = 5

(gdb) print y
$2 = 3

(gdb) print z
$3 = 0    # Aún no se ha calculado
```

### Paso 8: Entrar en Función

```bash
(gdb) step
sumar (a=5, b=3) at programa.c:4
4    int resultado = a + b;
```

**📌 Concepto nuevo: Argumentos de Función**

Los valores `a=5, b=3` que ves son los **argumentos** que se pasaron a `sumar()`.

GDB te los muestra automáticamente. (No profundizamos ahora, pero es importante saber que aparecen.)

### Paso 9: Ver Variable dentro de Función

```bash
(gdb) next
5    return resultado;

(gdb) print resultado
$4 = 8    # Se calculó correctamente
```

### Paso 10: Continuar Hasta el Fin

```bash
(gdb) continue
5 + 3 = 8
[Inferior 1 (process 12345) exited normally]
```

El programa terminó.

---

## Parte 5: Inspeccionar Memoria

### ¿Por Qué Inspeccionar Memoria?

A veces, ver el valor de una variable no es suficiente. Necesitas ver:
- ¿Qué hay alrededor de esta variable?
- ¿Se está sobrescribiendo algo?
- ¿Qué contiene esta dirección exactamente?

### Comando x (examine)

```bash
(gdb) print &x
$1 = 0x7ffffffde4c0

(gdb) x/4xb 0x7ffffffde4c0
0x7ffffffde4c0: 0x05    0x00    0x00    0x00
```

**Desglose del comando:**

```
x/4xb 0x7ffffffde4c0
│ │ │ │
│ │ │ └─ dirección de memoria
│ │ └─── b = byte (1 byte)
│ └───── x = hexadecimal
└─────── 4 = mostrar 4 bytes
```

**Traducción:**

"Examina 4 bytes en formato hexadecimal comenzando en 0x7ffffffde4c0"

**Resultado:**

```
0x05 0x00 0x00 0x00
│    │    │    │
│    │    │    └─ byte 4
│    │    └────── byte 3
│    └─────────── byte 2
└──────────────── byte 1 (valor 5)
```

Es el número 5 (0x05) almacenado en 4 bytes (como un int).

### Otros Formatos

```bash
# Ver como string
(gdb) x/s 0x7ffffffde4c0

# Ver como entero
(gdb) x/d 0x7ffffffde4c0

# Ver como dirección
(gdb) x/a 0x7ffffffde4c0
```

---

## Parte 6: Detectar Bugs en Tiempo Real

### Caso: Buffer Overflow (Adelanto)

📌 Concepto nuevo: **Buffer Overflow**

Un buffer overflow es cuando escribes más datos en una variable de lo que cabe en memoria. Esto sobrescribe otras variables.

(Lo veremos en detalle en Lección 06. Por ahora: es cuando algo "se desborda" de su espacio.)

### Código Vulnerable

```c
#include <string.h>

void procesar(char *entrada) {
    char buffer[10];
    strcpy(buffer, entrada);  // ¡Peligroso!
}

int main() {
    procesar("AAAAAAAAAAAAAAAA");  // 16 A's, pero buffer es 10
    return 0;
}
```

### Debuggear con GDB

```bash
gcc -g -O0 -fno-stack-protector -o programa programa.c
gdb ./programa
```

(El `-fno-stack-protector` desactiva una protección que veremos después.)

```bash
(gdb) break procesar
(gdb) run
Breakpoint 1, procesar (entrada=0x555555554010 "AAAAAAAAAAAAAAAA") at programa.c:4

(gdb) print &buffer
$1 = 0x7ffffffde4c0

(gdb) x/20x &buffer
0x7ffffffde4c0: 0x41    0x41    0x41    0x41    0x41    0x41    0x41    0x41
0x7ffffffde4c8: 0x41    0x41    0x41    0x41    0x41    0x41    0x41    0x41
                A      A      A      A      A      A      A      A
                A      A      A      A      A      A      A      A
```

**¿Qué ves?**

- Buffer tiene 10 bytes (0x41 = 'A')
- Pero aparecen 16 A's en memoria
- Las últimas 6 A's sobrescribieron algo que no les pertenece

**Eso es un buffer overflow.**

---

## Parte 7: Comandos Esenciales Resumido

| Comando | Descripción |
|---------|-------------|
| `run` | Ejecutar el programa |
| `break main` | Poner breakpoint en main |
| `break programa.c:10` | Poner breakpoint en línea 10 |
| `continue` | Continuar ejecución |
| `next` | Siguiente línea (sin entrar en funciones) |
| `step` | Siguiente línea (entra en funciones) |
| `print x` | Ver valor de variable x |
| `print &x` | Ver dirección de x |
| `x/4xb &x` | Ver 4 bytes en hex |
| `list` | Ver código alrededor del breakpoint |
| `backtrace` | Ver pila de llamadas |
| `quit` | Salir de GDB |

---

## Parte 8: Flujo de Debugging Típico

Cuando tienes un bug:

1. **Compilar con -g -O0**
   ```bash
   gcc -g -O0 -o programa programa.c
   ```

2. **Iniciar GDB**
   ```bash
   gdb ./programa
   ```

3. **Poner breakpoint cerca del problema**
   ```bash
   (gdb) break main
   ```

4. **Ejecutar**
   ```bash
   (gdb) run
   ```

5. **Avanzar línea por línea**
   ```bash
   (gdb) next
   (gdb) next
   ```

6. **Ver valores sospechosos**
   ```bash
   (gdb) print x
   (gdb) x/10x &buffer
   ```

7. **Si es necesario, entrar en función**
   ```bash
   (gdb) step
   ```

8. **Repetir hasta encontrar el bug**

---

## Parte 9: Concepto de Stack Frame (Introducción)

📌 Concepto nuevo: **Stack Frame**

Un stack frame es el espacio en memoria que se "reserva" cuando se llama a una función.

**Ejemplo visual:**

```c
void funcion() { }

int main() {
    funcion();  // Aquí se crea stack frame de funcion()
}
```

**En memoria:**

```
Stack antes de funcion():
[variables de main]
[dirección de retorno]

Stack dentro de funcion():
[variables de main]
[dirección de retorno]
[variables de funcion()]     ← Stack frame de funcion()
[nuevo RIP]                  ← (concepto: veremos después)
```

En GDB, puedes ver los stack frames:

```bash
(gdb) backtrace
#0  funcion() at programa.c:3
#1  0x00005555554011bd in main() at programa.c:6
```

Hay 2 frames:
- Frame #0: donde estamos (funcion)
- Frame #1: quién nos llamó (main)

⚠️ Nota: Los detalles de cómo funciona exactamente veremos en Lección 05. Por ahora: es la "zona de memoria" de cada función.

---

## Parte 10: Diferencias: printf() vs GDB

### Debugging con printf()

```c
printf("Valor de x: %d\n", x);
```

**Ventajas:**
- Rápido
- Simple

**Desventajas:**
- Tienes que recompilar
- Solo ves lo que decidiste mostrar
- No puedes interactuar

### Debugging con GDB

```bash
(gdb) print x
$1 = 5
```

**Ventajas:**
- No necesitas recompilar
- Ves lo que quieras, cuando quieras
- Puedes inspeccionar cualquier cosa
- Puedes cambiar el flujo de ejecución

**Desventajas:**
- Un poco más complejo
- Requiere compilar con -g

**Recomendación:** Aprende a usar GDB desde ahora. Es la herramienta más poderosa para debugging.

---

## Parte 11: Conceptos para Después

⚠️ Estos conceptos son importantes, pero los veremos en detalle en lecciones futuras:

- **Registros del CPU** (rax, rbx, rdi, etc.) → Lección 05
- **Assembly x86-64** (instrucciones de máquina) → Lección 05
- **Convención de llamadas** (cómo se pasan argumentos) → Lección 05
- **Return Address / RIP** (cómo retorna una función) → Lección 05
- **Stack Layout detallado** → Lección 05-06

Por ahora: solo necesitas saber que existen y que GDB puede ayudarte a verlos.

---

## Checkpoints

✅ Completaste cuando:

- [ ] Entiendes qué es GDB y por qué es importante
- [ ] Sabes compilar con -g -O0
- [ ] Puedes poner breakpoints
- [ ] Puedes ver valores de variables
- [ ] Puedes avanzar línea por línea
- [ ] Puedes inspeccionar memoria con x/
- [ ] Entiendes los 3 estados de GDB (cargado, corriendo, parado)
- [ ] Sabes la diferencia entre next y step

---

## Práctica Recomendada

Usa GDB en el **Ejercicio 01 de Nivel 01** (Debugging Básico) que ya completaste.

Ahora que entiendes la teoría, intenta:
1. Poner breakpoints en diferentes lugares
2. Ver cómo cambian las variables
3. Inspeccionar memoria de buffer
4. Detectar el overflow observando memoria

---

## Lecturas Relacionadas

- `recursos/gdb-guia.md` - Referencia profunda de GDB (creada después)
- `recursos/herramientas.md` - Sección GDB
- `Ejercicio 01 Nivel 01` - Práctica de debugging

## Próxima Lección

**Lección 05: Assembly y Registros** - Entenderás cómo funciona realmente el CPU

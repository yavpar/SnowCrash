# Lección 05: Assembly y Registros del CPU

## Objetivos de Aprendizaje

- Entender qué son los registros del CPU y para qué sirven
- Leer código assembly x86-64 básico
- Relacionar código C con su equivalente en assembly
- Entender cómo el compilador convierte C a instrucciones de máquina

## Tiempo Estimado

60-90 minutos de lectura

## Dificultad

⭐⭐ Intermedio

---

## Parte 1: ¿Qué es Assembly?

Cuando escribes código en C, el compilador lo traduce a **instrucciones de máquina** que el CPU entiende directamente.

**Assembly** es la representación legible de esas instrucciones de máquina. Es el lenguaje más cercano al hardware.

```
C:        int x = 5 + 3;
    ↓ compilador
Assembly: mov eax, 5
          add eax, 3
          mov [rbp-4], eax
    ↓ ensamblador
Máquina:  B8 05 00 00 00
          83 C0 03
          89 45 FC
```

**¿Por qué necesito saber assembly para seguridad?**

Porque cuando analizas un binario (programa compilado), **no tienes el código C original**. Solo tienes assembly. Para entender qué hace un programa, necesitas leerlo.

---

## Parte 2: Registros del CPU

### ¿Qué es un Registro?

📌 **Concepto nuevo: Registro**

Un **registro** es un espacio de memoria **dentro del propio CPU**, muy pequeño (8 bytes en x86-64) pero extremadamente rápido.

**Analogía:**

```
Memoria RAM = Bodega (grande, lenta)
Registros   = Mesa de trabajo (pequeña, rapidísima)

Cuando el CPU opera, trae datos de la "bodega"
a su "mesa de trabajo" (registros),
opera ahí, y guarda el resultado de vuelta.
```

### Registros Principales en x86-64

En la arquitectura x86-64 (tu computadora Fedora), hay 16 registros de propósito general:

```
rax   rbx   rcx   rdx
rsi   rdi   rsp   rbp
r8    r9    r10   r11
r12   r13   r14   r15
```

Más dos registros especiales:
```
rip   rflags
```

### ¿Para Qué Sirve Cada Uno?

Los más importantes ahora:

| Registro | Nombre completo | Uso principal |
|----------|----------------|---------------|
| `rax` | Accumulator | Valor de retorno de funciones |
| `rsp` | Stack Pointer | Apunta al tope del stack |
| `rbp` | Base Pointer | Apunta a la base del frame actual |
| `rip` | Instruction Pointer | Dirección de la instrucción actual |
| `rdi` | Destination Index | Primer argumento de función |
| `rsi` | Source Index | Segundo argumento de función |
| `rdx` | Data | Tercer argumento de función |

⚠️ Nota: Los argumentos de función (rdi, rsi, rdx...) serán importantes cuando veamos Buffer Overflow en Lección 06.

### Tamaños de Registros

📌 **Concepto nuevo: Tamaños de registros**

El mismo registro físico tiene diferentes nombres según cuántos bits uses:

```
64 bits: rax  (8 bytes) ← el completo
32 bits: eax  (4 bytes) ← mitad inferior
16 bits:  ax  (2 bytes) ← cuarto inferior
 8 bits:  al  (1 byte)  ← octavo inferior
```

Visualmente:

```
rax: [  8 bytes completos                ]
eax:                     [  4 bytes  ]
 ax:                           [ 2b  ]
 al:                              [1b]
```

**¿Por qué importa?**

Cuando lees assembly, verás `eax`, `edi`, `esi` frecuentemente. Son los mismos registros que `rax`, `rdi`, `rsi`, pero usando solo 32 bits (suficiente para int).

---

## Parte 3: Instrucciones Assembly Básicas

### mov - Mover datos

La instrucción más común. Copia un valor de un lugar a otro.

```assembly
mov rax, 5        ; rax = 5 (cargar valor literal)
mov rax, rbx      ; rax = rbx (copiar registro a registro)
mov rax, [rbp-4]  ; rax = memoria en dirección (rbp-4)
mov [rbp-4], rax  ; memoria[rbp-4] = rax
```

**Nota sobre los corchetes `[]`:**

```assembly
mov rax, rbp      ; rax = valor del registro rbp
mov rax, [rbp]    ; rax = valor en la DIRECCIÓN que contiene rbp
```

Los corchetes significan **"ve a esa dirección y lee/escribe ahí"**.

### add y sub - Aritmética

```assembly
add rax, 5    ; rax = rax + 5
add rax, rbx  ; rax = rax + rbx
sub rax, 3    ; rax = rax - 3
```

### push y pop - Stack

```assembly
push rax   ; Guardar rax en el stack (rsp -= 8, memoria[rsp] = rax)
pop rax    ; Recuperar valor del stack a rax (rax = memoria[rsp], rsp += 8)
```

### call y ret - Funciones

```assembly
call funcion   ; Guardar dirección de retorno y saltar a funcion
ret            ; Recuperar dirección de retorno y saltar ahí
```

⚠️ Nota: `call` y `ret` son fundamentales para entender Buffer Overflow (Lección 06).

### cmp y jmp - Comparación y Saltos

```assembly
cmp rax, 5    ; Comparar rax con 5 (no modifica rax)
je 0x400500   ; Saltar si igual (jump if equal)
jne 0x400500  ; Saltar si no igual (jump if not equal)
jmp 0x400500  ; Saltar siempre (jump unconditional)
```

---

## Parte 4: C vs Assembly

### Ejemplo 1: Asignación de Variable

**C:**
```c
int x = 5;
```

**Assembly:**
```assembly
mov DWORD PTR [rbp-4], 5
```

Traducción: "Escribe el valor 5 en memoria en la dirección rbp-4 (donde está x)".

`DWORD PTR` significa "4 bytes" (un int en C es 4 bytes).

### Ejemplo 2: Suma de Variables

**C:**
```c
int z = x + y;
```

**Assembly:**
```assembly
mov eax, DWORD PTR [rbp-4]   ; eax = x
add eax, DWORD PTR [rbp-8]   ; eax = x + y
mov DWORD PTR [rbp-12], eax  ; z = eax
```

Traducción:
1. Cargar x en eax
2. Sumar y a eax
3. Guardar resultado en z

### Ejemplo 3: Llamada a Función

**C:**
```c
int resultado = suma(5, 3);
```

**Assembly:**
```assembly
mov edi, 5          ; primer argumento (a) en edi
mov esi, 3          ; segundo argumento (b) en esi
call suma           ; llamar a suma
mov [rbp-4], eax    ; resultado = valor retornado (en rax)
```

📌 **Concepto nuevo: Convención de llamadas**

Cuando llamas a una función, los argumentos se pasan en registros específicos en orden:

```
1er argumento → rdi
2do argumento → rsi
3er argumento → rdx
4to argumento → rcx
5to argumento → r8
6to argumento → r9
```

El valor que retorna la función siempre llega en `rax`.

Esto es una **convención**: un acuerdo estándar que todos los compiladores siguen en Linux x86-64.

---

## Parte 5: Stack Frame en Assembly

### ¿Qué Es un Stack Frame?

Cuando una función es llamada, el CPU necesita:
1. Saber dónde estaban las variables de la función anterior
2. Saber a dónde regresar cuando termine

Para eso, cada función crea un **stack frame**: su propio espacio privado en el stack.

### Estructura del Stack Frame

```
Stack (crece hacia abajo):

Dirección alta
    ┌──────────────────┐
    │  argumento 7+    │  (si hay más de 6 args)
    ├──────────────────┤
    │  return address  │  ← dirección a donde regresar
    ├──────────────────┤
    │  saved rbp       │  ← rbp de la función anterior
    ├──────────────────┤  ← rbp apunta aquí
    │  variable local 1│
    │  variable local 2│
    │  ...             │
    └──────────────────┘  ← rsp apunta aquí (tope)
Dirección baja
```

### Prólogo y Epílogo de Función

**Toda función en assembly comienza con un "prólogo":**

```assembly
push rbp          ; guardar rbp anterior
mov rbp, rsp      ; rbp = nuevo frame
sub rsp, 16       ; reservar espacio para variables locales
```

**Y termina con un "epílogo":**

```assembly
mov rsp, rbp      ; deshacer espacio de variables
pop rbp           ; restaurar rbp anterior
ret               ; regresar (usar return address)
```

### Ejemplo Completo

**Código C:**
```c
int suma(int a, int b) {
    int resultado = a + b;
    return resultado;
}
```

**Assembly generado:**
```assembly
suma:
    push rbp                      ; prólogo: guardar rbp
    mov rbp, rsp                  ; prólogo: nuevo frame
    mov [rbp-4], edi              ; guardar argumento a
    mov [rbp-8], esi              ; guardar argumento b
    mov eax, [rbp-4]              ; eax = a
    add eax, [rbp-8]              ; eax = a + b
    mov [rbp-12], eax             ; resultado = eax
    mov eax, [rbp-12]             ; eax = resultado (para retornar)
    pop rbp                       ; epílogo: restaurar rbp
    ret                           ; epílogo: regresar
```

---

## Parte 6: Usar GDB para Ver Assembly

### Disassemblar una Función

```bash
gdb ./programa
(gdb) disassemble main
```

Salida:

```
Dump of assembler code for function main:
   0x0000555555400526 <+0>:  push   rbp
   0x0000555555400527 <+1>:  mov    rbp,rsp
   0x000055555540052a <+4>:  sub    rsp,0x10
   0x000055555540052e <+8>:  mov    DWORD PTR [rbp-0x4],0x5
   0x0000555555400535 <+15>: mov    DWORD PTR [rbp-0x8],0x3
   0x000055555540053c <+22>: call   0x555555400500 <suma>
   0x0000555555400541 <+27>: mov    DWORD PTR [rbp-0xc],eax
   0x0000555555400544 <+30>: pop    rbp
   0x0000555555400545 <+31>: ret
```

### Ver Registros en Tiempo Real

```bash
(gdb) break main
(gdb) run
(gdb) info registers
rax            0x0
rbx            0x0
rcx            0x7ffff7dd8000
rdx            0x7fffffffe8a8
rsi            0x7fffffffe898
rdi            0x1
rbp            0x7fffffffe4c0
rsp            0x7fffffffe4b0
rip            0x555555400526
```

### Ver Registro Específico

```bash
(gdb) print $rax
$1 = 0

(gdb) print $rip
$2 = 0x555555400526

(gdb) print $rsp
$3 = 0x7fffffffe4b0
```

---

## Parte 7: Usando objdump para Análisis Estático

### ¿Qué es Análisis Estático?

Analizar el binario **sin ejecutarlo**. Útil cuando:
- No quieres ejecutar código peligroso
- Quieres entender la estructura general primero

### Disassemblar con objdump

```bash
objdump -d -M intel programa
```

`-M intel` usa sintaxis Intel (más legible que la sintaxis AT&T por defecto).

**Diferencia entre syntaxis:**

```
AT&T (defecto):    mov    %edi,-0x4(%rbp)
Intel (más claro): mov    DWORD PTR [rbp-0x4],edi
```

En Intel: destino primero, luego fuente. Más natural de leer.

### Filtrar por Función

```bash
objdump -d -M intel programa | grep -A 20 "<suma>:"
```

Muestra solo la función `suma` y las 20 líneas siguientes.

---

## Parte 8: Lo Que Necesitas Recordar

Para los ejercicios siguientes, lo fundamental es:

1. **Registros importantes:**
   - `rsp` = tope del stack
   - `rbp` = base del frame actual
   - `rip` = instrucción actual (qué está ejecutando)
   - `rdi, rsi, rdx` = argumentos 1, 2, 3 de función

2. **El stack crece hacia abajo** (direcciones decrecientes)

3. **Cada función tiene su stack frame** (espacio propio)

4. **El return address** es la dirección donde regresar al terminar la función. Está en el stack, justo encima del saved rbp.

5. **`ret` usa el return address** para saber a dónde saltar.

⚠️ Nota: Este punto 4 y 5 son **críticos** para Lección 06 (Buffer Overflow). Si controlas el return address, controlas a dónde salta el programa.

---

## Checkpoints

✅ Completaste cuando:

- [ ] Entiendes qué es un registro y para qué sirve
- [ ] Conoces los registros principales (rax, rsp, rbp, rip, rdi, rsi, rdx)
- [ ] Puedes leer instrucciones assembly básicas (mov, add, push, pop, call, ret)
- [ ] Entiendes cómo se pasan argumentos a funciones (convención de llamadas)
- [ ] Entiendes la estructura de un stack frame
- [ ] Puedes usar GDB para ver assembly y registros
- [ ] Puedes usar objdump para desensamblar un binario

---

## Práctica Recomendada

Usa **Ejercicio 02 del Nivel 01** (Disassembly Práctico con Objdump).

Ahora que entiendes la teoría, cada línea de assembly tendrá sentido.

---

## Lecturas Relacionadas

- `recursos/objdump-guia.md` - Guía completa de objdump
- `recursos/gdb-guia.md` - Sección "Disassembly y Registros"

## Próxima Lección

**Lección 06: Buffer Overflow** - Con esta base, entenderás exactamente cómo se explota un overflow.

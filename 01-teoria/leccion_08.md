# Lección 08: ROP y Protecciones Modernas

## Objetivos de Aprendizaje

- Entender qué ocurre cuando NX bit está activado
- Entender qué es Return-Oriented Programming (ROP)
- Entender qué es un gadget y cómo se encadenan
- Conectar todo lo aprendido en el curso

## Tiempo Estimado

45-60 minutos de lectura

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: El Problema - NX Bit Activado

En Ejercicio 04 desactivamos el NX bit con `-z execstack`. En el mundo real, está activado.

**¿Qué significa NX activado?**

El stack está marcado como **no ejecutable**. Aunque puedas escribir en él (overflow), el CPU rechaza ejecutar lo que escribiste ahí.

```
Sin NX:
    Escribes shellcode en el stack → CPU lo ejecuta ✅

Con NX:
    Escribes shellcode en el stack → CPU rechaza ejecutarlo ❌
    CPU lanza "Segmentation fault" (o similar)
```

**¿El overflow sigue funcionando?**

Sí. Sigues pudiendo sobrescribir el return address. La diferencia es que no puedes apuntar a shellcode en el stack.

**¿Dónde puedes apuntar?**

A código que **ya existe** en memoria y que sí es ejecutable: el código del propio programa y de las librerías (libc, etc.).

---

## Parte 2: ROP - Return-Oriented Programming

📌 **Concepto nuevo: ROP**

**Return-Oriented Programming** es una técnica que, en lugar de ejecutar código nuevo (shellcode), encadena pequeñas secuencias de instrucciones que **ya existen** en el binario.

Estas pequeñas secuencias se llaman **gadgets**.

### ¿Qué es un Gadget?

Un **gadget** es una secuencia de instrucciones que termina en `ret`.

```assembly
; Gadget 1: "pop rdi; ret"
pop rdi    ; toma valor del stack y lo pone en rdi
ret        ; toma el siguiente valor del stack y salta ahí

; Gadget 2: "pop rsi; ret"
pop rsi
ret

; Gadget 3: "syscall; ret"
syscall
ret
```

Estos gadgets ya existen en el binario o en librerías. No los escribes, los **encuentras**.

---

## Parte 3: Cómo Funciona una ROP Chain

La idea es construir un stack que, cuando sea "consumido" por múltiples `ret`, ejecute lo que quieres.

### Ejemplo: Llamar a exit(42)

Para llamar `exit(42)` necesitamos:
- `rdi = 42` (primer argumento)
- Llamar a `exit`

**ROP chain en el stack:**

```
Stack (lo que escribimos con el overflow):

    ┌──────────────────────────────┐
    │  dirección de "pop rdi; ret" │  ← return address (primer salto)
    ├──────────────────────────────┤
    │  42                          │  ← valor que "pop rdi" tomará
    ├──────────────────────────────┤
    │  dirección de exit()         │  ← segundo salto (desde el ret de gadget)
    └──────────────────────────────┘
```

**Ejecución:**

```
1. Función retorna → salta a "pop rdi; ret"
2. "pop rdi" toma 42 del stack → rdi = 42
3. "ret" toma siguiente dirección del stack → salta a exit()
4. exit() ejecuta con rdi=42 → exit(42)
```

---

## Parte 4: Encontrar Gadgets

### Con objdump

```bash
objdump -d -M intel binario | grep -B2 "ret"
```

Busca secuencias de instrucciones que terminen en `ret`.

### Con herramientas especializadas

```bash
# ROPgadget (instalar con pip3)
ROPgadget --binary binario --string "pop rdi"

# Salida:
# 0x0000000000400500 : pop rdi ; ret
```

### ¿Qué Gadgets Buscar?

Los más útiles para construir ataques:

```
pop rdi; ret    → cargar primer argumento
pop rsi; ret    → cargar segundo argumento
pop rdx; ret    → cargar tercer argumento
pop rax; ret    → cargar número de syscall
syscall; ret    → ejecutar syscall
```

---

## Parte 5: ASLR y ROP

Con ASLR activado, las direcciones cambian en cada ejecución. Esto complica ROP porque no sabes las direcciones de los gadgets.

**Solución:** Primero necesitas un **information leak** (filtración de información) para descubrir las direcciones reales en esa ejecución. Format strings son muy útiles para esto.

⚠️ Nota: ASLR bypass con leaks es un tema del siguiente proyecto en la serie. Aquí solo necesitas entender el concepto.

---

## Parte 6: Conexión con Todo lo Aprendido

```
Lección 01: Vulnerabilidades existen
    ↓
Lección 02: Binarios ELF tienen secciones (.text, .data, etc.)
    ↓
Lección 03: C tiene punteros, arrays, funciones vulnerables
    ↓
Lección 04: GDB permite ver qué pasa en tiempo de ejecución
    ↓
Lección 05: Assembly = instrucciones reales del CPU, registros
    ↓
Lección 06: Buffer overflow → controlar return address → controlar flujo
    ↓
Lección 07: Format strings → leer/escribir memoria arbitraria
    ↓
Lección 08: ROP → explotar con protecciones activadas
```

Cada lección construye sobre la anterior. Con todo esto, tienes la base completa de binary exploitation.

---

## Parte 7: Protecciones Resumidas

| Protección | Qué hace | Cómo desactivar (laboratorio) | Bypass real |
|------------|----------|-------------------------------|-------------|
| Stack Canary | Detecta overflow | `-fno-stack-protector` | Leak del canario |
| NX Bit | Stack no ejecutable | `-z execstack` | ROP chains |
| ASLR | Direcciones aleatorias | `-no-pie` | Information leak |
| RELRO | GOT read-only | `-Wl,-z,norelro` | Difícil |
| PIE | Código en dirección aleatoria | `-no-pie` | Information leak |

En el mundo real, todas están activadas simultáneamente. Por eso los exploits modernos son complejos.

---

## Checkpoints

✅ Completaste cuando:

- [ ] Entiendes por qué NX impide ejecutar shellcode en el stack
- [ ] Entiendes qué es un gadget (secuencia de instrucciones + ret)
- [ ] Entiendes cómo una ROP chain usa el stack para encadenar gadgets
- [ ] Sabes cómo buscar gadgets con objdump
- [ ] Entiendes por qué ASLR complica ROP
- [ ] Puedes relacionar todas las lecciones del curso

---

## Práctica Recomendada

**Ejercicio 06 del Nivel 01** (ROP Gadgets Basics).

---

## Lecturas Relacionadas

- `recursos/hints-futuros.md` - ROP avanzado en proyectos siguientes
- `recursos/referencias.md` - Papers sobre ROP

## Fin del Curso Base

Has completado las 8 lecciones teóricas. Ahora tienes las bases para:
- Analizar binarios
- Encontrar vulnerabilidades
- Entender y crear exploits básicos
- Prepararte para técnicas avanzadas

**Siguiente:** Nivel Bonus con desafíos que combinan todas las técnicas.

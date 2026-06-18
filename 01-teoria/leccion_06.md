# Lección 06: Buffer Overflow

## Objetivos de Aprendizaje

- Entender exactamente qué ocurre en memoria durante un buffer overflow
- Entender por qué sobrescribir el return address da control del programa
- Conocer las protecciones modernas y cómo afectan los exploits
- Prepararse para el Ejercicio 04 (crear un exploit real)

## Tiempo Estimado

60-90 minutos de lectura

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: Recordatorio - Stack Frame

En Lección 05 vimos que cuando se llama una función, el stack se ve así:

```
Stack:

    ┌──────────────────┐
    │  return address  │  ← dirección a donde regresar
    ├──────────────────┤
    │  saved rbp       │  ← rbp de la función anterior
    ├──────────────────┤  ← rbp apunta aquí
    │  variable local 1│
    │  variable local 2│
    │  buffer[N]       │
    └──────────────────┘  ← rsp apunta aquí
```

El **return address** es crítico: cuando la función termina y ejecuta `ret`, el CPU toma ese valor y salta ahí.

---

## Parte 2: ¿Qué Es Exactamente un Buffer Overflow?

Un **buffer overflow** ocurre cuando escribes **más datos** en un buffer de lo que ese buffer puede contener.

El exceso de datos sobrescribe lo que está **después** del buffer en memoria.

### Ejemplo Visual

```c
void funcion(char *entrada) {
    char buffer[10];       // 10 bytes
    strcpy(buffer, entrada); // copia sin verificar tamaño
}
```

**Caso normal** (entrada = "HOLA", 4 bytes):

```
Stack:
    ┌──────────────────────────────┐
    │  return address (0x400500)   │
    ├──────────────────────────────┤
    │  saved rbp                   │
    ├──────────────────────────────┤
    │  H  O  L  A  \0  ?  ?  ?  ? │  ← buffer[10]
    └──────────────────────────────┘
```

**Caso overflow** (entrada = "AAAAAAAAAAAAAAAAAAAAAAAAAAAA", 28 bytes):

```
Stack:
    ┌──────────────────────────────┐
    │  A  A  A  A  A  A  A  A      │  ← return address SOBRESCRITO
    ├──────────────────────────────┤
    │  A  A  A  A  A  A  A  A      │  ← saved rbp SOBRESCRITO
    ├──────────────────────────────┤
    │  A  A  A  A  A  A  A  A  A  A│  ← buffer[10] lleno
    └──────────────────────────────┘
```

El return address ahora contiene `0x4141414141414141` (AAAAAAAA en hex).

Cuando la función ejecuta `ret`, el CPU intenta saltar a `0x4141414141414141`. Esa dirección no existe → **crash**.

---

## Parte 3: Explotar el Overflow

Si en lugar de A's puedes escribir **una dirección real**, el CPU saltará ahí.

### El Plan de Ataque

1. Overflow el buffer hasta llegar al return address
2. Sobrescribir el return address con la dirección que quieres ejecutar
3. La función retorna → CPU salta a tu dirección

### Encontrar el Offset

Necesitas saber **cuántos bytes** hay entre el inicio del buffer y el return address.

```
Stack:
    ┌──────────────────────────────┐ ← return address (aquí quiero llegar)
    ├──────────────────────────────┤
    │  saved rbp (8 bytes)         │
    ├──────────────────────────────┤
    │  buffer[10] (10 bytes)       │
    └──────────────────────────────┘

Offset = 10 (buffer) + 8 (saved rbp) = 18 bytes
```

Pero en la práctica, el compilador puede agregar **padding** (bytes de relleno para alineación). Por eso siempre se verifica experimentalmente con GDB.

### Construir el Payload

```python
payload = b"A" * offset        # llenar hasta return address
payload += dirección_objetivo  # sobrescribir return address
```

Cuando la función retorne, ejecutará código en `dirección_objetivo`.

---

## Parte 4: Little Endian

📌 **Concepto nuevo: Little Endian**

Las computadoras x86-64 almacenan números en **little endian**: el byte menos significativo primero.

**Ejemplo:**

La dirección `0x400500` en memoria se almacena así:

```
Dirección en hex: 0x0000000000400500

En memoria (little endian):
[00] [05] [40] [00] [00] [00] [00] [00]
 ↑ primero                       último ↑
```

**¿Por qué importa?**

Cuando construyes un payload en Python y quieres poner una dirección, debes escribirla en little endian:

```python
from struct import pack

dirección = 0x400500
en_little_endian = pack("<Q", dirección)
# Resultado: b'\x00\x05\x40\x00\x00\x00\x00\x00'
```

`<Q` significa: little endian (`<`), unsigned long long de 8 bytes (`Q`).

---

## Parte 5: Protecciones Modernas

Los sistemas modernos tienen protecciones contra buffer overflow. En el Ejercicio 04 las desactivamos para aprender. Aquí las explicamos:

### 1. Stack Canary

📌 **Concepto nuevo: Stack Canary**

El compilador inserta un **valor mágico** (el "canario") entre las variables locales y el return address:

```
Stack:
    ┌──────────────────────────────┐
    │  return address              │
    ├──────────────────────────────┤
    │  saved rbp                   │
    ├──────────────────────────────┤
    │  CANARY (valor mágico)       │  ← nuevo
    ├──────────────────────────────┤
    │  buffer[10]                  │
    └──────────────────────────────┘
```

Antes de ejecutar `ret`, la función verifica que el canario no cambió. Si cambió → alguien hizo overflow → el programa termina inmediatamente.

**Cómo se activa:**
```bash
gcc -fstack-protector programa.c   # activado
gcc -fno-stack-protector programa.c  # desactivado
```

### 2. ASLR (Address Space Layout Randomization)

📌 **Concepto nuevo: ASLR**

Cada vez que el programa se ejecuta, las direcciones de memoria son **aleatorias**.

```
Ejecución 1: buffer está en 0x7fff1234
Ejecución 2: buffer está en 0x7fff9abc
Ejecución 3: buffer está en 0x7fff5678
```

Si no sabes la dirección exacta, no puedes escribirla en el payload.

**Estado en sistema:**
```bash
cat /proc/sys/kernel/randomize_va_space
# 0 = desactivado
# 2 = activado (default en Fedora)
```

### 3. NX Bit (No-Execute)

📌 **Concepto nuevo: NX Bit**

El stack está marcado como **no ejecutable**. Puedes escribir datos ahí, pero el CPU rechaza ejecutar código desde ahí.

⚠️ Nota: Cuando tengamos NX activado y ASLR activado, necesitaremos **ROP chains** (Lección 08) para explotar de todas formas.

---

## Parte 6: Por Qué Funciona el Exploit del Ejercicio 04

En Ejercicio 04 compilamos con:

```bash
gcc -fno-stack-protector   # Sin canario
    -z execstack           # Stack ejecutable (sin NX)
    -no-pie                # Sin ASLR
```

Con esas condiciones:
- ✅ No hay canario → podemos sobrescribir return address libremente
- ✅ Direcciones son fijas → sabemos exactamente dónde está todo
- ✅ Stack ejecutable → podríamos poner shellcode (no lo hacemos en este ejercicio)

El exploit que construyes en Ejercicio 04:
1. Rellena buffer + saved rbp con A's
2. Sobrescribe return address con dirección de `funcion_privilegiada`
3. La función retorna → CPU salta a `funcion_privilegiada`
4. Se imprime "¡¡¡ HAS GANADO !!!"

---

## Parte 7: Resumen Visual Completo

```
ANTES del exploit:

Stack de procesar_entrada():
    ┌──────────────────────────────┐
    │  return address → main+XX    │  ← CPU usará esto al retornar
    ├──────────────────────────────┤
    │  saved rbp                   │
    ├──────────────────────────────┤
    │  buffer[16]: vacío           │
    └──────────────────────────────┘

DURANTE el exploit (strcpy con payload largo):

    ┌──────────────────────────────┐
    │  0x400556 (funcion_priv)     │  ← SOBRESCRITO con nuestra dirección
    ├──────────────────────────────┤
    │  AAAAAAAA                    │  ← saved rbp sobrescrito (no importa)
    ├──────────────────────────────┤
    │  AAAAAAAAAAAAAAAA            │  ← buffer lleno con A's
    └──────────────────────────────┘

AL RETORNAR (ret):

    CPU lee return address → 0x400556
    CPU salta a 0x400556
    CPU ejecuta funcion_privilegiada()
    Imprime "¡¡¡ HAS GANADO !!!"
```

---

## Checkpoints

✅ Completaste cuando:

- [ ] Entiendes qué es un buffer overflow en términos de memoria
- [ ] Sabes qué es el return address y dónde está en el stack
- [ ] Entiendes por qué sobrescribir el return address da control
- [ ] Sabes qué es little endian y por qué importa para payloads
- [ ] Conoces las 3 protecciones principales (canary, ASLR, NX)
- [ ] Entiendes por qué en Ejercicio 04 desactivamos esas protecciones

---

## Práctica Recomendada

Ahora sí: **Ejercicio 04 del Nivel 01** (Buffer Overflow - Crear Exploit Funcional).

Con esta teoría, cada paso del ejercicio tendrá sentido completo.

---

## Lecturas Relacionadas

- `recursos/gdb-guia.md` - Debugging de exploits
- `recursos/glosario.md` - Buffer overflow, ASLR, NX bit, canary

## Próxima Lección

**Lección 07: Format Strings** - Otra clase de vulnerabilidad que permite leer y escribir en memoria arbitraria.

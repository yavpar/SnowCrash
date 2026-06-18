# Lección 07: Format Strings

## Objetivos de Aprendizaje

- Entender cómo funcionan los especificadores de formato en printf
- Entender por qué pasar input del usuario directamente a printf es peligroso
- Aprender a leer memoria con %x y %s
- Aprender a escribir en memoria con %n

## Tiempo Estimado

45-60 minutos de lectura

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: Recordatorio - printf Normal

Ya conoces printf de C:

```c
printf("Hola %s, tienes %d años\n", nombre, edad);
```

Aquí:
- `"Hola %s, tienes %d años\n"` → **format string** (string de formato)
- `%s` → especificador: "imprime el argumento como string"
- `%d` → especificador: "imprime el argumento como número decimal"
- `nombre, edad` → los argumentos que corresponden a cada especificador

Esta es la forma **correcta** de usar printf.

---

## Parte 2: El Problema - Format String Controlada por Usuario

Mira esta diferencia:

```c
// SEGURO: format string es literal, datos son argumentos
printf("%s\n", usuario);

// VULNERABLE: usuario ES el format string
printf(usuario);
```

¿Por qué es diferente?

En el segundo caso, si el usuario entra `"Hola %x"`, printf interpreta `%x` como especificador y **busca un argumento que no existe**.

¿Qué hace cuando no hay argumento? **Lee del stack directamente.**

---

## Parte 3: ¿Cómo printf Lee Argumentos?

📌 **Concepto: Cómo printf encuentra sus argumentos**

Cuando printf recibe múltiples argumentos, los lee de registros y luego del stack en orden:

```
printf("A=%x B=%x C=%x D=%x\n", a, b, c, d);

Internamente, printf busca:
- 1er %x → rsi (2do argumento)
- 2do %x → rdx (3er argumento)
- 3er %x → rcx (4to argumento)
- 4to %x → r8  (5to argumento)
- 5to %x → r9  (6to argumento)
- 6to %x → stack (el resto)
- 7mo %x → más stack
...
```

**¿Qué pasa si pones más especificadores que argumentos?**

printf no sabe. Sigue leyendo del stack como si hubiera argumentos. Lee valores que no son argumentos reales.

---

## Parte 4: Leer Memoria con %x

**Código vulnerable:**

```c
char entrada[100];
scanf("%s", entrada);
printf(entrada);  // vulnerable
```

**Si el usuario entra:** `"AAA %x %x %x %x"`

**printf ejecuta:**

```
Imprime: "AAA "
Luego: %x → lee del stack → imprime como hex
Luego: %x → lee más stack → imprime como hex
...
```

**Resultado:**

```
AAA 7ffffffde4c0 0 12345678 deadbeef
```

El usuario **leyó valores del stack** que no debería ver. Esto puede incluir:
- Direcciones de memoria (útiles para bypassear ASLR)
- Valores de variables privadas
- El canario (útil para bypassear stack canary)

---

## Parte 5: Leer un String con %s

`%x` imprime un número. `%s` imprime un **string desde una dirección**.

Si el stack contiene una dirección, `%s` irá a esa dirección y leerá el string que hay ahí.

**Ejemplo:**

```
Stack contiene: 0x400500 (dirección de un string en .rodata)

printf con %s → va a 0x400500 → lee string ahí → lo imprime
```

Esto permite leer contenido de cualquier dirección que esté en el stack.

---

## Parte 6: Escribir en Memoria con %n

📌 **Concepto nuevo: %n**

`%n` es un especificador especial: **escribe** en memoria en lugar de leer.

```c
int contador = 0;
printf("Hola%n", &contador);
// contador ahora = 4 (caracteres impresos antes de %n)
```

`%n` toma el próximo argumento como **dirección** y escribe ahí la cantidad de caracteres impresos hasta ese momento.

**En un programa vulnerable:**

Si controlas el format string, puedes:
1. Poner una dirección en el stack (en tu input)
2. Usar `%n` para escribir en esa dirección

Esto permite **escribir en memoria arbitraria**.

---

## Parte 7: Especificadores Directos con $

En lugar de leer argumentos en orden, puedes especificar cuál leer:

```
%3$x  → leer el 3er argumento (directo)
%5$x  → leer el 5to argumento (directo)
```

Esto es útil para:
- Llegar a un argumento específico sin imprimir todos los anteriores
- Encontrar exactamente dónde está tu input en el stack

---

## Parte 8: Detectar la Vulnerabilidad

Señales de alerta en código:

```c
// VULNERABLE
printf(entrada);
fprintf(file, entrada);
sprintf(buffer, entrada);

// SEGURO
printf("%s", entrada);
fprintf(file, "%s", entrada);
snprintf(buffer, sizeof(buffer), "%s", entrada);
```

La regla es simple: **el primer argumento de printf siempre debe ser un literal**, nunca una variable.

---

## Parte 9: Diferencia con Buffer Overflow

| Aspecto | Buffer Overflow | Format String |
|---------|----------------|---------------|
| Vulnerabilidad | strcpy sin límite | printf con input directo |
| Leer memoria | No directamente | Sí, con %x %s |
| Escribir en memoria | Sí (sobrescribiendo) | Sí, con %n |
| Control flujo | Sobrescribir return address | Sobrescribir con %n |
| Dificultad | Media | Alta |

---

## Checkpoints

✅ Completaste cuando:

- [ ] Entiendes la diferencia entre `printf("%s", x)` y `printf(x)`
- [ ] Sabes por qué la segunda forma es vulnerable
- [ ] Entiendes cómo %x lee del stack
- [ ] Entiendes qué hace %s con una dirección en el stack
- [ ] Sabes qué hace %n y por qué es peligroso
- [ ] Puedes identificar una format string vulnerability en código

---

## Práctica Recomendada

**Ejercicio 05 del Nivel 01** (Format String Vulnerabilities).

---

## Lecturas Relacionadas

- `recursos/glosario.md` - Format strings
- `recursos/referencias.md` - Artículos clásicos sobre format strings

## Próxima Lección

**Lección 08: ROP y Protecciones** - Qué hacer cuando las protecciones están activadas.

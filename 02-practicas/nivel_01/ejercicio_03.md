# Ejercicio 03: Detectar Vulnerabilidades mediante Análisis Estático

## Objetivos de Aprendizaje

- Identificar patrones peligrosos en código C
- Usar herramientas de análisis estático
- Entender por qué ciertas funciones son peligrosas
- Proponer fixes para código vulnerable

## Tiempo Estimado

45-60 minutos

## Dificultad

⭐⭐ Intermedio

---

## Parte 1: Identificar Funciones Peligrosas

Hay funciones de C que **NUNCA son seguras** porque no tienen límite de tamaño:

| Función | Problema | Alternativa Segura |
|---------|----------|-------------------|
| `strcpy()` | No verifica tamaño | `strncpy()` o `strlcpy()` |
| `strcat()` | No verifica tamaño | `strncat()` o `strlcat()` |
| `sprintf()` | No verifica tamaño | `snprintf()` |
| `gets()` | Obsoleta, nunca segura | `fgets()` |
| `scanf("%s")` | No verifica tamaño | `scanf("%20s")` con límite |

### Código Vulnerable Deliberado

```bash
cat > analizar.c << 'EOF'
#include <stdio.h>
#include <string.h>

void funcion1(char *usuario) {
    char nombre[10];
    strcpy(nombre, usuario);  // VULNERABLE - strcpy sin límite
    printf("Hola %s\n", nombre);
}

void funcion2(char *entrada) {
    char buffer[20];
    sprintf(buffer, "Datos: %s", entrada);  // VULNERABLE - sprintf sin límite
    printf("%s\n", buffer);
}

void funcion3(char *texto) {
    char cadena[5];
    gets(cadena);  // VULNERABLE - gets NUNCA es seguro
    printf("%s\n", cadena);
}

void funcion4(char *str) {
    char resultado[15];
    strcpy(resultado, "Prefijo:");  // Primera copia
    strcat(resultado, str);         // VULNERABLE - strcat sin límite
    printf("%s\n", resultado);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        funcion1(argv[1]);
    }
    return 0;
}
EOF
```

---

## Parte 2: Análisis Manual - Buscar Patrones

### Técnica 1: Grep para funciones peligrosas

```bash
# Buscar strcpy
grep -n "strcpy" analizar.c
# Output:
# 6:    strcpy(nombre, usuario);

# Buscar sprintf
grep -n "sprintf" analizar.c
# Output:
# 11:    sprintf(buffer, "Datos: %s", entrada);

# Buscar gets
grep -n "gets" analizar.c
# Output:
# 16:    gets(cadena);

# Buscar strcat
grep -n "strcat" analizar.c
# Output:
# 21:    strcat(resultado, str);
```

### Técnica 2: Buscar todo en una línea

```bash
grep -n "strcpy\|strcat\|sprintf\|gets" analizar.c
# Salida rápida de todas las funciones peligrosas
```

### Técnica 3: Análisis contextual

Para cada `strcpy`, preguntar:
1. ¿Cuál es el tamaño del buffer destino?
2. ¿Cuál es la fuente?
3. ¿Quién controla la fuente?

Ejemplo línea 6:
```c
char nombre[10];        // Tamaño: 10 bytes
strcpy(nombre, usuario); // Fuente: parámetro usuario
                         // Controlada por: el que llamó la función
                         // RIESGO: Alto - usuario puede ser más largo
```

---

## Parte 3: Usar Herramientas de Análisis

### Opción 1: Compilador warnings

```bash
gcc -Wall -Wextra -O2 analizar.c

# Output típico:
analizar.c:6:5: warning: implicit declaration of function 'printf'
analizar.c:3:22: warning: unused parameter 'usuario'
analizar.c:16:5: warning: 'gets' is deprecated
```

Pero GCC **NO detecta** la mayoría de buffer overflows.

### Opción 2: Herramientas especializadas

**clang Static Analyzer** (si instalado):

```bash
scan-build gcc -c analizar.c
# Detecta muchos problemas
```

**Cppcheck** (alternativa):

```bash
cppcheck analizar.c
# Salida:
# [analizar.c:6]: (error) Buffer is not null terminated.
# [analizar.c:16]: (error) gets() is always unsafe.
```

### Opción 3: Análisis Manual Profundo

Crear tabla de vulnerabilidades:

```
┌────────────┬──────────────┬───────────────────┬─────────┐
│ Línea      │ Función      │ Tipo              │ Riesgo  │
├────────────┼──────────────┼───────────────────┼─────────┤
│ 6          │ strcpy       │ Buffer overflow   │ ALTO    │
│ 11         │ sprintf      │ Buffer overflow   │ ALTO    │
│ 16         │ gets         │ Buffer overflow   │ CRÍTICO │
│ 21         │ strcat       │ Buffer overflow   │ ALTO    │
└────────────┴──────────────┴───────────────────┴─────────┘
```

---

## Parte 4: Entender Por Qué Es Peligroso

### Caso 1: strcpy - Buffer Overflow Simple

```c
char nombre[10];
strcpy(nombre, usuario);
```

¿Qué pasa si usuario = "AbCdEfGhIjKlMnOp" (16 bytes)?

```
Buffer:  [A][b][C][d][E][f][G][h][I][j]  <- 10 bytes, LLENO
Entrada:  A  b  C  d  E  f  G  h  I  j  K  l  M  n  O  p
                                        ↓ Estos 6 bytes sobrescriben
```

Los últimos 6 bytes van a memoria que no les pertenece.

### Caso 2: sprintf - Formato String + Overflow

```c
char buffer[20];
sprintf(buffer, "Datos: %s", entrada);
```

Problema doble:
1. Si entrada es larga, overflow
2. Si entrada contiene `%x` u otros formatos, **information leak**

### Caso 3: gets() - Obsoleto

```c
char cadena[5];
gets(cadena);  // Lee HASTA ENTER, sin límite
```

Usuario entra: `AAAAAAAAAAAAAAAA` (16 A's)

```
Buffer:  [A][A][A][A][A]
Input:    A  A  A  A  A  A  A  A  A  A  A  A  A  A  A  A
                      ↓ Todos sobrescriben
```

**gets() fue removido del estándar C11** porque es tan peligroso.

### Caso 4: strcat() - Concatenación sin límite

```c
char resultado[15];
strcpy(resultado, "Prefijo:");     // resultado = "Prefijo:" (8 bytes)
strcat(resultado, str);            // Añade str
```

Si `str = "AbCdEfGhIj"` (10 bytes):

```
resultado (después de strcpy):  "Prefijo:\0"   [8 bytes + null]
                  Espacio libre:           7 bytes

strcat intenta copiar 10 bytes en 7 bytes disponibles
Overflow de 3 bytes
```

---

## Parte 5: Código Corregido

### Versión Segura

```bash
cat > seguro.c << 'EOF'
#include <stdio.h>
#include <string.h>

#define NOMBRE_MAX 10
#define BUFFER_MAX 20
#define RESULTADO_MAX 15

void funcion1(char *usuario) {
    char nombre[NOMBRE_MAX];
    // SEGURO: strncpy con límite explícito
    strncpy(nombre, usuario, NOMBRE_MAX - 1);
    nombre[NOMBRE_MAX - 1] = '\0';  // Garantizar terminador
    printf("Hola %s\n", nombre);
}

void funcion2(char *entrada) {
    char buffer[BUFFER_MAX];
    // SEGURO: snprintf con límite
    snprintf(buffer, sizeof(buffer), "Datos: %s", entrada);
    printf("%s\n", buffer);
}

void funcion3(void) {
    char cadena[5];
    // SEGURO: fgets con límite explícito
    if (fgets(cadena, sizeof(cadena), stdin) != NULL) {
        printf("%s\n", cadena);
    }
}

void funcion4(char *str) {
    char resultado[RESULTADO_MAX];
    // SEGURO: strlcpy + strlcat (si disponible)
    // O: snprintf
    snprintf(resultado, sizeof(resultado), "Prefijo:%s", str);
    printf("%s\n", resultado);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        funcion1(argv[1]);
    }
    return 0;
}
EOF
```

### Compilar y comparar

```bash
gcc -Wall -Wextra -O2 -o seguro seguro.c
# Debería compilar SIN warnings
```

---

## Parte 6: Análisis Práctico - Sistema de Puntos

Para cada función vulnerable, asignar riesgo:

```
Escala de Riesgo:
CRÍTICO (10): Usuario puede ejecutar código arbitrario
ALTO (7-9):   Usuario puede sobrescribir datos importantes  
MEDIO (4-6):  Posible crash o leak de información
BAJO (1-3):   Comportamiento inesperado
```

### Analizar nuestro código:

```
Línea 6 - strcpy(nombre, usuario):
  - Tamaño buffer: 10
  - Fuente: argv[1] (controlada por usuario)
  - Riesgo: ALTO (puede sobrescribir stack)
  
Línea 11 - sprintf(..., entrada):
  - Tamaño buffer: 20
  - Fuente: parámetro entrada
  - Riesgo: ALTO (overflow + posible format string)
  
Línea 16 - gets():
  - Tamaño buffer: 5
  - Fuente: stdin (controlada por usuario)
  - Riesgo: CRÍTICO (gets nunca es seguro)
  
Línea 21 - strcat(resultado, str):
  - Tamaño buffer: 15
  - Ya tiene "Prefijo:" (8 bytes)
  - Espacio libre: 6 bytes
  - Fuente: parámetro str
  - Riesgo: ALTO (probablemente overflow)
```

---

## Parte 7: Compilar y Testear

### Testear versión vulnerable

```bash
gcc -g -O0 -fno-stack-protector -o analizar analizar.c

# Prueba 1: Input normal
./analizar "Juan"
# Output esperado:
# Hola Juan

# Prueba 2: Input largo
./analizar "AbCdEfGhIjKlMnOpQrStUvWxYz"
# Posible crash o comportamiento raro
```

### Testear versión segura

```bash
gcc -g -O0 -o seguro seguro.c

# Prueba 1: Input normal
./seguro "Juan"
# Output esperado:
# Hola Juan

# Prueba 2: Input largo
./seguro "AbCdEfGhIjKlMnOpQrStUvWxYz"
# Output esperado:
# Hola AbCdEfGhIj  (cortado a 9 caracteres + null)
# NO crash
```

---

## Checkpoints

✅ Completaste cuando:

- [ ] Identificaste todas las funciones peligrosas en analizar.c
- [ ] Creaste tabla de vulnerabilidades con línea, función, riesgo
- [ ] Compilaste con warnings (`gcc -Wall -Wextra`)
- [ ] Entendiste por qué cada función es peligrosa
- [ ] Creaste versión segura (seguro.c)
- [ ] Compilaste versión segura sin warnings
- [ ] Testeaste ambas versiones con input largo
- [ ] Documentaste diferencias entre versiones

---

## Desafío Extra

### Nivel 1: Encontrar más vulnerabilidades

```bash
# Script de análisis
grep -E "strcpy|strcat|sprintf|gets|scanf" *.c

# Crear lista de todos los usos inseguros en tu código
```

### Nivel 2: Automatizar búsqueda

```bash
# Script bash para analizar archivo
#!/bin/bash
echo "=== ANÁLISIS DE VULNERABILIDADES ==="
echo ""

archivo=$1

echo "1. Funciones peligrosas encontradas:"
grep -n "strcpy\|strcat\|sprintf\|gets" "$archivo"

echo ""
echo "2. Warnings del compilador:"
gcc -Wall -Wextra -c "$archivo" 2>&1 | grep warning

echo ""
echo "3. Lineas con arrays:"
grep -n "char.*\[" "$archivo"
```

Guardar como `analizar.sh`:
```bash
chmod +x analizar.sh
./analizar.sh analizar.c
```

### Nivel 3: Verificar Fixes

```bash
# Para cada vulnerabilidad, proponer fix:

# Original:  strcpy(dest, src)
# Fix:       strncpy(dest, src, sizeof(dest)-1); dest[sizeof(dest)-1]='\0';

# Original:  sprintf(buf, fmt, var)
# Fix:       snprintf(buf, sizeof(buf), fmt, var)

# Original:  gets(buf)
# Fix:       fgets(buf, sizeof(buf), stdin)

# Original:  strcat(dest, src)
# Fix:       strncat(dest, src, sizeof(dest) - strlen(dest) - 1)
```

---

## Conceptos Clave Aprendidos

### Funciones Peligrosas
- strcpy, strcat, sprintf, gets: Sin límite
- Alternativas: strncpy, strncat, snprintf, fgets
- El límite DEBE ser sizeof() - 1

### Análisis Estático
- Herramientas automáticas: cppcheck, scan-build
- Análisis manual: grep, lógica
- Entender el contexto: tamaño + fuente

### Triage de Vulnerabilidades
- Clasificar por riesgo
- Priorizar fixes
- Documentar cambios

---

## Lecturas Relacionadas

- `recursos/glosario.md` - Buffer overflow, format strings
- `01-teoria/leccion_01.md` - Vulnerabilidades generales
- `referencias.md` - OWASP resources

## Siguiente

**Ejercicio 04:** Buffer Overflow - Crear exploit funcional

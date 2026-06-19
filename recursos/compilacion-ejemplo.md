# Compilación: Ejemplo Práctico Paso a Paso

## El Programa

Usaremos un programa pequeño pero con todos los elementos interesantes:
- Variable global inicializada (→ `.data`)
- Variable global sin inicializar (→ `.bss`)
- String literal (→ `.rodata`)
- Variables locales (→ stack)
- Función propia + llamada a printf

```bash
mkdir -p ~/SnowCrash/recursos/ejemplo-compilacion
cd ~/SnowCrash/recursos/ejemplo-compilacion

cat > demo.c << 'EOF'
#include <stdio.h>

int contador = 42;          // variable global inicializada → .data
int resultado;              // variable global sin inicializar → .bss

int sumar(int a, int b) {
    return a + b;
}

int main() {
    int x = 10;             // variable local → stack
    int y = 20;             // variable local → stack
    char *mensaje = "Hola desde ft_snowcrash\n";  // string → .rodata

    resultado = sumar(x, y);
    contador = resultado;

    printf(mensaje);
    printf("Resultado: %d\n", resultado);
    printf("Contador:  %d\n", contador);

    return 0;
}
EOF
```

---

## Etapa 1: Preprocesador

```bash
gcc -E demo.c -o demo.i
```

### Ver el antes y el después

```bash
wc -l demo.c
# → 20 líneas

wc -l demo.i
# → ~800+ líneas

# Ver las últimas líneas del .i (tu código original, expandido)
tail -30 demo.i
```

El final del `.i` es tu código. Todo lo anterior es el contenido de `stdio.h` y sus dependencias.

### Buscar dónde quedó tu código

```bash
grep -n "contador" demo.i
grep -n "sumar" demo.i
```

Verás que tu código está intacto, pero los `#include` desaparecieron y fueron reemplazados por las declaraciones que contenían.

### Ver que los comentarios desaparecieron

```bash
grep -n "variable global" demo.i
# → sin resultados: los comentarios fueron eliminados
```

---

## Etapa 2: Compilador → Assembly

```bash
gcc -S -O0 demo.c -o demo.s
cat demo.s
```

### Leer el assembly generado

Busca la función `sumar`:

```bash
grep -A 15 "sumar:" demo.s
```

Verás algo como:

```asm
sumar:
    pushq   %rbp
    movq    %rsp, %rbp
    movl    %edi, -4(%rbp)     # guardar argumento a en [rbp-4]
    movl    %esi, -8(%rbp)     # guardar argumento b en [rbp-8]
    movl    -4(%rbp), %edx     # edx = a
    movl    -8(%rbp), %eax     # eax = b
    addl    %edx, %eax         # eax = a + b
    popq    %rbp
    ret
```

### Buscar dónde quedó el string "Hola"

```bash
grep -n "Hola" demo.s
```

Verás algo como:

```asm
.LC0:
    .string "Hola desde ft_snowcrash\n"
```

El string está en una sección especial (`.rodata`). La función `main` referencia `.LC0` por nombre, no por contenido.

### Ver la variable global `contador`

```bash
grep -n "contador" demo.s
```

Verás:

```asm
contador:
    .long   42
```

Definida en `.data` con su valor inicial.

### Ver la variable global `resultado`

```bash
grep -n "resultado" demo.s
```

Verás que está en `.bss` (sin valor, el kernel la pondrá a 0).

---

## Etapa 3: Ensamblador → Objeto (.o)

```bash
gcc -c demo.c -o demo.o
```

### Ver los símbolos del .o

```bash
nm demo.o
```

Output esperado:

```
0000000000000000 B resultado      # B = .bss (sin inicializar)
0000000000000000 D contador       # D = .data (inicializada)
0000000000000000 T sumar          # T = .text (función)
0000000000000018 T main           # T = .text (función, offset 0x18)
                 U printf         # U = undefined (externo, sin resolver)
```

Los símbolos `sumar` y `main` tienen offset 0x0 y 0x18 dentro del `.text` del objeto. Aún no tienen direcciones absolutas.

`printf` está marcado como `U` (undefined): el ensamblador sabe que existe pero no sabe dónde está.

### Ver los huecos (relocations)

```bash
objdump -r demo.o
```

Output:

```
RELOCATION RECORDS FOR [.text]:
OFFSET           TYPE              VALUE
000000000000002a R_X86_64_PC32     contador-0x4
000000000000003e R_X86_64_PLT32    sumar-0x4
000000000000005a R_X86_64_PC32     resultado-0x4
000000000000006e R_X86_64_PLT32    printf-0x4
```

Estos son los huecos. En el offset `0x3e` del `.text` hay una llamada a `sumar` con dirección desconocida. El linker la llenará.

### Ver el código de máquina crudo

```bash
objdump -d demo.o
```

Busca la llamada a `sumar` o `printf`:

```asm
3e:  e8 00 00 00 00    callq  43 <main+0x43>
```

El `00 00 00 00` es el hueco: la dirección de `sumar` aún no está.

---

## Etapa 4: Linker → Ejecutable

```bash
gcc demo.o -o demo
```

### Ver que los huecos están rellenos

```bash
objdump -d demo | grep -A 2 "call"
```

Ahora verás direcciones reales:

```asm
40056e:  e8 b4 ff ff ff    callq  400528 <sumar>
```

`0xb4ffffff` es `-0x4c` en complemento a 2 = offset relativo a la siguiente instrucción. El linker calculó la distancia exacta entre la llamada y la función.

### Ver las secciones del ELF final

```bash
readelf -S demo
```

Busca las secciones importantes:

```
  [Nr] Name       Type     Address          Off    Size
  [13] .text      PROGBITS 0000000000400430 000430 000192  AX  → ejecutable
  [15] .rodata    PROGBITS 0000000000400610 000610 000038  A   → solo lectura
  [24] .data      PROGBITS 0000000000601010 001010 000010  WA  → lectura/escritura
  [25] .bss       NOBITS   0000000000601020 001020 000008  WA  → sin datos en disco
```

Fíjate en los flags:
- `.text`: `AX` = Allocated + eXecutable
- `.rodata`: `A` = solo Allocated (ni escribible ni marcada ejecutable)
- `.data`: `WA` = Writable + Allocated
- `.bss`: `WA` = Writable + Allocated (NOBITS = no ocupa espacio en disco)

### Ver dónde quedó cada cosa

**Encontrar el string "Hola":**

```bash
objdump -s -j .rodata demo
```

Output:

```
Contents of section .rodata:
 400610 01000200 486f6c61 20646573 64652066  ....Hola desde f
 400620 745f736e 6f776372 61736800 52657375  t_snowcrash.Resu
```

Hex `48 6f 6c 61` = "Hola". Está en la dirección `0x400610`.

**Encontrar `contador` con su valor 42:**

```bash
objdump -s -j .data demo
```

Output:

```
Contents of section .data:
 601010 00000000 00000000 2a000000 00000000  ........*.......
```

El valor `2a` en hex = 42 en decimal. Es `contador = 42`.

**Encontrar `resultado` en .bss:**

```bash
readelf -S demo | grep bss
```

`.bss` no tiene contenido en el archivo (NOBITS). Solo existe en memoria, donde el kernel lo inicializa a ceros.

---

## Ver Todo en Memoria con GDB

```bash
gcc -g -O0 -o demo demo.c
gdb ./demo
```

### Ver dónde están las variables en memoria

```bash
(gdb) break main
(gdb) run

# Dirección de cada símbolo
(gdb) print &contador
$1 = (int *) 0x601028

(gdb) print &resultado
$2 = (int *) 0x60102c

# Ver el valor inicial de contador
(gdb) print contador
$3 = 42

# Ver el valor inicial de resultado (debe ser 0)
(gdb) print resultado
$4 = 0
```

### Ver la memoria de .data directamente

```bash
(gdb) x/4xw 0x601028
0x601028: 0x0000002a  0x00000000  0x00000000  0x00000000
                ↑
              42 en hex = 0x2a
```

### Avanzar y ver cómo cambia la memoria

```bash
(gdb) next        # int x = 10
(gdb) next        # int y = 20
(gdb) next        # char *mensaje = ...
(gdb) next        # resultado = sumar(x, y)

(gdb) print resultado
$5 = 30           # 10 + 20

(gdb) print contador
$6 = 42           # aún no cambió

(gdb) next        # contador = resultado

(gdb) print contador
$7 = 30           # ahora sí cambió
```

### Ver el stack frame de main

```bash
(gdb) break main
(gdb) run

(gdb) info frame
Stack level 0, frame at 0x7fffffffe4d0:
 rip = 0x40056a in main (demo.c:12)
 rbp = 0x7fffffffe4c0
 rsp = 0x7fffffffe4b0

(gdb) info locals
x = 0              # aún no asignado
y = 0
mensaje = 0x0
```

```bash
(gdb) next         # int x = 10
(gdb) info locals
x = 10
```

### Ver en qué dirección está el string "Hola"

```bash
(gdb) print mensaje
$8 = 0x400610 "Hola desde ft_snowcrash\n"
```

La dirección `0x400610` está en `.rodata`. Compárala con la que viste en `objdump -s -j .rodata`.

### Verificar que .rodata no se puede escribir

```bash
(gdb) set *mensaje = 'X'
# Cannot access memory at address 0x400610
```

El kernel rechaza la escritura. `.rodata` está mapeada como solo lectura.

### Ver el stack completo en un momento

```bash
(gdb) break sumar
(gdb) continue

(gdb) backtrace
#0  sumar (a=10, b=20) at demo.c:6
#1  0x4005a3 in main () at demo.c:16

(gdb) info args
a = 10
b = 20

(gdb) x/8xw $rsp
# Verás: el return address, el saved rbp, y los argumentos
```

---

## Ver el Mapa de Memoria del Proceso

```bash
# Mientras el programa está en GDB pausado:
(gdb) break main
(gdb) run

(gdb) info proc mappings
```

Output:

```
Mapped address spaces:

      Start Addr         End Addr     Size   Offset  objfile
      0x400000          0x401000   0x1000        0  /home/yavier/demo  ← .text, .rodata
      0x600000          0x601000   0x1000        0  /home/yavier/demo  ← headers
      0x601000          0x602000   0x1000   0x1000  /home/yavier/demo  ← .data, .bss
 0x7ffff7a00000   0x7ffff7bcd000  0x1cd000      0  /lib64/libc.so.6   ← libc cargada
 0x7ffff7dd8000   0x7ffff7ddc000   0x4000 0x1d8000  /lib64/libc.so.6
 0x7ffff7ddc000   0x7ffff7dde000   0x2000 0x1dc000  /lib64/libc.so.6
 0x7ffff7dde000   0x7ffff7de2000   0x4000        0
 0x7ffff7de2000   0x7ffff7de3000   0x1000        0  /lib64/ld-linux-x86-64.so.2
 0x7ffffffde000   0x7ffffffff000  0x21000        0  [stack]
```

Puedes ver exactamente:
- Tu código en `0x400000`
- libc cargada en `0x7ffff7a00000`
- El stack en `0x7ffffffde000`

---

## Comparar Etapas: Resumen Visual

| Etapa | Archivo | Ver con | Qué muestra |
|-------|---------|---------|-------------|
| Original | `demo.c` | `cat` | código C |
| Preprocesado | `demo.i` | `tail -30` | C expandido, sin `#` |
| Assembly | `demo.s` | `cat` | instrucciones texto |
| Objeto | `demo.o` | `nm`, `objdump -d` | código binario, huecos |
| Ejecutable | `demo` | `readelf`, `objdump`, `gdb` | binario completo |

---

## Lecturas Relacionadas

- `recursos/compilacion-proceso.md` - La teoría detrás de cada etapa
- `recursos/ejecucion-kernel.md` - Qué hace el kernel al ejecutar el binario
- `recursos/gdb-guia.md` - Más comandos de GDB

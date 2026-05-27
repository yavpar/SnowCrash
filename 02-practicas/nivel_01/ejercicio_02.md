# Ejercicio 02: Disassembly Práctico con Objdump

## Objetivos de Aprendizaje

- Usar objdump para desensamblar código
- Leer assembly x86-64 real
- Relacionar C con assembly
- Entender convención de llamadas

## Tiempo Estimado

60-75 minutos

## Dificultad

⭐⭐ Intermedio

---

## Parte 1: Crear Programas para Analizar

### Programa Simple: Suma de Números

```bash
cat > suma.c << 'EOF'
#include <stdio.h>

int suma(int a, int b) {
    int resultado = a + b;
    return resultado;
}

int main() {
    int x = 5;
    int y = 3;
    int z = suma(x, y);
    printf("5 + 3 = %d\n", z);
    return 0;
}
EOF
```

### Compilar

```bash
# Con optimización mínima
gcc -g -O0 -o suma suma.c

# Con optimización media (para comparar)
gcc -g -O2 -o suma_opt suma.c
```

---

## Parte 2: Disassembly Básico

### Ver TODO el código

```bash
objdump -d suma | head -100
```

Salida esperada:

```
suma:
08048400 <suma>:
 8048400:       55                      push   %rbp
 8048401:       48 89 e5                mov    %rsp,%rbp
 8048404:       89 7d fc                mov    %edi,-0x4(%rbp)
 8048407:       89 75 f8                mov    %esi,-0x8(%rbp)
 804840a:       8b 45 fc                mov    -0x4(%rbp),%eax
 804840d:       03 45 f8                add    -0x8(%rbp),%eax
 8048410:       89 45 f4                mov    %eax,-0xc(%rbp)
 8048413:       8b 45 f4                mov    -0xc(%rbp),%eax
 8048416:       5d                      pop    %rbp
 8048417:       c3                      retq
```

### Usar Intel Syntax (más claro)

```bash
objdump -d -M intel suma | grep -A 30 "<suma>:"
```

Salida (Intel):

```
08048400 <suma>:
 8048400:       55                      push   rbp
 8048401:       48 89 e5                mov    rbp,rsp
 8048404:       89 7d fc                mov    DWORD PTR [rbp-0x4],edi
 8048407:       89 75 f8                mov    DWORD PTR [rbp-0x8],esi
 804840a:       8b 45 fc                mov    eax,[rbp-0x4]
 804840d:       03 45 f8                add    eax,[rbp-0x8]
 8048410:       89 45 f4                mov    DWORD PTR [rbp-0xc],eax
 8048413:       8b 45 f4                mov    eax,[rbp-0xc]
 8048416:       5d                      pop    rbp
 8048417:       c3                      retq
```

### Desglosando línea por línea

**Línea 1: `push rbp`**
```
Acción: Guardar el RBP anterior en stack
Razón: Preservar frame anterior para poder volver
```

**Línea 2: `mov rbp,rsp`**
```
Acción: RBP = RSP (base del nuevo frame)
Razón: Ahora RBP apunta al inicio de MI frame
```

**Línea 3-4: Guardar argumentos**
```
mov DWORD PTR [rbp-0x4],edi    ; a está en edi (1er arg)
mov DWORD PTR [rbp-0x8],esi    ; b está en esi (2do arg)

Acción: Copiar argumentos a memoria local
Por qué: Los argumentos llegan en registros (edi, esi)
        pero necesito poderlos usar durante la función
```

**Línea 5-6: Hacer la suma**
```
mov eax,[rbp-0x4]   ; eax = a
add eax,[rbp-0x8]   ; eax = eax + b

Acción: eax = a + b
```

**Línea 7: Guardar resultado**
```
mov DWORD PTR [rbp-0xc],eax   ; resultado = eax

Acción: Guardar suma en variable local "resultado"
```

**Línea 8-9: Retorno**
```
mov eax,[rbp-0xc]   ; eax = resultado (valor retorno)
pop rbp             ; restaurar RBP anterior
retq                ; saltar a dirección guardada
```

---

## Parte 3: Análisis de Main

### Desensamblar main

```bash
objdump -d -M intel suma | grep -A 50 "<main>:"
```

Salida esperada (puede variar):

```
08048418 <main>:
 8048418:       55                      push   rbp
 8048419:       48 89 e5                mov    rbp,rsp
 804841c:       48 83 ec 10             sub    rsp,0x10
 8048420:       c7 45 fc 05 00 00 00    mov    DWORD PTR [rbp-0x4],0x5
 8048427:       c7 45 f8 03 00 00 00    mov    DWORD PTR [rbp-0x8],0x3
 804842e:       8b 45 fc                mov    eax,[rbp-0x4]
 8048431:       8b 4d f8                mov    ecx,[rbp-0x8]
 8048434:       89 c7                   mov    edi,eax
 8048436:       89 ce                   mov    esi,ecx
 8048438:       e8 c3 ff ff ff          call   8048400 <suma>
 804843d:       89 45 f4                mov    DWORD PTR [rbp-0xc],eax
 8048440:       ...printf call...
```

### Seguimiento paso a paso

1. **Preparar variables locales** (x=5, y=3)
   ```
   mov DWORD PTR [rbp-0x4],0x5   ; x = 5
   mov DWORD PTR [rbp-0x8],0x3   ; y = 3
   ```

2. **Preparar argumentos para suma()**
   ```
   mov eax,[rbp-0x4]    ; eax = x (5)
   mov ecx,[rbp-0x8]    ; ecx = y (3)
   mov edi,eax          ; edi = 5  (1er arg)
   mov esi,ecx          ; esi = 3  (2do arg)
   ```

3. **Llamar a suma**
   ```
   call 8048400 <suma>
   ; Automáticamente: push rip (dirección de retorno)
   ```

4. **Después del retorno, z = resultado**
   ```
   mov DWORD PTR [rbp-0xc],eax  ; z = eax (que tiene retorno)
   ```

---

## Parte 4: Comparación Optimizaciones

### Compilar y desensamblar con -O2

```bash
gcc -g -O2 -o suma_opt suma.c
objdump -d -M intel suma_opt | grep -A 15 "<suma>:"
```

Resultado con -O2 (probablemente más corto):

```
0000000000400500 <suma>:
  400500:       01 c6                   add    esi,edi
  400502:       89 f0                   mov    eax,esi
  400504:       c3                      retq
```

### Comparación

| Aspecto | -O0 | -O2 |
|---------|-----|-----|
| Líneas | 10+ | 3 |
| Creación de frame | Sí | No (inline) |
| Uso de memoria | Sí | No |
| Velocidad | Normal | Rápido |
| Debuggeable | Sí | Difícil |

**Con -O2, el compilador:**
- Elimina la frame (usa registros directamente)
- Suma en un registro (esi)
- Retorna inmediatamente

---

## Parte 5: Análisis de Secciones

### Ver secciones del binario

```bash
objdump -h suma
```

Output:

```
Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 .interp       0000001c  0000000000400238  0000000000400238  00000238  2**0
  1 .note.ABI-tag 00000020  0000000000400254  0000000000400254  00000254  2**2
  ...
 13 .text         00000182  00000000004005b0  00000000004005b0  000005b0  2**4
 14 .fini         0000000e  0000000000400732  00000000400732  00000732  2**2
 15 .rodata       00000015  0000000000400748  0000000000400748  00000748  2**3
```

**`.text`** = Donde está nuestro código (ejecutable)
**`.rodata`** = Strings (read-only)
**`.data`** = Variables globales inicializadas

### Ver strings en .rodata

```bash
objdump -s -j .rodata suma

Contents of section .rodata:
 400748 01000200 48656c6c 6f2c2057 6f726c64  ....Hello, World
 400758 00                                   .
```

Hex `48 65 6c 6c 6f` = "Hello" en ASCII

---

## Parte 6: Preguntas de Investigación

### Pregunta 1: Convención de Llamadas

¿En qué registros llegan los argumentos a suma()?

```bash
# Respuesta esperada: edi, esi, edx, ecx, r8, r9
# a llega en edi (primer argumento)
# b llega en esi (segundo argumento)
```

### Pregunta 2: Offset de Variables

En la versión -O0, ¿a qué offset relativo a RBP están almacenadas?

```bash
# En suma():
# a está en [rbp-0x4]
# b está en [rbp-0x8]
# resultado en [rbp-0xc]

# Diferencia: 4 bytes entre cada variable (DWORD)
```

### Pregunta 3: Size de Frame

¿Cuántos bytes reserva main en stack?

```bash
# En main:
# sub rsp,0x10
# Respuesta: 0x10 = 16 bytes (para x, y, z + padding)
```

### Pregunta 4: Return Address

Después de `call 8048400`, ¿dónde está guardada la dirección de retorno?

```bash
# El CPU automáticamente hace: push rip
# Dirección de retorno está en [rsp] o [rbp+8]
```

---

## Parte 7: Vulnerabilidades en Assembly

### Crear programa vulnerable deliberadamente

```bash
cat > vulnerable.c << 'EOF'
#include <string.h>

void copia(char *entrada) {
    char buffer[10];
    strcpy(buffer, entrada);  // ¡Vulnerable!
}

int main(int argc, char **argv) {
    if (argc > 1)
        copia(argv[1]);
    return 0;
}
EOF

gcc -g -O0 -fno-stack-protector -o vulnerable vulnerable.c
```

### Analizar assembly de copia()

```bash
objdump -d -M intel vulnerable | grep -A 20 "<copia>:"
```

Esperado:

```
0000000000400546 <copia>:
  400546:       55                      push   rbp
  400547:       48 89 e5                mov    rbp,rsp
  40054a:       48 83 ec 10             sub    rsp,0x10
  40054e:       48 89 45 f8             mov    QWORD PTR [rbp-0x8],rdi
  400552:       48 8b 45 f8             mov    rax,[rbp-0x8]
  400556:       48 8b 15 b3 0a 20 00    mov    rdx,[rip+0x200ab3]
  40055d:       48 89 c6                mov    rsi,rax
  400560:       bf e0 86 04 00          mov    edi,0x4086e0
  400565:       e8 c6 fe ff ff          call   400430 <strcpy@plt>
  40056a:       90                      nop
  40056b:       c9                      leaveq
  40056c:       c3                      retq
```

### Identificar la vulnerabilidad en assembly

```
40054a:       48 83 ec 10             sub    rsp,0x10   <-- 16 bytes
```

Buffer es 10 bytes, pero hay 16 bytes reservados.
**¿Por qué?** Alineación (16 bytes = x86-64 ABI requirement)

```
40054e:       48 89 45 f8             mov    QWORD PTR [rbp-0x8],rdi
```

Entrada (8 bytes, QWORD) está en [rbp-0x8]

**El problema:** strcpy no tiene límite. Si entrada > 10 bytes, sobrescribe.

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste suma.c y suma_opt.c
- [ ] Usaste objdump para desensamblar ambas versiones
- [ ] Leíste línea por línea la función suma
- [ ] Entendiste cómo llegan los argumentos (registros)
- [ ] Comparaste -O0 vs -O2
- [ ] Identificaste diferencias en assembly
- [ ] Viste cómo se guardan variables locales
- [ ] Analizar función copia() vulnerable
- [ ] Entendiste dónde está el overflow en assembly

---

## Desafío Extra

### Nivel 1: Encontrar gadgets

```bash
# Buscar "pop rdi; ret"
objdump -d -M intel suma | grep -B2 "retq" | grep -A2 "pop.*rdi"

# ¿Cuál es la dirección de este gadget?
# (Será importante para proyectos posteriores sobre ROP)
```

### Nivel 2: Modificar y Recompilar

```bash
# Cambiar suma para tomar 3 argumentos
int suma3(int a, int b, int c) {
    return a + b + c;
}

gcc -g -O0 -o suma3 suma_modificado.c
objdump -d -M intel suma3 | grep -A 20 "<suma3>:"

# ¿En qué registro aparece el tercer argumento?
# Respuesta: edx
```

---

## Conceptos Clave Aprendidos

### Assembly x86-64
- Registros para argumentos: edi, esi, edx, ecx, r8, r9
- Stack frame: push rbp, mov rbp rsp
- Argumentos en registro (calling convention)
- Variables locales en [rbp-offset]

### Opciones de Compilación
- -O0: Sin optimizar (máximo debug)
- -O2: Optimizar (velocidad, menos debug)
- Diferencias enormes en código generado

### Ingeniería Inversa
- objdump desensambla a readable assembly
- Intel syntax es más legible que AT&T
- Convención de llamadas es consistente
- Vulnerabilidades son visibles en assembly

---

## Lecturas Relacionadas

- `recursos/objdump-guia.md` - Más sobre objdump
- `recursos/gdb-guia.md` - Combinado con GDB
- `01-teoria/leccion_03.md` - Convención de llamadas

## Siguiente

**Ejercicio 03:** Detectar vulnerabilidades mediante análisis estático

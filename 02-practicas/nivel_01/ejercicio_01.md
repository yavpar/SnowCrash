# Ejercicio 01: Debugging Básico con GDB

## Objetivos de Aprendizaje

- Usar GDB para inspeccionar variables en runtime
- Entender cómo el compilador organiza variables en memoria
- Detectar valores inesperados antes de que causen problemas
- Usar breakpoints y stepping para seguir ejecución

## Tiempo Estimado

45-60 minutos

## Dificultad

⭐⭐ Intermedio

---

## Parte 1: Programa Misterioso

Hay un programa que calcula algo, pero tiene un comportamiento extraño. Necesitas debuggearlo.

### Crear el programa

```bash
cd ~/SnowCrash/02-practicas/nivel_01
cat > misterio.c << 'EOF'
#include <stdio.h>
#include <string.h>

int procesar_entrada(char *entrada) {
    int contador = 0;
    char buffer[20];
    
    strcpy(buffer, entrada);  // Copia entrada a buffer
    
    for (int i = 0; i < strlen(entrada); i++) {
        if (entrada[i] >= 'A' && entrada[i] <= 'Z') {
            contador++;
        }
    }
    
    printf("Buffer contiene: %s\n", buffer);
    printf("Conteo de mayúsculas: %d\n", contador);
    
    return contador;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <texto>\n", argv[0]);
        return 1;
    }
    
    int resultado = procesar_entrada(argv[1]);
    printf("Resultado final: %d\n", resultado);
    
    return 0;
}
EOF
```

### Compilar con debug

```bash
gcc -g -O0 -o misterio misterio.c
```

### Observar el comportamiento normal

```bash
./misterio "HOLA"
# Salida esperada:
# Buffer contiene: HOLA
# Conteo de mayúsculas: 4
# Resultado final: 4
```

### Observar comportamiento extraño

```bash
./misterio "Esta es una entrada muy larga que probablemente cause problemas"
# ¿Qué pasó? Probablemente crash o valores raros
```

**Tu tarea:** Usar GDB para entender qué salió mal.

---

## Parte 2: Debugging Paso a Paso

### Iniciar GDB

```bash
gdb ./misterio
```

### Ejecutar con argumentos

En GDB, ejecuta con argumentos así:

```bash
(gdb) run "Esta es una entrada muy larga que probablemente cause problemas"
```

Probablemente verás un comportamiento inesperado.

### Buscar dónde empieza el problema

Poner breakpoint al inicio de `procesar_entrada`:

```bash
(gdb) break procesar_entrada
Breakpoint 1 at 0x1169

(gdb) run "HOLA"
Breakpoint 1, procesar_entrada (entrada=0x7ffffffde8b3 "HOLA") at misterio.c:4

(gdb) list
4    int procesar_entrada(char *entrada) {
5        int contador = 0;
6        char buffer[20];
7        
8        strcpy(buffer, entrada);
...
```

### Ver el estado de variables

```bash
(gdb) print entrada
$1 = 0x7ffffffde8b3 "HOLA"

(gdb) print contador
$2 = 0

(gdb) print buffer
$3 = "\000\001\377\377\377\177\000\000\000\000\000\000\000\000\000\000\000\000\000\000"
```

**Nota:** `buffer` no está inicializado aún (contiene basura).

### Stepping - Ir línea por línea

Avanza hasta la línea de `strcpy`:

```bash
(gdb) next
5        int contador = 0;

(gdb) next
6        char buffer[20];

(gdb) next
8        strcpy(buffer, entrada);

(gdb) print buffer
$4 = "\000\001\377\377\377\177"  # Aún basura, no copió

(gdb) next
# Aquí strcpy ejecutó
```

### Ver el buffer DESPUÉS de strcpy

```bash
(gdb) print buffer
$5 = "HOLA"  # ¡Ahora tiene contenido!

(gdb) print entrada
$6 = 0x7ffffffde8b3 "HOLA"  # Igual que antes
```

### Probar con entrada larga

Reinicia y prueba:

```bash
(gdb) run "Esta es una entrada muy larga"
Breakpoint 1, procesar_entrada (entrada=...) at misterio.c:4

(gdb) next
(gdb) next
(gdb) next
# Ahora en strcpy
(gdb) print buffer
$7 = "\000\001\000..."  # Basura

(gdb) step
# Ejecutar strcpy

(gdb) print buffer
$8 = "Esta es una entrada muy larga"  # ¿Pero wait!
```

**Pregunta:** ¿Cabe "Esta es una entrada muy larga" en 20 caracteres?

Respuesta: **NO**. Solo caben 20, pero strcpy copia TODO.

### Inspeccionar memoria para ver la corrupción

```bash
(gdb) x/40x &buffer
0x7ffffffde4c0: 0x61  0x74  0x73  0x45  0x20  0x73  0x65  0x20
                 E    s    t    a      s    e      
0x7ffffffde4c8: 0x61  0x6e    0x75    0x20    0x61    0x64    0x6e    0x65
                 u    n      a     s    p    a    d    n    e
...  # Muchos más bytes

(gdb) x/s &buffer
0x7ffffffde4c0: "Esta es una entrada muy larga que probablemente causa problemas"
```

**¡Eso es un buffer overflow!**

El string es 62 bytes pero buffer solo tiene 20.
Los 42 bytes extra sobrescribieron memoria que no les pertenece.

---

## Parte 3: Preguntas de Investigación

Responde estas usando GDB. Pista: usa `x/Nx &variable` para ver N bytes.

### Pregunta 1: ¿Cuántos caracteres caben?

```bash
# En GDB, cuando está en el buffer:
(gdb) print &buffer
$1 = (char (*)[20]) 0x7ffffffde4c0

(gdb) print &contador
$2 = (int *) 0x7ffffffde4d4

# Distancia en bytes: 0x7ffffffde4d4 - 0x7ffffffde4c0 = ?
# Si son N bytes, entonces solo N pueden escribirse sin pisar contador
```

Calcula cuántos bytes hay entre `buffer` y `contador`.

**Respuesta esperada:** En x86-64, probablemente 20 bytes (para buffer) + 4 bytes padding = 24 bytes.

### Pregunta 2: ¿Qué se sobrescribe?

```bash
(gdb) run "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIII"
# 36 caracteres = 36 bytes

# Cuando crash:
(gdb) backtrace
# ¿De dónde regresa? Probablemente NO es el sitio esperado
```

¿A qué dirección intenta retornar?

### Pregunta 3: Protección

```bash
# Compilar sin buffer overflow protection
gcc -g -O0 -fno-stack-protector -o misterio_sin_proteccion misterio.c

gdb ./misterio_sin_proteccion
(gdb) run "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIII"
```

¿Qué pasa? ¿Se crashea igual?

---

## Parte 4: Solución y Análisis

### El Problema

`strcpy()` copia **sin verificar tamaño**. Si entrada es más larga que buffer, sobrescribe memoria adyacente.

### Versión Segura

```c
#include <stdio.h>
#include <string.h>

int procesar_entrada(char *entrada) {
    int contador = 0;
    char buffer[20];
    
    // SEGURO: strncpy con límite
    strncpy(buffer, entrada, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // Garantizar null-terminator
    
    for (int i = 0; i < strlen(entrada); i++) {
        if (entrada[i] >= 'A' && entrada[i] <= 'Z') {
            contador++;
        }
    }
    
    printf("Buffer contiene: %s\n", buffer);
    printf("Conteo de mayúsculas: %d\n", contador);
    
    return contador;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <texto>\n", argv[0]);
        return 1;
    }
    
    int resultado = procesar_entrada(argv[1]);
    printf("Resultado final: %d\n", resultado);
    
    return 0;
}
```

### Compilar versión segura

```bash
gcc -g -O0 -o misterio_seguro misterio_seguro.c

# Probar
./misterio_seguro "Esta es una entrada muy larga que probablemente causa problemas"
# Debería funcionar sin crash
```

### Debuggear versión segura

```bash
gdb ./misterio_seguro
(gdb) break procesar_entrada
(gdb) run "Esta es una entrada muy larga que probablemente causa problemas"
(gdb) next
(gdb) next
(gdb) next
(gdb) print buffer
$1 = "Esta es una entrada " # Cortado a 19 + null terminator
```

**Ahora está seguro.** El buffer contiene solo lo que cabe.

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste misterio.c con gcc -g -O0
- [ ] Ejecutaste en GDB y viste el buffer overflow
- [ ] Usaste `print` para inspeccionar variables
- [ ] Usaste `next` para stepping línea por línea
- [ ] Usaste `x/` para ver memoria crudamente
- [ ] Identificaste cuál es la vulnerabilidad
- [ ] Creaste versión segura con strncpy
- [ ] Verificaste que versión segura no crashes

---

## Desafío Extra

### Nivel 1: Encontrar el offset exacto

¿Cuántos caracteres necesitas para sobrescribir `contador`?

```bash
# Prueba con entrada de longitud creciente
./misterio_debug "A" * 20   # No sobrescribe
./misterio_debug "A" * 21   # ¿Ahora?
./misterio_debug "A" * 25   # ¿Y ahora?

# Usa GDB para verificar
gdb ./misterio
(gdb) break procesar_entrada
(gdb) run "AAAAAAAAAAAAAAAAAAAA"  # 20 A's
(gdb) print &buffer
(gdb) print &contador
# Calcula distancia

(gdb) run "AAAAAAAAAAAAAAAAAAAAB"  # 21 A's, 1 B
(gdb) print contador  # ¿cambió?
```

### Nivel 2: Intentar controlar el return address

(Avanzado - conocimiento de ROP próximo)

```bash
# Crear patrón único
echo "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIII" > entrada.txt

# Ver si se crashea
./misterio "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIII"

# En GDB:
(gdb) run "AAAABBBBCCCCDDDDEEEEFFFFGGGGHHHHIIII"
# ¿Cuál fue el RIP fallido?

(gdb) backtrace
# ¿Qué dirección ilegal intentó acceder?
```

---

## Conceptos Clave Aprendidos

### Buffer Overflow
- String entra en buffer más pequeño
- Sobrescribe memoria adyacente
- Puede corromper variables locales
- Puede corromper return address (RIP)

### Debugging Interactivo
- GDB permite ver estado en cualquier momento
- `break` pausa ejecución
- `next` ejecuta línea de C
- `step` entra en funciones
- `print` ve variables
- `x/` ve memoria crudamente

### Memoria y Layout
- Variables locales están en stack
- Orden importa (buffer antes de contador)
- Distancia en bytes determina qué se sobrescribe

---

## Lecturas Relacionadas

- `recursos/gdb-guia.md` - Más sobre GDB
- `recursos/glosario.md` - Buffer overflow
- `01-teoria/leccion_03.md` - Stack y memoria

## Siguiente

**Ejercicio 02:** Disassembly y análisis a nivel assembly

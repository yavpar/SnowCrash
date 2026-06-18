# Ejercicio 05: Format String Vulnerabilities

## Lectura Previa Obligatoria

Antes de comenzar este ejercicio, asegúrate de haber leído:
- **Lección 07: Format Strings** (`01-teoria/leccion_07.md`)

En esa lección aprendiste:
- Por qué `printf(usuario)` es vulnerable
- Cómo %x lee valores del stack
- Cómo %s lee strings desde direcciones del stack
- Qué hace %n y por qué es peligroso

---

## Objetivos

- Verificar la vulnerabilidad con inputs reales
- Leer valores del stack con %x
- Encontrar tu propia entrada en el stack
- Entender por qué la versión segura no es vulnerable

## Tiempo Estimado

60-75 minutos

## Dificultad

⭐⭐⭐ Intermedio-Avanzado

---

## Parte 1: Crear el Programa Vulnerable

```bash
cd ~/SnowCrash/02-practicas/nivel_01

cat > formato.c << 'EOF'
#include <stdio.h>
#include <string.h>

void procesar(char *entrada) {
    printf("Tu entrada: ");
    printf(entrada);         // VULNERABLE: entrada es el format string
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <entrada>\n", argv[0]);
        return 1;
    }
    procesar(argv[1]);
    return 0;
}
EOF

gcc -g -O0 -fno-stack-protector -o formato formato.c
```

Y la versión segura para comparar:

```bash
cat > formato_seguro.c << 'EOF'
#include <stdio.h>

void procesar(char *entrada) {
    printf("Tu entrada: ");
    printf("%s", entrada);   // SEGURO: format string es literal
    printf("\n");
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Uso: %s <entrada>\n", argv[0]);
        return 1;
    }
    procesar(argv[1]);
    return 0;
}
EOF

gcc -g -O0 -o formato_seguro formato_seguro.c
```

---

## Parte 2: Verificar la Diferencia

Prueba ambos programas con el mismo input:

```bash
./formato "Hola mundo"
./formato_seguro "Hola mundo"
# Ambos deberían imprimir: Tu entrada: Hola mundo
```

Ahora prueba con un especificador:

```bash
./formato "Hola %x"
./formato_seguro "Hola %x"
```

**¿Qué diferencia ves?**

- `formato` interpretará `%x` y mostrará un valor del stack
- `formato_seguro` imprimirá literalmente `Hola %x`

---

## Parte 3: Leer el Stack con %x

```bash
./formato "%x %x %x %x %x %x %x %x"
```

Verás varios valores hexadecimales. Esos son valores del stack que no deberías ver.

Prueba con más:

```bash
./formato "%x.%x.%x.%x.%x.%x.%x.%x.%x.%x"
```

### Buscar tu propia entrada en el stack

Cuando printf ejecuta, tu string de entrada también está en el stack (como argumento). Si pones suficientes `%x`, eventualmente verás el valor hex de tus propios caracteres.

```bash
./formato "AAAA.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x"
```

Busca `41414141` en la salida (`0x41` = `A` en ASCII).

**¿En qué posición aparece?** Eso te dice el offset de tu entrada.

---

## Parte 4: Leer con Offset Directo

Una vez que sabes el offset, puedes acceder directamente con `%N$x`:

```bash
# Si tu entrada está en el 6to argumento:
./formato "AAAA.%6\$x"
# Debería mostrar: AAAA.41414141
```

Prueba distintos números hasta encontrar el que muestra `41414141`.

---

## Parte 5: Ver Direcciones de Memoria

```bash
./formato "%p.%p.%p.%p.%p.%p.%p.%p"
```

`%p` muestra direcciones en formato `0x...`. Estas son direcciones reales del stack y de libc en tu proceso.

---

## Parte 6: Comparar con Versión Segura

```bash
./formato_seguro "AAAA.%x.%x.%x.%x.%x.%x"
./formato_seguro "%p.%p.%p.%p"
./formato_seguro "%6\$x"
```

Todos deberían imprimir exactamente lo que escribiste, sin interpretar nada.

**Eso es la diferencia entre vulnerable y seguro.**

---

## Parte 7: Análisis con GDB

```bash
gdb ./formato
(gdb) break procesar
(gdb) run "AAAA.%x.%x.%x"

# Cuando pause, ver qué hay en el stack
(gdb) x/20x $rsp

# Ver qué argumentos tiene printf en este momento
(gdb) info args
```

---

## Checkpoints

✅ Completaste cuando:

- [ ] Compilaste formato.c y formato_seguro.c
- [ ] Viste la diferencia entre ambos con `%x`
- [ ] Viste valores del stack con `%x %x %x ...`
- [ ] Encontraste `41414141` (tus A's) en la salida
- [ ] Identificaste en qué offset está tu entrada
- [ ] Usaste `%N$x` para leerlo directamente
- [ ] Confirmaste que formato_seguro no es vulnerable
- [ ] Viste direcciones con `%p`

---

## Desafío Extra

### Nivel 1: Encontrar el canario

```bash
gcc -g -O0 -fstack-protector -o formato_canary formato.c
./formato_canary "AAAA.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x.%x"
```

El canario es un valor del stack que no cambia entre el inicio y el final de la función. Busca un valor que aparezca consistentemente.

### Nivel 2: Usar %s para leer un string

```bash
# Si el stack contiene una dirección que apunta a un string
# %s irá a esa dirección y leerá el string
./formato "AAAA.%s"
# Puede crashear o imprimir basura - eso es esperado
```

¿Por qué puede crashear? Porque `%s` intenta leer desde la dirección que está en el stack, y esa dirección puede no ser válida.

### Nivel 3: Analizar con GDB dónde crashea %s

```bash
gdb ./formato
(gdb) run "AAAA.%s"
# Cuando crashee:
(gdb) backtrace
(gdb) info registers
# ¿Qué dirección intentó leer?
```

---

## Lecturas Relacionadas

- `01-teoria/leccion_07.md` - Teoría completa de format strings
- `recursos/glosario.md` - Format string vulnerability

## Siguiente

**Leer Lección 08** (ROP y Protecciones) antes de continuar con Ejercicio 06.

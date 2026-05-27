# 🎯 Ejercicio 01: Mi Primer Binario

## 📋 Objetivo de Aprendizaje

Después de completar este ejercicio, podrás:

- ✅ Escribir un programa simple en C
- ✅ Compilarlo con `gcc`
- ✅ Ejecutarlo en la terminal
- ✅ Entender la diferencia entre código fuente y binario ejecutable
- ✅ Comprender cómo funciona un programa compilado

**Tiempo estimado:** 20-30 minutos  
**Dificultad:** ⭐ (muy fácil)

---

## 📖 Introducción

En esta lección creamos nuestro **primer programa en C** y lo compilamos a binario.

¿Por qué es importante?

Porque todo lo que haces en seguridad ofensiva involucra **analizar binarios compilados**. Necesitas entender cómo se ven desde adentro.

---

## 🎯 La Tarea

### Parte 1: Crear el Archivo

Vamos a crear un programa simple que dice hola.

**Abre una terminal** y ejecuta:

```bash
# Navega a tu carpeta de proyecto
cd ~/proyectos/42/snowcrash/02-practicas/nivel_00
# O simplemente:
cd /Users/yavier/SnowCrash/02-practicas/nivel_00

# Crea un archivo llamado ejercicio_01.c
cat > ejercicio_01.c << 'EOF'
#include <stdio.h>

int main() {
    printf("Hola, soy un binario!\n");
    return 0;
}
EOF
```

**¿Qué acabas de hacer?**

- Creaste un archivo llamado `ejercicio_01.c`
- Contiene un programa C muy simple
- Solo imprime un mensaje

### Parte 2: Compilar el Programa

Ahora compila el programa:

```bash
gcc -o ejercicio_01 ejercicio_01.c
```

**¿Qué significa?**

```
gcc              = Compilador de C
-o ejercicio_01  = Nombre del binario resultante
ejercicio_01.c   = Archivo fuente a compilar
```

**¿Qué pasó?**

Se creó un nuevo archivo: `ejercicio_01`  
Este es el **binario ejecutable**.

Verifica:

```bash
ls -la
```

Deberías ver algo como:

```
-rw-r--r--  1 user  staff   208 May 24 10:30 ejercicio_01.c
-rwxr-xr-x  1 user  staff  8512 May 24 10:30 ejercicio_01
```

Nota: `ejercicio_01` tiene `x` (permiso de ejecución), `ejercicio_01.c` no.

### Parte 3: Ejecutar el Binario

Ahora ejecuta el binario:

```bash
./ejercicio_01
```

**Salida esperada:**

```
Hola, soy un binario!
```

¡Felicidades! Acabas de crear tu primer binario. 🎉

---

## 💡 Pistas (Si Te Atrapas)

### "No funciona gcc"
Si ves: `command not found: gcc`

**Solución:** GCC no está instalado. En Fedora:
```bash
sudo dnf install gcc
```

### "No puedo ejecutar ./ejercicio_01"
Si ves: `permission denied`

**Solución:** Agrega permiso de ejecución:
```bash
chmod +x ejercicio_01
./ejercicio_01
```

### "¿Dónde está el archivo?"
Si no ves `ejercicio_01.c` o `ejercicio_01`:

**Solución:** Verifica dónde estás:
```bash
pwd  # Muestra dónde estás
ls   # Muestra los archivos en esta carpeta
```

---

## ✅ Solución Completa

Aquí está exactamente lo que deberías hacer:

### Paso 1: Crear el archivo
```bash
cat > ejercicio_01.c << 'EOF'
#include <stdio.h>

int main() {
    printf("Hola, soy un binario!\n");
    return 0;
}
EOF
```

### Paso 2: Compilar
```bash
gcc -o ejercicio_01 ejercicio_01.c
```

### Paso 3: Ejecutar
```bash
./ejercicio_01
```

### Paso 4: Verificar (opcional)
```bash
ls -la
```

---

## 📚 Explicación Línea por Línea

Ahora vamos a entender el código:

```c
#include <stdio.h>
```

- `#include` = "Incluye una librería"
- `<stdio.h>` = Librería de entrada/salida ("Standard I/O")
- **¿Por qué?** Necesitamos `printf()`, que está en esta librería

```c
int main() {
```

- `int` = La función devolverá un número entero
- `main()` = Función especial - aquí comienza el programa
- **¿Por qué?** Todo programa C comienza en `main()`

```c
    printf("Hola, soy un binario!\n");
```

- `printf()` = Función para imprimir texto
- `"Hola, soy un binario!\n"` = El texto a imprimir
- `\n` = Nueva línea (salto de línea)
- **¿Por qué?** Queremos que el programa imprima algo

```c
    return 0;
```

- `return 0` = Devuelve el número 0
- **¿Por qué?** 0 significa "el programa terminó correctamente"

```c
}
```

- Cierra la función `main()`

---

## 🔍 ¿Qué es un "Binario"?

Acabas de crear un archivo llamado `ejercicio_01`. Es el **binario**.

### ¿Qué significa eso?

**Binario = Archivo ejecutable compilado a código máquina**

```
Tu código (texto):           ejercicio_01.c
                                 ↓
                           [gcc compila]
                                 ↓
Binario (código máquina):    ejercicio_01
```

### ¿Qué es "Código Máquina"?

Es el lenguaje que entiende la CPU. Algo como:

```
01001101 01100001 01110010 01101011 00100000
11001100 11001100 11000100 11001100 10101010
```

No es legible para humanos, pero la CPU lo entiende perfectamente.

### ¿Por Qué Importa?

En seguridad ofensiva, **analizamos binarios compilados**, no código fuente.

Porque en el "mundo real":

- ✅ Tienes acceso al binario (el programa ejecutable)
- ❌ NO tienes el código fuente

Tu tarea será entender qué hace ese binario sin verlo en C.

---

## 🔧 ¿Qué es `gcc`?

`gcc` = **GNU C Compiler** = El compilador de C

### ¿Qué hace?

Convierte código C (legible) a código máquina (ejecutable):

```
Entrada:      ejercicio_01.c (texto que entiendes)
                     ↓
                   gcc
                     ↓
Salida:       ejercicio_01 (binario que ejecuta la CPU)
```

### Flags Útiles

```bash
gcc -o nombre            # Nombre del binario resultante
gcc -Wall               # Muestra todas las advertencias
gcc -g                  # Incluye info de debug (para gdb)
gcc -O2                 # Optimiza el código
gcc -static             # Compilación estática
```

Para este ejercicio solo usamos `-o`.

---

## 🎯 Variaciones (Experimenta)

Ahora que entiendes lo básico, intenta:

### Variación 1: Cambiar el Mensaje

Modifica el archivo:

```bash
cat > ejercicio_01.c << 'EOF'
#include <stdio.h>

int main() {
    printf("Mi nombre es [Tu nombre]\n");
    return 0;
}
EOF

gcc -o ejercicio_01 ejercicio_01.c
./ejercicio_01
```

Resultado: Debería imprimir tu nombre.

### Variación 2: Imprimir Múltiples Líneas

```bash
cat > ejercicio_01.c << 'EOF'
#include <stdio.h>

int main() {
    printf("Línea 1\n");
    printf("Línea 2\n");
    printf("Línea 3\n");
    return 0;
}
EOF

gcc -o ejercicio_01 ejercicio_01.c
./ejercicio_01
```

Resultado: Deberían aparecer 3 líneas.

### Variación 3: Imprimir un Número

```bash
cat > ejercicio_01.c << 'EOF'
#include <stdio.h>

int main() {
    int numero = 42;
    printf("El número es: %d\n", numero);
    return 0;
}
EOF

gcc -o ejercicio_01 ejercicio_01.c
./ejercicio_01
```

Resultado: `El número es: 42`

---

## 📊 Antes vs Después

### Antes de este ejercicio:

```
❌ No tenía un programa compilado
❌ No sabía cómo usar gcc
❌ No entendía la diferencia entre código y binario
```

### Después de este ejercicio:

```
✅ Tengo un binario ejecutable
✅ Sé cómo compilar con gcc
✅ Entiendo el flujo: código → compilador → binario
```

---

## 🔮 Conexión con lo Siguiente

Lo que acabas de hacer ahora lo harás en **sentido inverso** en los próximos ejercicios:

**Ahora:**
```
Código C → gcc → Binario → Ejecuta
```

**Próximamente:**
```
Binario → Analizar → Entender → Explotar
```

El próximo ejercicio usará **herramientas de análisis** para inspeccionar binarios.

---

## ✨ Checklist de Completitud

Marca cuando hayas hecho cada cosa:

- [ ] Abrí una terminal
- [ ] Navegué a la carpeta correcta
- [ ] Creé el archivo `ejercicio_01.c`
- [ ] Compilé con `gcc -o ejercicio_01 ejercicio_01.c`
- [ ] Ejecuté `./ejercicio_01`
- [ ] Vi la salida: "Hola, soy un binario!"
- [ ] Intenté una variación (cambiar el mensaje)
- [ ] Compilé y ejecuté nuevamente

Si checaste todo: ✅ **¡Ejercicio completado!**

---

## 🚀 Próximo Paso

Ahora que sabes compilar y ejecutar:

👉 Ve a [Ejercicio 02: Explorando con `file` y `strings`](ejercicio_02.md)

Ahí aprenderás a **inspeccionar el binario** que creaste.

---

## 📚 Recursos Relacionados

- 📖 [recursos/herramientas.md](../../recursos/herramientas.md) - Qué es gcc
- 📖 [recursos/glosario.md](../../recursos/glosario.md) - "Binario", "Compilador"
- 📚 [Lección 01: Introducción](../leccion_01.md) - Si necesitas refrescar

---

**Dificultad:** ⭐  
**Tiempo real:** ~20 minutos  
**Conceptos clave:** Compilación, binarios ejecutables, gcc  
**Estado:** ✅ Completo

---

¡Felicidades! Acabas de crear tu primer binario. 🎉

**Siguiente:** [Ejercicio 02](ejercicio_02.md)

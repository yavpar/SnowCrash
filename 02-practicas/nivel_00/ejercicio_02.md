# 🎯 Ejercicio 02: Explorando con `file` y `strings`

## 📋 Objetivo de Aprendizaje

Después de completar este ejercicio, podrás:

- ✅ Usar la herramienta `file` para identificar tipos de archivos
- ✅ Usar la herramienta `strings` para extraer texto de binarios
- ✅ Entender qué información puedes obtener sin ejecutar un binario
- ✅ Distinguir entre análisis dinámico (ejecutar) y estático (sin ejecutar)

**Tiempo estimado:** 25-35 minutos  
**Dificultad:** ⭐ (muy fácil)  
**Pre-requisito:** [Ejercicio 01](ejercicio_01.md)

---

## 📖 Introducción

En el ejercicio anterior creaste un binario y lo ejecutaste.

Pero ¿y si **no confías en el binario**? ¿Y si quieres **analizarlo sin ejecutarlo**?

En ciberseguridad, a menudo recibimos binarios desconocidos. Antes de ejecutar algo desconocido, quieres:

1. ✅ Identificar qué tipo de archivo es
2. ✅ Ver qué strings (textos) contiene
3. ✅ Decidir si es seguro ejecutarlo

Eso es lo que aprenderás en este ejercicio.

---

## 🎯 La Tarea

### Parte 1: Entender `file`

La herramienta `file` te dice **qué tipo de archivo es** algo.

#### Uso Básico

```bash
cd /Users/yavier/SnowCrash/02-practicas/nivel_00

file ejercicio_01.c
```

**Salida esperada:**

```
ejercicio_01.c: C source code text
```

Ahora prueba con el binario:

```bash
file ejercicio_01
```

**Salida esperada:**

```
ejercicio_01: Mach-O 64-bit executable x86_64
```

**¿Qué significa?**

- `Mach-O` = Formato de archivo para macOS/Darwin
- (Si estuvieras en Fedora sería: `ELF 64-bit LSB executable`)
- `64-bit` = Arquitectura de 64 bits
- `x86_64` = Tipo de procesador
- `executable` = Puede ser ejecutado

#### ¿Por Qué Es Importante?

`file` te dice:

- ✅ ¿Es un binario ejecutable?
- ✅ ¿Es de 32 o 64 bits?
- ✅ ¿Para qué arquitectura fue compilado?
- ✅ ¿Es un script o un binario?

---

### Parte 2: Entender `strings`

La herramienta `strings` extrae todos los **textos legibles** de un archivo binario.

#### Uso Básico

```bash
strings ejercicio_01
```

**Salida esperada:**

```
/usr/lib/dyld
__pagezero
__text
__data
...
Hola, soy un binario!
...
```

¿Ves `Hola, soy un binario!`? **Ese es el string que imprimiste.**

`strings` buscó en todo el binario y extrajo todos los textos.

#### ¿Por Qué Es Importante?

`strings` te permite:

- ✅ Ver mensajes de error sin ejecutar
- ✅ Ver rutas de archivos que usa el programa
- ✅ Ver URLs o direcciones IP
- ✅ Ver nombres de funciones
- ✅ Descubrir información sobre vulnerabilidades

---

### Parte 3: Análisis Estático vs Dinámico

**Concepto importante:**

```
ANÁLISIS ESTÁTICO = Analizar sin ejecutar
  Herramientas: file, strings, objdump, IDA Pro

ANÁLISIS DINÁMICO = Analizar ejecutando
  Herramientas: gdb, strace, ltrace
```

#### Este Ejercicio Es Análisis Estático

```bash
file ejercicio_01        # Análisis estático
strings ejercicio_01     # Análisis estático
./ejercicio_01           # Análisis dinámico (ejecutas)
```

---

## 💡 Pistas (Si Te Atrapas)

### "No funciona `file`"
```bash
# Intenta con la ruta completa
/usr/bin/file ejercicio_01
```

### "No encuentro `strings`"
```bash
# En Fedora
sudo dnf install binutils

# Luego
strings ejercicio_01
```

### "Veo demasiado output"
Si `strings` produce mucho texto:

```bash
# Filtra solo líneas legibles
strings ejercicio_01 | grep -v "^[^a-zA-Z0-9]"

# O guarda en un archivo
strings ejercicio_01 > strings_output.txt
cat strings_output.txt
```

---

## ✅ Solución Completa

### Paso 1: Navega a la carpeta
```bash
cd /Users/yavier/SnowCrash/02-practicas/nivel_00
```

### Paso 2: Analiza el archivo con `file`
```bash
file ejercicio_01.c
file ejercicio_01
```

### Paso 3: Extrae strings
```bash
strings ejercicio_01
```

### Paso 4: Filtra para ver solo strings interesantes
```bash
strings ejercicio_01 | grep "Hola"
```

**Salida esperada:**

```
Hola, soy un binario!
```

¡Lo encontraste sin ejecutar el programa! 🎉

---

## 📚 Explicación Detallada

### ¿Qué Es `file` Realmente?

`file` inspecciona los primeros bytes de un archivo (el "magic number") para determinar su tipo.

```
ARCHIVO:                MAGIC NUMBER:    TIPO:
ejercicio_01.c          Texto            Código C
ejercicio_01            MZ o ELF         Ejecutable
imagen.jpg              FF D8 FF E0      JPEG
documento.pdf           %PDF             PDF
```

### ¿Qué Es `strings` Realmente?

`strings` busca en el archivo binario todas las secuencias de caracteres imprimibles.

```
BINARIO (en hexadecimal):
48 65 6C 6C 6F 20 57 6F 72 6C 64

STRINGS (decodificado):
Hello World
```

### ¿Por Qué Los Strings Están en el Binario?

Cuando compilas con `gcc`:

```c
printf("Hola, soy un binario!\n");
```

El compilador **guarda ese string en el binario** para que `printf()` pueda usarlo.

No se "borra". Se queda ahí, escondido en el binario, lista para ser encontrado con `strings`.

---

## 🔍 Ejemplo Real: Analizar un Binario Desconocido

Imagina que alguien te da un archivo misterioso: `binario_misterio`

### Paso 1: ¿Qué es?
```bash
file binario_misterio
```
Resultado: `ELF 64-bit executable`

Bien, es un binario Linux ejecutable.

### Paso 2: ¿Qué contiene?
```bash
strings binario_misterio | head -20
```

Resultado: Ves mensajes como:
```
/etc/passwd
rm -rf /
...
```

⚠️ **Alarma:** Ese binario intenta acceder a `/etc/passwd` y ejecutar comandos peligrosos.

### Decisión:
```
❌ NO lo ejecuto (podría dañar mi sistema)
✅ Lo analizo más con otras herramientas
```

Sin ejecutar, ya descubriste que es **peligroso**.

---

## 🎯 Variaciones (Experimenta)

### Variación 1: Analiza tu programa del Ejercicio 01

Ya lo hiciste, pero repite varios comandos:

```bash
file ejercicio_01
strings ejercicio_01
strings ejercicio_01 | wc -l        # Cuántos strings hay
strings ejercicio_01 | head -10     # Primeros 10
strings ejercicio_01 | tail -10     # Últimos 10
```

### Variación 2: Analiza un Programa del Sistema

```bash
file /bin/ls
strings /bin/ls | grep -i error
strings /bin/ls | head -20
```

**¿Qué ves?**

Textos de mensajes de error, rutas, nombres de funciones.

### Variación 3: Guarda el Output

```bash
strings ejercicio_01 > mis_strings.txt
cat mis_strings.txt
```

Ahora tienes todos los strings en un archivo para analizar.

---

## 📊 Comparación: Código vs Binario

### El Código (Lo Escribiste):

```c
#include <stdio.h>

int main() {
    printf("Hola, soy un binario!\n");
    return 0;
}
```

**Muy pequeño, muy claro.**

### El Binario (Lo Compiló):

```bash
strings ejercicio_01 | wc -l
```

Resultado: **~50-100 strings** (mucho más)

¿Por qué tanto?

- El compilador agregó librerías
- Sistema operativo agrega símbolos
- Headers y metadatos

Pero tu string está ahí: "Hola, soy un binario!"

---

## 🔮 Conexión con lo Siguiente

### Próximamente (Ejercicio 03):

Veremos **cómo la memoria funciona** dentro del programa.

### Luego (Lección 02):

Aprenderemos sobre **ELF format** (estructura de binarios Linux) en profundidad.

### Avanzado (Lección 04):

Usaremos `gdb` para **ejecutar paso a paso** y ver exactamente qué hace cada instrucción.

---

## ✨ Checklist de Completitud

- [ ] Ejecuté `file ejercicio_01.c`
- [ ] Ejecuté `file ejercicio_01`
- [ ] Vi la diferencia en los outputs
- [ ] Ejecuté `strings ejercicio_01`
- [ ] Encontré mi string: "Hola, soy un binario!"
- [ ] Filtré con `grep` para encontrar solo strings específicos
- [ ] Conté cuántos strings hay
- [ ] Experimenté con archivos del sistema (`/bin/ls`)

Si checaste todo: ✅ **¡Ejercicio completado!**

---

## 🚀 Próximo Paso

Ahora que sabes **inspeccionar binarios estáticamente**:

👉 Ve a [Ejercicio 03: Entendiendo la Memoria](ejercicio_03.md)

Ahí aprenderás cómo **la memoria de un programa** se organiza.

---

## 📚 Recursos Relacionados

- 📖 [recursos/herramientas.md](../../recursos/herramientas.md) - Detalles de `file` y `strings`
- 📖 [recursos/glosario.md](../../recursos/glosario.md) - "Binario", "ELF", "String"
- 📚 [Lección 01](../leccion_01.md) - Si necesitas refrescar conceptos

---

**Dificultad:** ⭐  
**Tiempo real:** ~25 minutos  
**Conceptos clave:** Análisis estático, `file`, `strings`, magic numbers  
**Estado:** ✅ Completo

---

¡Excelente! Ya sabes inspeccionar binarios sin ejecutarlos. 🔍

**Siguiente:** [Ejercicio 03](ejercicio_03.md)

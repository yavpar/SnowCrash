# 📚 Lección 01: Introducción a Seguridad Informática

## 🎯 Objetivos de Aprendizaje

Al terminar esta lección, entenderás:

- ✅ Qué es la seguridad informática
- ✅ Qué es una vulnerabilidad y por qué existe
- ✅ Qué es un exploit y cómo se diferencia de una vulnerabilidad
- ✅ Por qué estudiamos ciberseguridad desde una perspectiva ofensiva
- ✅ Qué es ft_snowcrash y por qué es importante
- ✅ Cómo este proyecto te prepara para el futuro

**Tiempo estimado:** 30-45 minutos

---

## 📖 Introducción

¿Alguna vez te has preguntado cómo los "hackers" rompen sistemas? ¿Cómo acceden a información que no debería ser pública? ¿Cómo es posible que un programa "falle" de una manera que beneficia a alguien?

La respuesta es: **entendiendo las vulnerabilidades**.

Este curso te enseñará exactamente eso. No para atacar sistemas ilegalmente, sino para **comprenderlos profundamente** y poder defenderlos mejor.

---

## 1️⃣ ¿QUÉ ES LA SEGURIDAD INFORMÁTICA?

### Definición Simple

**Seguridad Informática = Proteger datos y sistemas de acceso no autorizado**

Pero eso es muy vago. Déjame ser más explícito.

### Desglose

Imagina que tienes una casa:

```
LA CASA:
├─ Puertas          ← "Protecciones" del software
├─ Ventanas         ← Puntos de entrada
├─ Paredes          ← Límites del sistema
├─ Candados         ← Contraseñas y permisos
└─ Alarma           ← Sistemas de detección
```

**Seguridad informática** es asegurar que:

1. ✅ Solo las personas autorizadas entren
2. ✅ La información dentro esté protegida
3. ✅ Si alguien intenta entrar, lo detectemos
4. ✅ Si entra, cause el menor daño posible

### Ejemplo Concreto: Una Base de Datos

```
Base de datos de un banco:
├─ Datos públicos: Direcciones de sucursales
├─ Datos privados: Saldos de cuentas
└─ Datos ultra-secretos: Claves maestras

Seguridad:
✓ Los saldos NO deben ser visibles para cualquiera
✓ Las claves maestras deben estar MUY protegidas
✓ Si alguien intenta ver un saldo sin permiso, bloqueamos
✓ Si lo logra, dejamos evidencia (logs)
```

---

## 2️⃣ ¿QUÉ ES UNA VULNERABILIDAD?

### Definición

**Vulnerabilidad = Un error o debilidad en un sistema que permite hacer algo no previsto**

### Lo Importante

Una vulnerabilidad **NO es un ataque**. Es un **defecto** que *permite* ataques.

### Analogía con la Casa

```
Tu casa tiene una cerradura.

ESCENARIO 1 - Sin vulnerabilidad:
  El ladrón intenta entrar
  → La cerradura funciona correctamente
  → NO puede entrar
  → Fin de la historia

ESCENARIO 2 - Con vulnerabilidad:
  El ladrón intenta entrar
  → Descubre que la cerradura tiene un defecto
  → Puede abrir sin llave
  → Entra sin problema
```

**La vulnerabilidad es el defecto de la cerradura, no el acto del ladrón.**

---

## 3️⃣ EJEMPLO REAL: BUFFER OVERFLOW

Ahora vamos a ver un ejemplo concreto de vulnerabilidad en programación.

### El Escenario

Imagina un programa que pide tu nombre:

```
¿Cuál es tu nombre?
> Juan
Hola, Juan!
```

Parece inofensivo, ¿verdad?

### El Código (Simplificado)

```c
#include <stdio.h>
#include <string.h>

int main() {
    char nombre[10];  // Espacio para 10 caracteres
    
    printf("¿Cuál es tu nombre?\n");
    scanf("%s", nombre);  // ← PROBLEMA AQUÍ
    
    printf("Hola, %s!\n", nombre);
    return 0;
}
```

### ¿Cuál es el Problema?

La variable `nombre` tiene espacio para **10 caracteres**. Pero `scanf("%s", nombre)` **NO verifica el límite**.

### ¿Qué Pasa Si?

Si el usuario escribe un nombre MÁS LARGO que 10 caracteres:

```
¿Cuál es tu nombre?
> JuanJuanJuanJuanJuan (22 caracteres)
[CRASH! - El programa se bloquea o hace algo inesperado]
```

### ¿Por Qué Crash?

Porque los 22 caracteres no caben en 10 espacios. Los caracteres extras se sobrescriben en memoria, **corrompiendo datos que no deberían ser tocados**.

```
MEMORIA:
┌────────────────────────────────────────┐
│ Variables del programa                 │
├────────────────────────────────────────┤
│ nombre: [10 espacios]                  │  ← Aquí guardan el nombre
│ [Datos importantes]                    │  ← Esto NO debería tocarse
│ [Más datos]                            │
│ [Dirección de retorno de función]      │  ← ¡MUY IMPORTANTE!
└────────────────────────────────────────┘

Si escribimos 22 caracteres:
│ nombre: [XXXXXXXXXXXXXXXXXXXX]          │  ← Los 10 + 12 extras
│ [XXXXX] ← Sobrescribió datos           │  ← ¡PROBLEMA!
```

### El Punto Clave

**La vulnerabilidad es:** El programa no valida la longitud del input.

**El exploit es:** Usar esa vulnerabilidad para causar un crash (o peor).

---

## 4️⃣ VULNERABILIDAD vs EXPLOIT

Esta es una distinción **CRÍTICA**. Mucha gente las confunde.

### Definiciones

| Concepto | Definición | Ejemplo |
|----------|-----------|---------|
| **Vulnerabilidad** | El defecto en el código | `scanf("%s")` sin validación |
| **Exploit** | El código/técnica que usa la vulnerabilidad | Un script que envía muchos caracteres |
| **Attack** | El acto de usar el exploit | Alguien ejecuta el script contra el programa |

### Visual

```
Vulnerabilidad:  DEFECTO EN EL CÓDIGO
        ↓
Exploit:         FORMA DE USAR ESE DEFECTO
        ↓
Attack:          ALGUIEN EJECUTA EL EXPLOIT
        ↓
Damage:          LO QUE PASA DESPUÉS
```

### Otro Ejemplo: Una Puerta Sin Candado

```
Casa con una puerta:

VULNERABILIDAD: 
  → La puerta no tiene candado (error de diseño)

EXPLOIT: 
  → Un script que abre cualquier puerta sin candado

ATTACK: 
  → Un ladrón usa ese script en tu casa

DAMAGE: 
  → Te roban cosas
```

---

## 5️⃣ ¿POR QUÉ ESTUDIAMOS CIBERSEGURIDAD "OFENSIVA"?

### La Pregunta

"¿Por qué aprendo a romper sistemas si quiero protegerlos?"

### La Respuesta

**No puedes defender algo que no entiendes.**

### Analógía

```
Si quieres ser un buen cerrajero:
  ✗ No aprendes solo "cómo funcionan las cerraduras"
  ✓ Aprendes cómo abrir cerraduras sin llave

Si quieres ser un buen cocinero:
  ✗ No aprendes solo "cómo funcionan los cuchillos"
  ✓ Aprendes a usar todos los cuchillos, incluso cortarte

Si quieres ser bueno en seguridad:
  ✗ No aprendes solo "cómo funcionan los firewalls"
  ✓ Aprendes a encontrar y explotar vulnerabilidades
```

### Los Dos Lados

```
        ATACANTE                    DEFENSOR
        --------                    --------
        
Busca:  ¿Cómo entro?        Busca:  ¿Por dónde entran?
Usa:    Exploits            Usa:    El mismo conocimiento
Meta:   Acceso no autorizado Meta:   Prevenir ese acceso

Resultado: Los defensores que conocen ataques 
           construyen sistemas MUCHO más seguros
```

### Ejemplo Real: Google

Google tiene miles de ingenieros de seguridad que:

1. Buscan vulnerabilidades en Chrome
2. Las explotan (antes de lanzar)
3. Las arreglan

Si Google NO explotara sus propias vulnerabilidades, los hackers lo harían.

---

## 6️⃣ ¿QUÉ ES UN PAYLOAD?

Hay un término más que debes conocer: **payload**.

### Definición

**Payload = El "contenido" o "carga" que envías para explotar una vulnerabilidad**

### Ejemplos

```
VULNERABILIDAD: Programa acepta nombres muy largos

PAYLOAD SIMPLE:
  → 1000 caracteres 'A'
  → Causa un crash

PAYLOAD AVANZADO:
  → 1000 caracteres específicamente diseñados
  → En lugar de crash, ejecuta un comando
  → El comando abre una puerta trasera en el sistema
```

### Visual

```
        INPUT (lo que escribes)
            ↓
┌──────────────────────────────────┐
│ PROGRAMA VULNERABLE              │
│                                  │
│ Recibe tu input                  │
│ NO valida longitud               │ ← VULNERABILIDAD
│ Escribe en memoria sin límite    │
└──────────────────────────────────┘
            ↓
    ALGO MALO PASA
    (crash, falla, ejecuta código)
    
Tu input → EXPLOIT → PAYLOAD
```

---

## 7️⃣ TIPOS COMUNES DE VULNERABILIDADES

No necesitas memorizar estos ahora, pero es bueno saberlos:

### 1. Buffer Overflow
**Qué es:** Escribir más datos de los que caben en un espacio de memoria

**Resultado:** Corrompe datos, puede ejecutar código

**Ejemplo:** Lo que vimos arriba con `scanf("%s")`

### 2. SQL Injection
**Qué es:** Insertar comandos SQL maliciosos en un campo de entrada

**Resultado:** Acceso no autorizado a bases de datos

**Ejemplo:**
```
Campo: "Ingresa tu usuario"
Input normal: Juan
Input malicioso: admin' --

Resultado: Acceso como admin sin contraseña
```

### 3. Cross-Site Scripting (XSS)
**Qué es:** Inyectar código JavaScript en una página web

**Resultado:** Robo de sesiones, credenciales, datos

### 4. Format String
**Qué es:** Manipular funciones de formato (`printf`) para leer/escribir memoria

**Resultado:** Lectura de datos secretos, ejecución de código

### 5. Command Injection
**Qué es:** Inyectar comandos del sistema operativo

**Resultado:** Ejecución de comandos arbitrarios en el servidor

**Ejemplo:**
```
Sistema pide nombre de archivo
Input normal: documento.txt
Input malicioso: documento.txt; rm -rf /

Resultado: Borra todo el servidor
```

---

## 8️⃣ ¿QUÉ ES ft_snowcrash?

### La Realidad

**ft_snowcrash es un proyecto educativo de École 42** diseñado para enseñar exactamente lo que hemos visto en esta lección.

### Estructura

```
ft_snowcrash tiene:

NIVELES:  0 → 1 → 2 → ... → 9 → Bonus
          ↓
        CADA NIVEL:
          • Binarios ejecutables vulnerables
          • Documentación de la vulnerabilidad
          • La tarea: Explotar la vulnerabilidad
```

### Cómo Funciona

```
Nivel 1 (Ejemplo):
  Te dan un programa compilado
  
  Tienes que:
  1. Entender cómo funciona
  2. Encontrar la vulnerabilidad
  3. Crear un exploit
  4. Ejecutar el exploit para "romper" el programa
  
  Recompensa:
  Si lo logras, accedes al siguiente nivel
```

### Por Qué Es Importante

ft_snowcrash te enseña:

1. ✅ Cómo analizar binarios sin código fuente
2. ✅ Cómo encontrar vulnerabilidades
3. ✅ Cómo crear exploits
4. ✅ Cómo usar herramientas profesionales (`gdb`, `objdump`, etc.)

---

## 9️⃣ ¿CÓMO ENCAJA ESTO EN LA SERIE?

### La Ruta Completa de École 42 (Cybersecurity)

```
ft_snowcrash (AQUÍ ESTAMOS)
    ↓
Aprenderás:
- Cómo funcionan binarios
- Cómo funciona la memoria
- Vulnerabilidades básicas
- Herramientas de análisis

    ↓
    ↓
Siguiente Proyecto: Explotación Avanzada
    ↓
Aprenderás:
- Buffer overflow avanzado
- ROP chains
- Evasión de protecciones (ASLR, NX, Canaries)

    ↓
    ↓
Después: Kernel Exploitation, Malware, etc.
```

### El Punto Clave

**Cada proyecto de esta serie construye sobre el anterior.**

Si dominas ft_snowcrash:
- ✅ Los siguientes serán MÁS FÁCILES
- ✅ Entenderás conceptos más avanzados
- ✅ Serás un verdadero experto en seguridad

---

## 🔟 RESUMEN RÁPIDO

Hemos aprendido:

| Concepto | Qué Es |
|----------|--------|
| **Seguridad Informática** | Proteger sistemas de acceso no autorizado |
| **Vulnerabilidad** | Un defecto o debilidad en el código |
| **Exploit** | Una técnica que usa la vulnerabilidad |
| **Payload** | Lo que envías para explotar |
| **Buffer Overflow** | Escribir más datos de los que caben |
| **ft_snowcrash** | Proyecto para aprender seguridad ofensiva |

---

## 🔮 HINT FUTURO

Los conceptos que aprendemos aquí (especialmente **buffer overflow**) serán **CRÍTICOS** en:

- Explotación binaria avanzada (Proyecto 2)
- Return-oriented programming (ROP)
- Kernel exploitation
- Todos los proyectos posteriores de seguridad

**Takeaway:** Domina esto ahora = Avanza rápido después. 🚀

---

## ✅ Próximos Pasos

Ahora que entiendes los conceptos:

1. 👉 Ve a [02-practicas/nivel_00/ejercicio_01.md](../../02-practicas/nivel_00/ejercicio_01.md)
2. 👉 Realiza el primer ejercicio: "Mi Primer Binario"
3. 👉 Luego vuelve aquí si hay algo que no entiendas

---

## 💬 Preguntas Clave (Reflexiona)

Antes de continuar, pregúntate:

- ¿Puedo explicar la diferencia entre vulnerabilidad y exploit?
- ¿Entiendo por qué estudiamos seguridad "ofensiva"?
- ¿Sé qué es un buffer overflow?
- ¿Entiendo por qué es importante aprender esto?

Si respondiste "no" a alguna, **relée esa sección** antes de continuar.

---

## 📚 Recursos Complementarios

- 📖 [recursos/glosario.md](../../recursos/glosario.md) - Si algún término no está claro
- 📖 [recursos/herramientas.md](../../recursos/herramientas.md) - Qué herramientas usaremos
- 🔗 [recursos/referencias.md](../../recursos/referencias.md) - Para leer más

---

**Última actualización:** Mayo 2026  
**Tiempo total de lectura:** ~40 minutos  
**Dificultad:** ⭐ (muy fácil)  
**Estado:** ✅ Completa

---

## 🎊 ¡Felicidades!

Acabas de completar la **Lección 01**. 

Ya entiendes los fundamentos. Ahora es hora de **llevar esto a la práctica**.

**Próxima parada:** [Ejercicio 01: Mi Primer Binario](../../02-practicas/nivel_00/ejercicio_01.md)

🚀

# 🛠️ SETUP.md - Configuración del Entorno en Fedora

Este archivo te guía paso a paso para preparar tu máquina Fedora para el curso de ft_snowcrash.

---

## 📋 Requisitos del Sistema

- **Fedora** (última versión - 40 o superior recomendado)
- **RAM**: Mínimo 2GB (4GB recomendado)
- **Espacio en disco**: Mínimo 5GB disponible
- **Conexión a internet**: Para descargar paquetes

---

## ✅ Paso 1: Actualizar el Sistema

Primero, actualiza Fedora a la última versión de paquetes:

```bash
# Actualizar lista de paquetes
sudo dnf update -y

# Upgrade completo del sistema
sudo dnf upgrade -y

# Reinicias si es necesario (especialmente si se actualizó el kernel)
sudo reboot
```

**¿Qué hace esto?**
- `dnf` es el gestor de paquetes de Fedora (como `apt` en Ubuntu)
- `-y` significa "sí a todas las preguntas"
- `update` trae nuevas versiones de paquetes
- `upgrade` aplica esas actualizaciones

---

## 🔧 Paso 2: Instalar Herramientas Esenciales de Desarrollo

```bash
sudo dnf install -y \
  gcc \
  gdb \
  make \
  binutils \
  git \
  vim \
  nano \
  wget \
  curl
```

**Explicación de cada herramienta:**

| Herramienta | Propósito |
|------------|-----------|
| `gcc` | Compilador de C - necesario para compilar programas |
| `gdb` | Debugger - herramienta para analizar binarios y encontrar errores |
| `make` | Automatiza compilación de proyectos |
| `binutils` | Incluye `objdump`, `strings`, `hexdump` - análisis de binarios |
| `git` | Control de versiones - para trabajar con código |
| `vim`/`nano` | Editores de texto para la terminal |
| `wget`/`curl` | Descargar archivos de internet |

---

## 🔍 Paso 3: Instalar Herramientas de Análisis (Recomendado)

```bash
sudo dnf install -y \
  strace \
  ltrace \
  file \
  hexdump
```

**¿Por qué estas herramientas?**

| Herramienta | Para qué sirve |
|------------|----------------|
| `strace` | Ver qué **syscalls** (llamadas del sistema) hace un programa |
| `ltrace` | Ver qué **funciones de biblioteca** llama un programa |
| `file` | Identificar el tipo de un archivo |
| `hexdump` | Ver el contenido en hexadecimal (base 16) |

---

## 📦 Paso 4: Instalar Python (Opcional pero Recomendado)

```bash
sudo dnf install -y python3 python3-pip
```

**¿Por qué?** Algunos ejercicios pueden usar scripts de Python para generar archivos o ayudarte a entender conceptos.

---

## ✔️ Paso 5: Verificar la Instalación

Ejecuta estos comandos para asegurarte que todo está bien:

```bash
# Verificar gcc
gcc --version

# Verificar gdb
gdb --version

# Verificar git
git --version

# Verificar binutils (objdump)
objdump --version

# Verificar make
make --version
```

**Salida esperada:**
```
gcc (GCC) 13.2.1 20231205 (Red Hat 13.2.1-6)
GNU gdb (GDB) 13.1
git version 2.43.0
GNU objdump 2.40
GNU Make 4.3
```

Si alguno falta o da error, vuelve al Paso 2 y reinstálalo.

---

## 🌍 Paso 6: Configurar Git (Importante)

```bash
# Configurar tu nombre
git config --global user.name "Tu Nombre"

# Configurar tu email
git config --global user.email "tu.email@ejemplo.com"

# Verificar la configuración
git config --global --list
```

**¿Por qué?** Git necesita saber quién eres para hacer commits (guardos de código).

---

## 📂 Paso 7: Crear Estructura de Carpetas

```bash
# Crear carpeta principal
mkdir -p ~/proyectos/42/snowcrash
cd ~/proyectos/42/snowcrash

# Crear subcarpetas
mkdir -p 01-teoria 02-practicas recursos

# Verificar estructura
tree . 2>/dev/null || find . -type d
```

**Nota:** Si `tree` no existe, instálalo:
```bash
sudo dnf install -y tree
```

---

## 🧪 Paso 8: Primer Test - Compilar un Programa Simple

Vamos a verificar que `gcc` funciona correctamente creando un simple programa:

```bash
# Crear un archivo de prueba
cat > ~/proyectos/42/snowcrash/test_gcc.c << 'EOF'
#include <stdio.h>

int main() {
    printf("¡Hola, ft_snowcrash!\n");
    return 0;
}
EOF
```

**¿Qué hace?** Crea un archivo `test_gcc.c` con un simple programa que imprime un mensaje.

Ahora compila:
```bash
cd ~/proyectos/42/snowcrash
gcc -o test_gcc test_gcc.c
```

Ejecuta:
```bash
./test_gcc
```

**Salida esperada:**
```
¡Hola, ft_snowcrash!
```

¡Si ves esto, todo funciona correctamente! 🎉

---

## 🐛 Paso 9: Configurar tu Editor de Código (Opcional)

Si prefieres un editor gráfico, instala uno de estos:

### Opción A: Visual Studio Code
```bash
sudo dnf install -y code
```

### Opción B: Gedit (editor simple)
```bash
sudo dnf install -y gedit
```

### Opción C: Usar Vim o Nano (sin instalar nada extra)
Ambos ya están incluidos.

---

## 📍 Paso 10: Crear un Script de Setup Automático

Aquí está un script que automatiza todo lo anterior:

```bash
# Crear el script
cat > ~/setup-snowcrash.sh << 'EOF'
#!/bin/bash

echo "=== Setup de ft_snowcrash en Fedora ==="

# Actualizar sistema
echo "1/5 Actualizando sistema..."
sudo dnf update -y && sudo dnf upgrade -y

# Instalar herramientas esenciales
echo "2/5 Instalando herramientas de desarrollo..."
sudo dnf install -y gcc gdb make binutils git vim nano wget curl

# Instalar herramientas de análisis
echo "3/5 Instalando herramientas de análisis..."
sudo dnf install -y strace ltrace file hexdump tree

# Instalar Python (opcional)
echo "4/5 Instalando Python..."
sudo dnf install -y python3 python3-pip

# Crear estructura de carpetas
echo "5/5 Creando estructura de carpetas..."
mkdir -p ~/proyectos/42/snowcrash/{01-teoria,02-practicas,recursos}

echo "=== ✅ Setup completado ==="
echo "Verifica la instalación con: gcc --version"
EOF

# Hacer el script ejecutable
chmod +x ~/setup-snowcrash.sh

# Ejecutar el script
~/setup-snowcrash.sh
```

---

## 🆘 Troubleshooting (Solución de Problemas)

### Problema: `command not found: gcc`
**Solución:** Reinstala gcc
```bash
sudo dnf install -y gcc
```

### Problema: `Permission denied` al ejecutar un script
**Solución:** Dale permisos de ejecución
```bash
chmod +x nombre_del_script.sh
```

### Problema: `sudo` pide contraseña constantemente
**Solución:** Es normal. Ingresa tu contraseña de usuario.

### Problema: No tengo conexión a internet en la VM
**Solución:** 
- Verifica que la VM tiene acceso a internet
- Prueba con `ping google.com`
- Si no funciona, consulta la configuración de red de tu hypervisor

---

## ✨ Paso 11: Verificación Final

Ejecuta este script de verificación:

```bash
cat > ~/verify-setup.sh << 'EOF'
#!/bin/bash

echo "=== Verificando Setup de ft_snowcrash ==="
echo ""

# Verificar herramientas
check_tool() {
    if command -v $1 &> /dev/null; then
        echo "✅ $1: Instalado"
    else
        echo "❌ $1: NO instalado"
    fi
}

check_tool gcc
check_tool gdb
check_tool git
check_tool objdump
check_tool python3
check_tool strace
check_tool make

echo ""
echo "=== Fin de verificación ==="
EOF

chmod +x ~/verify-setup.sh
~/verify-setup.sh
```

**Salida esperada:**
```
=== Verificando Setup de ft_snowcrash ===

✅ gcc: Instalado
✅ gdb: Instalado
✅ git: Instalado
✅ objdump: Instalado
✅ python3: Instalado
✅ strace: Instalado
✅ make: Instalado

=== Fin de verificación ===
```

---

## 🎯 Siguientes Pasos

Una vez completado el setup:

1. ✅ Verificaste que todo funciona
2. 📚 Lee [README.md](README.md) para entender la estructura del curso
3. 🚀 Comienza con [01-teoria/leccion_01.md](01-teoria/leccion_01.md)

---

## 📝 Notas Importantes

- **Guarda este archivo**: Lo necesitarás en el futuro si reinstas Fedora
- **Documenta cambios**: Si instalas cosas adicionales, actualiza este archivo
- **Preguntas**: Si algo no funciona, consulta la sección de Troubleshooting o los recursos

---

**Última actualización**: Mayo 2026  
**Plataforma**: Fedora Linux (última versión)

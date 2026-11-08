# Reproductor MP3

Aplicación de escritorio para administrar y reproducir una biblioteca musical de archivos MP3. Incluye un minero de metadatos, almacenamiento mediante SQLite y una interfaz gráfica desarrollada con Qt.

## Tecnologías

- C++17
- Qt 6.12.0 (Widgets y Multimedia)
- FFmpeg (backend multimedia)
- TagLib
- SQLite3
- Meson
- Ninja

## Requisitos

Se necesita un compilador C++ compatible con C++17, Meson, Ninja, pkg-config, TagLib, SQLite3 y Qt 6 con sus módulos Widgets y Multimedia.

### Linux Mint / Ubuntu

Instalar las dependencias de compilación:

```bash
sudo apt update

sudo apt install \
    build-essential \
    meson \
    ninja-build \
    pkg-config \
    libtag1-dev \
    libsqlite3-dev \
    libgl1-mesa-dev
```

### Qt

Instalar Qt mediante el [Qt Online Installer](https://www.qt.io/development/download-qt-installer-oss).

Durante la instalación seleccionar:

- Qt 6.12.0
- Desktop GCC 64-bit
- Qt Multimedia

Se recomienda utilizar la versión de Qt indicada, ya que es la configuración utilizada durante las pruebas del proyecto.

El complemento FFmpeg debe estar disponible en la instalación de Qt Multimedia.

## Configuración del entorno

Suponiendo que Qt está instalado en `~/Qt/6.12.0/gcc_64`, configurar las variables de entorno:

```bash
export QT_ROOT="$HOME/Qt/6.12.0/gcc_64"

export PATH="$QT_ROOT/bin:$PATH"

export PKG_CONFIG_PATH="$QT_ROOT/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"

export LD_LIBRARY_PATH="$QT_ROOT/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

export QT_PLUGIN_PATH="$QT_ROOT/plugins"
```

Comprobar las versiones:

```bash
pkg-config --modversion Qt6Widgets Qt6Multimedia
```

Ambos módulos deben corresponder a Qt 6.12.0.

## Compilación

Clonar el repositorio y acceder al directorio del proyecto:

```bash
git clone <URL_DEL_REPOSITORIO>
cd reproductorMP3
```

Configurar la compilación:

```bash
meson setup build
```

Compilar los ejecutables:

```bash
meson compile -C build
```

## Ejecución

### Reproductor

```bash
./build/src/interfazGrafica/reproductor 
o en su defecto:
QT_MEDIA_BACKEND=ffmpeg ./build/src/interfazGrafica/reproductor
```

### Minero

El minero permite procesar archivos MP3 o recorrer directorios:

```bash
./build/src/minero/minero <ruta>
```

Sustituir `<ruta>` por la ruta de un archivo MP3 o un directorio que contenga música.

## Notas

- La base de datos SQLite se crea automáticamente cuando corresponde.
- La información musical se almacena en la base de datos; los archivos MP3 originales no se modifican.
- El minero procesa archivos que cumplen los requisitos de etiquetas ID3v2.4.0 establecidos por el proyecto.
- Qt debe estar disponible tanto para compilar como para ejecutar el reproductor.
- Los nombres de los paquetes de desarrollo y las dependencias de Qt pueden variar según la distribución Linux.

## Estado de compatibilidad

Entorno probado: Linux Mint, arquitectura x86_64, Qt 6.12.0 y FFmpeg 9.0.1.

La compatibilidad con otras distribuciones debe verificarse según sus versiones de glibc, bibliotecas y dependencias disponibles.
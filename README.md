# Reproductor MP3

Aplicación de escritorio para administrar y reproducir una biblioteca musical de archivos MP3. Incluye un minero de metadatos, almacenamiento mediante SQLite y una interfaz gráfica desarrollada con Qt.

## Tecnologías

- C++17
- Qt 6.12.0 (Widgets y Multimedia)
- FFmpeg 9.0.1 (backend multimedia)
- TagLib
- SQLite3
- Meson
- Ninja

## Requisitos

Se necesita un compilador C++ compatible con C++17, Meson, Ninja, pkg-config, TagLib, SQLite3 y Qt 6 con sus módulos Widgets y Multimedia.

### Linux Mint / Ubuntu

Instalar las dependencias del sistema:

```bash
sudo apt update

sudo apt install \
    build-essential \
    meson \
    ninja-build \
    pkg-config \
    libtag1-dev \
    libsqlite3-dev \
    libgl1-mesa-dev \
    libxcb-cursor0 \
    libxcb-cursor-dev
```

### Qt

Instalar Qt mediante el [Qt Online Installer](https://www.qt.io/development/download-qt-installer-oss).

Durante la instalación seleccionar:

- Qt 6.12.0
- Desktop GCC 64-bit
- Qt Multimedia

Qt Creator es opcional.

Se recomienda utilizar Qt 6.12.0, ya que es la versión utilizada durante las pruebas del proyecto. El backend FFmpeg debe estar disponible en la instalación de Qt Multimedia.

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

## Obtener el proyecto

Clonar el repositorio:

```bash
git clone https://github.com/juaneem-cmyk/reproductorMP3.git
cd reproductorMP3
```

## Compilación

Configurar Meson:

```bash
meson setup build-qt612
```

Compilar los ejecutables:

```bash
meson compile -C build-qt612
```

## Uso

### 1. Inicializar la base de datos

Antes de utilizar la aplicación por primera vez, ejecutar:

```bash
./build-qt612/src/minero/minero --inicializar-bd
```

Este comando crea la base de datos SQLite utilizando el esquema proporcionado. Si la base de datos ya existe, se conserva sin sobrescribirla.

### 2. Importar música

Indicar al minero el directorio que contiene los archivos MP3:

```bash
./build-qt612/src/minero/minero "$HOME/Música"
```

También se puede indicar directamente la ruta de un archivo MP3:

```bash
./build-qt612/src/minero/minero "/ruta/a/cancion.mp3"
```

El minero recorre los directorios, analiza los metadatos y registra las canciones en SQLite. Las canciones que ya están registradas se omiten para conservar los datos existentes.

### 3. Ejecutar el reproductor

```bash
./build-qt612/src/interfazGrafica/reproductor
```

La aplicación utiliza Qt Multimedia con el backend FFmpeg disponible en la instalación de Qt.

## Almacenamiento

La base de datos se guarda fuera del repositorio, en el directorio de datos del usuario:

- Si `XDG_DATA_HOME` está definido: `$XDG_DATA_HOME/reproductorMP3/biblioteca_musical.db`.
- En caso contrario: `~/.local/share/reproductorMP3/biblioteca_musical.db`.

La información musical se almacena en SQLite. Los archivos MP3 originales no se modifican.

## Consideraciones

- El minero procesa archivos que cumplen los requisitos de etiquetas ID3v2.4.0 establecidos por el proyecto.
- Las modificaciones de metadatos realizadas desde la aplicación se conservan en la base de datos.
- La duración de las canciones se obtiene durante la lectura, pero no se almacena en SQLite.
- Las canciones previamente registradas no se sobrescriben al volver a ejecutar el minero.
- Qt debe estar disponible tanto para compilar como para ejecutar el reproductor.

## Estado de compatibilidad

Entorno probado:

- Linux Mint.
- Arquitectura x86_64.
- Qt 6.12.0.
- FFmpeg 9.0.1.

La compatibilidad con otras distribuciones debe verificarse según sus versiones de glibc, bibliotecas y dependencias disponibles.
# Reproductor MP3

Aplicación de escritorio para administrar y reproducir una biblioteca musical de archivos MP3. El proyecto está desarrollado en C++17 y utiliza SQLite para conservar la información musical, TagLib para leer metadatos ID3v2.4.0 y Qt 6 para la interfaz gráfica y la reproducción.

La aplicación permite importar música desde la propia interfaz o mediante un minero independiente por terminal. También permite editar canciones, álbumes, personas e intérpretes grupales. **Los archivos MP3 originales se leen, pero no se modifican:** las ediciones de metadatos se guardan en la base de datos.

## Funcionalidades

- Importación de archivos MP3 desde la interfaz gráfica o desde el minero de terminal.
- Recorrido de directorios y subdirectorios para localizar archivos MP3.
- Lectura de etiquetas ID3v2.4.0 y de las imágenes incrustadas compatibles.
- Asociación de canciones con álbumes a partir de los metadatos y las rutas de los directorios.
- Administración de canciones, álbumes, personas e intérpretes grupales.
- Registro de personas aunque todavía no tengan canciones asociadas.
- Administración de integrantes de grupos.
- Búsqueda por título, artista e información del álbum.
- Vistas de lista y galería.
- Reproducción de MP3 con controles de reproducción, progreso y volumen.
- Portadas e imágenes personalizadas almacenadas fuera de los archivos MP3.

## Tecnologías

- C++17
- Qt 6.12.0: Widgets, Multimedia y Concurrent
- Backend FFmpeg 9.0.1 para Qt Multimedia (entorno utilizado durante las pruebas)
- TagLib
- SQLite3
- Meson
- Ninja
- pkg-config

## Requisitos

Se necesita un compilador C++ compatible con C++17, Meson, Ninja, pkg-config, TagLib, SQLite3 y una instalación de Qt 6 que incluya los módulos Widgets, Multimedia y Concurrent.

### Linux Mint / Ubuntu

Instalar las dependencias disponibles en los repositorios de la distribución:

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

Para reproducir el entorno con el que se desarrolló y probó el proyecto, instalar Qt mediante el [Qt Online Installer](https://www.qt.io/development/download-qt-installer-oss).

En el instalador seleccionar:

- Qt 6.12.0.
- Desktop GCC 64-bit.
- Qt Widgets.
- Qt Multimedia.
- Qt Concurrent, si aparece como componente seleccionable de la instalación.

Qt Creator es opcional; no es necesario para compilar el proyecto desde la terminal.

El entorno de referencia utilizó Qt 6.12.0 y el backend FFmpeg 9.0.1 de Qt Multimedia. Otras versiones de Qt podrían funcionar, pero no se consideran verificadas por las pruebas de este proyecto.

## Compatibilidad entre instalaciones de Qt

Durante el desarrollo se encontró un error al cambiar de canciones en el reproductor provocado por la versión utilizada de Qt, el error
aparentemente no afectaba a la funcionalidad, sin embargo, para evitar los Warnings se opto por cambiar de versión.

**La solución utilizada en el entorno de desarrollo fue seleccionar Qt 6.12.0 de forma consistente tanto al configurar/compilar como al ejecutar la aplicación.** No se concluyó que Qt 6.4.2 sea incompatible en general; simplemente, esa versión del sistema no fue la combinación validada para la entrega. La compatibilidad comprobada corresponde al entorno de referencia indicado en este README.

### Seleccionar una instalación concreta de Qt

Suponiendo que Qt 6.12.0 está instalado en `~/Qt/6.12.0/gcc_64`, configurar las variables de entorno en la terminal desde la que se compilará y ejecutará el proyecto:

```bash
export QT_ROOT="$HOME/Qt/6.12.0/gcc_64"

export PATH="$QT_ROOT/bin:$PATH"

export PKG_CONFIG_PATH="$QT_ROOT/lib/pkgconfig${PKG_CONFIG_PATH:+:$PKG_CONFIG_PATH}"

export LD_LIBRARY_PATH="$QT_ROOT/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}"

export QT_PLUGIN_PATH="$QT_ROOT/plugins"
```

Comprobar que los módulos de Qt se resuelven desde la misma instalación:

```bash
pkg-config --modversion Qt6Widgets Qt6Multimedia Qt6Concurrent
```

Los tres módulos deben mostrar la misma versión, en el entorno probado `6.12.0`. Si alguno devuelve `6.4.2` u otra versión, revisar `PATH` y `PKG_CONFIG_PATH` antes de configurar Meson.

Si se cambia de instalación o versión de Qt después de haber configurado el proyecto, conviene eliminar el directorio de compilación para que Meson no reutilice las dependencias detectadas anteriormente:

```bash
rm -rf build-qt612
```

Después se puede volver a ejecutar la configuración y compilación indicadas más adelante. Este comando elimina únicamente los archivos generados de compilación dentro de `build-qt612`, no elimina el código fuente ni la base de datos de la biblioteca.

## Tipografía opcional

La interfaz utiliza **Press Start 2P** en algunos encabezados para reforzar la estética pixel-art inspirada en Minecraft. Esta fuente es completamente opcional: **no es una dependencia del proyecto y no hace falta instalarla para compilar ni ejecutar la aplicación**.

Quien quiera obtener ese aspecto puede descargarla desde [Google Fonts – Press Start 2P](https://fonts.google.com/specimen/Press+Start+2P) e instalarla en su sistema. Si no está disponible, la ventana principal utiliza una fuente alternativa (DejaVu Sans Mono) para los encabezados. Los textos de canciones y metadatos mantienen una tipografía convencional para facilitar la lectura.

## Obtener el proyecto

Clonar el repositorio:

```bash
git clone https://github.com/juaneem-cmyk/reproductorMP3.git
cd reproductorMP3
```

## Compilación

Ejecutar los siguientes comandos desde la raíz del repositorio, en la misma terminal donde se configuraron las variables de entorno de Qt:

```bash
meson setup build-qt612
meson compile -C build-qt612
```

Si el directorio `build-qt612` ya existe y no se ha cambiado la instalación de Qt, normalmente basta con ejecutar:

```bash
meson compile -C build-qt612
```

Los ejecutables generados son:

- Interfaz gráfica: `build-qt612/src/interfazGrafica/reproductor`.
- Minero independiente: `build-qt612/src/minero/minero`.

## Primer uso: inicializar la base de datos

La base de datos debe inicializarse **una sola vez antes del primer uso**. Esto no significa que sea necesario minar una carpeta antes de abrir la interfaz: la importación de música también está disponible desde la propia aplicación.

Ejecutar:

```bash
./build-qt612/src/minero/minero --inicializar-bd
```

Este comando crea la base de datos SQLite a partir del esquema original del proyecto. Si la base de datos ya existe, no vuelve a ejecutar el esquema ni sobrescribe su contenido.

La aplicación guarda la base de datos fuera del repositorio, en el directorio de datos del usuario. La ubicación exacta se describe en la sección [Almacenamiento](#almacenamiento).

## Uso de la aplicación gráfica

Después de inicializar la base de datos, abrir el reproductor:

```bash
./build-qt612/src/interfazGrafica/reproductor
```

Para agregar música no es necesario ejecutar previamente el minero desde la terminal:

1. Abrir el reproductor.
2. Pulsar el botón **Importar música** de la barra de herramientas.
3. Seleccionar el directorio que contiene los archivos MP3.
4. Esperar a que termine la importación; el proceso se ejecuta en segundo plano y la lista se actualiza al finalizar.

La interfaz permite editar la información de canciones, álbumes, personas e intérpretes grupales. Los cambios de metadatos se guardan en SQLite y no se escriben en las etiquetas de los archivos MP3.

## Uso opcional del minero desde la terminal

El minero independiente es una alternativa para importar música sin utilizar la interfaz gráfica. No es un paso previo obligatorio para abrir el reproductor.

Para minar un directorio y sus subdirectorios:

```bash
./build-qt612/src/minero/minero "$HOME/Música"
```

También acepta la ruta de un único archivo MP3:

```bash
./build-qt612/src/minero/minero "/ruta/a/cancion.mp3"
```

La base de datos debe haber sido inicializada previamente. El minero busca archivos MP3, lee sus etiquetas y registra la información en SQLite. Si una ruta ya está registrada, la canción se omite para conservar las modificaciones que se hayan realizado desde la aplicación.

## Reglas de importación y metadatos

- El minero acepta archivos MP3 que contengan etiquetas **ID3v2.4.0**. Los archivos sin etiqueta ID3v2 o con otra versión (por ejemplo, ID3v2.3.0) se omiten.
- Una etiqueta ID3v2.4.0 puede tener campos vacíos. La información disponible se importa y los datos faltantes pueden completarse desde la interfaz.
- Se leen datos como título, artista, álbum, año, número de pista, género, información adicional e imágenes incrustadas compatibles.
- La duración se obtiene durante la lectura, pero no se almacena en SQLite.
- Para asociar una canción con un álbum, el minero busca entre los directorios superiores uno cuyo nombre coincida con el metadato del álbum. Si no encuentra una coincidencia, la canción se registra sin álbum asociado.
- Las canciones cuya ruta ya existe en la base de datos no se vuelven a insertar ni sobrescriben los datos editados manualmente.
- La importación y las ediciones no modifican las etiquetas ni el contenido de los MP3 originales.

## Almacenamiento

La base de datos se guarda fuera del repositorio, en el directorio de datos del usuario:

- Si `XDG_DATA_HOME` está definido y no está vacío: `$XDG_DATA_HOME/reproductorMP3/biblioteca_musical.db`.
- En caso contrario: `~/.local/share/reproductorMP3/biblioteca_musical.db`.

Las imágenes personalizadas se guardan también en el directorio de datos de la aplicación, separadas de los archivos MP3:

- `imagenes/canciones/`: imágenes personalizadas de canciones.
- `imagenes/interpretes/`: imágenes personalizadas de intérpretes.

El esquema SQLite se encuentra en `src/baseDatos/rolas.sql`. La aplicación no modifica ese esquema al utilizarse normalmente.

## Estado de compatibilidad

Entorno de referencia utilizado durante las pruebas:

- Linux Mint.
- Arquitectura x86_64.
- Compilador GCC con soporte C++17.
- Qt 6.12.0 (Widgets, Multimedia y Concurrent).
- Backend FFmpeg 9.0.1 de Qt Multimedia.
- Meson y Ninja.
- TagLib y SQLite3.

La compatibilidad con otras distribuciones, versiones de Qt o versiones del backend multimedia debe verificarse en el entorno correspondiente. Para evitar conflictos, se recomienda que los módulos de Qt utilizados durante la compilación y los complementos/bibliotecas cargados durante la ejecución pertenezcan a la misma instalación de Qt.
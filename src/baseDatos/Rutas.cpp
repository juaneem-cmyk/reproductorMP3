#include "Rutas.h"
#include <cstdlib>
#include <system_error>
#ifndef REPRODUCTORMP3_SOURCE_DIR
#error "Meson debe definir REPRODUCTORMP3_SOURCE_DIR"
#endif

/* Conocer las rutas de la aplicación y preparar el directorio de datos */
namespace fs = std::filesystem;
fs::path Rutas::rutaBaseDatos() {
    const char* directorioXDG = std::getenv("XDG_DATA_HOME");

    // Si la variable de entorno XDG_DATA_HOME está definida y no está vacía, se utiliza como directorio base para la base de datos
    if (directorioXDG != nullptr && directorioXDG[0] != '\0') {
        return fs::path(directorioXDG) /"reproductorMP3"/"biblioteca_musical.db";
    }
    const char* directorioHome = std::getenv("HOME");

    // Si la variable de entorno HOME está definida y no está vacía, se utiliza como directorio base para la base de datos
    if (directorioHome != nullptr && directorioHome[0] != '\0') {
        return fs::path(directorioHome) /".local"/"share"/ "reproductorMP3"/ "biblioteca_musical.db";
    }
    return {};
}

// Obtener la ruta del archivo de esquema SQL
fs::path Rutas::rutaEsquemaSQL() {
    return fs::path(REPRODUCTORMP3_SOURCE_DIR) / "src"/ "baseDatos"/ "rolas.sql";
}

// Obtener el directorio donde se almacenan los datos de la aplicación
fs::path Rutas::rutaDirectorioDatos() {
    return rutaBaseDatos().parent_path();
}

// Obtener la ruta de la imagen personalizada de una canción
fs::path Rutas::rutaImagenCancion(int idRola) {
    if (idRola <= 0) {
        return {};
    }
    return rutaDirectorioDatos() / "imagenes" / "canciones" / (std::to_string(idRola) + ".png");
}

// Obtener la ruta de la imagen personalizada de un intérprete
fs::path Rutas::rutaImagenInterprete(int idInterprete) {
    if (idInterprete <= 0) {
        return {};
    }
    return rutaDirectorioDatos() / "imagenes" / "interpretes" / (std::to_string(idInterprete) + ".png");
}

// Preparar el directorio de datos de la aplicación
bool Rutas::prepararDirectorioDatos(std::string& mensajeError) {
    const fs::path archivoBaseDatos = rutaBaseDatos();

    // Verificar si se pudo determinar la ruta del archivo de base de datos
    if (archivoBaseDatos.empty()) {
        mensajeError = "No se pudo determinar el directorio de datos de la aplicación.";
        return false;
    }
    std::error_code error;
    fs::create_directories(archivoBaseDatos.parent_path(), error);

    // Verificar si hubo un error al crear el directorio de datos
    if (error) {
        mensajeError = "No se pudo crear el directorio de datos: " + error.message();
        return false;
    }
    mensajeError.clear();
    return true;
}

// Preparar los directorios donde se guardan las imágenes personalizadas
bool Rutas::prepararDirectoriosImagenes(std::string& mensajeError) {
    const fs::path directorioDatos = rutaDirectorioDatos();

    if (directorioDatos.empty()) {
        mensajeError = "No se pudo determinar el directorio de datos de la aplicación.";
        return false;
    }
    std::error_code error;
    fs::create_directories(directorioDatos / "imagenes" / "canciones", error);

    if (error) {
        mensajeError = "No se pudo crear el directorio de imágenes de canciones: " + error.message();
        return false;
    }
    fs::create_directories(directorioDatos / "imagenes" / "interpretes", error);

    if (error) {
        mensajeError = "No se pudo crear el directorio de imágenes de intérpretes: " + error.message();
        return false;
    }
    mensajeError.clear();
    return true;
}
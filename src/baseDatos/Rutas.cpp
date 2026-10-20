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


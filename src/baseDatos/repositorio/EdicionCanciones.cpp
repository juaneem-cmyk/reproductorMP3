#include "EdicionCanciones.h"
#include "RepositorioMusical.h"

#include <memory>
#include <sqlite3.h>

// Recibir una conexión SQLite que ya existe y utilizarla para realizar operaciones de edición en la base de datos musical
EdicionCanciones::EdicionCanciones(ConexionSQLite& conexion) : conexion(conexion) {
}

bool EdicionCanciones::actualizarCancion(int idRola, const std::string& titulo, const std::string& artista,
    const std::string& album, std::string& mensajeError) const {
    mensajeError.clear();

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }

    if (idRola <= 0) {
        mensajeError = "El identificador de la canción no es válido.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();

    // Iniciar una transacción para mantener consistentes los cambios
    if (!conexion.ejecutarSQL("BEGIN IMMEDIATE;", mensajeError)) {
        return false;
    }

    // Deshacer los cambios si ocurre algún error
    auto cancelarTransaccion = [&]() {
        std::string errorRollback;
        conexion.ejecutarSQL("ROLLBACK;", errorRollback);
    };
    int idInterpreteActual = 0;
    int idAlbumActual = 0;
    int anio = 0;
    bool tieneAlbumActual = false;
    bool encontrada = false;

    std::string ruta;
    std::string artistaActual;
    std::string albumActual;

    mensajeError.clear();
    return true;
}
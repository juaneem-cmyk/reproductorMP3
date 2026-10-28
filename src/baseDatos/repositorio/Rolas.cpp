#include "Rolas.h"
#include <memory>
#include <sqlite3.h>

// Utilizar una conexión SQLite existente
Rolas::Rolas(ConexionSQLite& conexion) : conexion(conexion) {
}

bool Rolas::obtenerCanciones(std::vector<CancionBiblioteca>& canciones, std::string& mensajeError) const {
    canciones.clear();

    // Comprobar que exista una conexión abierta
    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();

    // Consultar canciones, intérpretes y álbumes asociados
    const char* consulta =
    "SELECT r.id_rola, r.title, p.name, a.name, "
    "r.path, r.track, r.year, r.genre "
    "FROM rolas AS r "
    "LEFT JOIN performers AS p "
    "ON r.id_performer = p.id_performer "
    "LEFT JOIN albums AS a "
    "ON r.id_album = a.id_album "
    "ORDER BY COALESCE(a.name, ''), r.track, r.title;";

    mensajeError.clear();
    return true;
}
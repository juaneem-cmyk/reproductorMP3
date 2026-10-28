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

    sqlite3_stmt* temporal = nullptr;
    int codigo = sqlite3_prepare_v2(baseDatos, consulta, -1, &temporal, nullptr);

    // Comprobar si la consulta pudo prepararse
    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);

        if (temporal != nullptr) {
            sqlite3_finalize(temporal);
        }
        return false;
    }
    // Liberar automáticamente la sentencia preparada
    std::unique_ptr <sqlite3_stmt, decltype(&sqlite3_finalize)> sentencia(temporal, &sqlite3_finalize);

    // Obtener un texto sin asumir que la columna contiene datos
    auto obtenerTexto = [&sentencia](int columna) -> std::string {
        const unsigned char* texto = sqlite3_column_text(sentencia.get(), columna);

        if (texto == nullptr) {
            return "";
        }
        return reinterpret_cast<const char*>(texto);
    };

    // Recorrer las canciones obtenidas
    while ((codigo = sqlite3_step(sentencia.get())) == SQLITE_ROW) {
        CancionBiblioteca cancion;
        cancion.idRola = sqlite3_column_int(sentencia.get(), 0);
        cancion.titulo = obtenerTexto(1);
        cancion.artista = obtenerTexto(2);
        cancion.album = obtenerTexto(3);
        cancion.ruta = obtenerTexto(4);
        cancion.pista = sqlite3_column_int(sentencia.get(), 5);
        cancion.anio = sqlite3_column_int(sentencia.get(), 6);
        cancion.genero = obtenerTexto(7);
        canciones.push_back(cancion);
    }

    // Comprobar que la consulta haya terminado correctamente
    if (codigo != SQLITE_DONE) {
        mensajeError = sqlite3_errmsg(baseDatos);
        canciones.clear();
        return false;
    }
    mensajeError.clear();
    return true;
}
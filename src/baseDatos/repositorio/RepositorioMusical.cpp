#include "RepositorioMusical.h"

#include <memory>
#include <sqlite3.h>

// Recibe una conexión SQLite que ya existe y la utiliza para realizar operaciones en la base de datos musical
RepositorioMusical::RepositorioMusical(ConexionSQLite& conexion) : conexion(conexion) {
}

bool RepositorioMusical::existeCancionPorRuta(const std::string& ruta, bool& existe, std::string& mensajeError) const {
    existe = false;

    // Comprobar que exista una conexión abierta
    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();
    sqlite3_stmt* sentenciaTemporal = nullptr;
    const char* consulta = "SELECT id_rola FROM rolas WHERE path = ? LIMIT 1;";

    // Preparar la consulta parametrizada
    int codigo = sqlite3_prepare_v2(baseDatos, consulta, -1, &sentenciaTemporal, nullptr
    );

    // Comprobar si la preparación de la sentencia fue exitosa
    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);

        if (sentenciaTemporal != nullptr) {
            sqlite3_finalize(sentenciaTemporal);
        }
        return false;
    }
    // La sentencia se libera automáticamente al salir del método
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> sentencia(sentenciaTemporal, &sqlite3_finalize);
    // Asociar la ruta al parámetro ?
    codigo = sqlite3_bind_text(sentencia.get(), 1, ruta.c_str(), -1, SQLITE_TRANSIENT
    );

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    // Ejecutar la consulta y determinar si se encontró una canción
    codigo = sqlite3_step(sentencia.get());

    // Comprobar el resultado de la ejecución de la consulta
    if (codigo == SQLITE_ROW) {
        existe = true;
        mensajeError.clear();
        return true;
    }

    // Si no se encontró ninguna fila, significa que la canción no existe
    if (codigo == SQLITE_DONE) {
        existe = false;
        mensajeError.clear();
        return true;
    }
    mensajeError = sqlite3_errmsg(baseDatos);
    return false;
}

bool RepositorioMusical::obtenerOCrearInterprete(const std::string& nombre, int& idInterprete, std::string& mensajeError) const {
    idInterprete = 0;

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();

    // Si el MP3 no tiene artista, usamos un intérprete desconocido
    const std::string nombreInterprete = nombre.empty() ? "Unknown" : nombre;
    // Buscar el intérprete antes de insertar uno nuevo
    const char* consulta = "SELECT id_performer " "FROM performers " "WHERE name = ? " "ORDER BY id_performer " "LIMIT 1;";
    sqlite3_stmt* temporal = nullptr;

    int codigo = sqlite3_prepare_v2(baseDatos, consulta, -1, &temporal, nullptr);

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);

        if (temporal != nullptr) {
            sqlite3_finalize(temporal);
        }
        return false;
    }
    idInterprete = static_cast<int>(sqlite3_last_insert_rowid(baseDatos));
    mensajeError.clear();
    return true;
}
#include "EdicionInterp.h"
#include <memory>
#include <sqlite3.h>

// Recibir una conexión SQLite existente para editar intérpretes
EdicionInterpretes::EdicionInterpretes(ConexionSQLite& conexion) : conexion(conexion) {
}

bool EdicionInterpretes::actualizarInterprete(int idInterprete, const std::string& nombre,
    int idTipo, std::string& mensajeError) const {
    mensajeError.clear();

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }

    if (idInterprete <= 0) {
        mensajeError = "El identificador del intérprete no es válido.";
        return false;
    }

    if (nombre.find_first_not_of(" \t\n\r") == std::string::npos) {
        mensajeError = "El nombre del intérprete no puede estar vacío.";
        return false;
    }

    if (idTipo < 0 || idTipo > 2) {
        mensajeError = "El tipo de intérprete no es válido.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();

    // Actualizar el nombre y tipo del intérprete seleccionado
    const char* actualizacion =
        "UPDATE performers "
        "SET name = ?, id_type = ? "
        "WHERE id_performer = ?;";
    sqlite3_stmt* temporal = nullptr;
    int codigo = sqlite3_prepare_v2(baseDatos, actualizacion, -1, &temporal, nullptr);

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);

        if (temporal != nullptr) {
            sqlite3_finalize(temporal);
        }
        return false;
    }
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> sentencia(temporal, &sqlite3_finalize);
    codigo = sqlite3_bind_text(sentencia.get(), 1, nombre.c_str(), -1, SQLITE_TRANSIENT);

    if (codigo == SQLITE_OK) {
        codigo = sqlite3_bind_int(sentencia.get(), 2, idTipo);
    }

    if (codigo == SQLITE_OK) {
        codigo = sqlite3_bind_int(sentencia.get(), 3, idInterprete);
    }

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    codigo = sqlite3_step(sentencia.get());

    if (codigo != SQLITE_DONE) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    mensajeError.clear();
    return true;
}
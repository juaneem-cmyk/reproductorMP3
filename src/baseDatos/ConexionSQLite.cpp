#include "ConexionSQLite.h"

// Implementación del destructor de CerradorSQLite
void ConexionSQLite::CerradorSQLite::operator()(sqlite3* conexion) const noexcept {
    if (conexion != nullptr) {
        sqlite3_close_v2(conexion);
    }
}

bool ConexionSQLite::abrir(const std::string& rutaArchivo, std::string& mensajeError) {
    // Verificar si la conexión ya está abierta
    if (conexion != nullptr) {
        mensajeError = "La conexión con SQLite ya está abierta.";
        return false;
    }
    sqlite3* manejador = nullptr;
    const int codigo = sqlite3_open(rutaArchivo.c_str(), &manejador);

    // Verificar si hubo un error al abrir la base de datos
    if (codigo != SQLITE_OK) {
        if (manejador != nullptr) {
            mensajeError = sqlite3_errmsg(manejador);
            sqlite3_close_v2(manejador);
        } else {
            mensajeError = "No se pudo abrir la base de datos.";
        }
        return false;
    }
    conexion.reset(manejador);
    mensajeError.clear();
    return true;
}

bool ConexionSQLite::ejecutarSQL(const std::string& sentencia, std::string& mensajeError) {
    // Verificar si la conexión está abierta
    if (conexion == nullptr) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }
    char* errorSQLite = nullptr;
    const int codigo = sqlite3_exec(conexion.get(), sentencia.c_str(), nullptr, nullptr, &errorSQLite);

    // Verificar si hubo un error al ejecutar la sentencia SQL
    if (codigo != SQLITE_OK) {
        if (errorSQLite != nullptr) {
            mensajeError = errorSQLite;
            sqlite3_free(errorSQLite);
        } else {
            mensajeError = sqlite3_errmsg(conexion.get());
        }
        return false;
    }
    mensajeError.clear();
    return true;
}

bool ConexionSQLite::estaAbierta() const noexcept {
    return conexion != nullptr;
}

sqlite3* ConexionSQLite::obtenerManejador() const noexcept {
    return conexion.get();
}
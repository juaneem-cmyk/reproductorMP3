#include "EdicionInterp.h"
#include <memory>
#include <sqlite3.h>

// Funciones auxiliares para preparar y vincular sentencias SQLite
namespace {
using SentenciaSQLite = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

bool preparar(sqlite3* baseDatos, const char* consulta, SentenciaSQLite& sentencia, std::string& mensajeError) {
    sqlite3_stmt* temporal = nullptr;
    const int codigo = sqlite3_prepare_v2(baseDatos, consulta, -1, &temporal, nullptr);

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        if (temporal != nullptr) {
            sqlite3_finalize(temporal);
        }
        return false;
    }
    sentencia.reset(temporal);
    return true;
}

// Vincula un valor de texto a una posición específica en la sentencia SQLite
bool vincularTexto(sqlite3* baseDatos, sqlite3_stmt* sentencia, int posicion, const std::string& valor, 
    std::string& mensajeError) {
    const int codigo = valor.empty() ? sqlite3_bind_null(sentencia, posicion)
    : sqlite3_bind_text(sentencia, posicion, valor.c_str(), -1, SQLITE_TRANSIENT);

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    return true;
}
}

EdicionInterpretes::EdicionInterpretes(ConexionSQLite& conexion) : conexion(conexion) {
}

// Implementación de los métodos obtenerPersona y actualizarInterprete
bool EdicionInterpretes::obtenerPersona(int idInterprete, DatosPersona& datos, std::string& mensajeError) const {
    mensajeError.clear();
    datos = {};

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }
    if (idInterprete <= 0) {
        mensajeError = "El identificador del intérprete no es válido.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();
    SentenciaSQLite sentencia(nullptr, &sqlite3_finalize);
    const char* consulta =
        "SELECT real_name, birth_date, death_date "
        "FROM persons WHERE id_person = ?;";

    if (!preparar(baseDatos, consulta, sentencia, mensajeError)) {
        return false;
    }

    int codigo = sqlite3_bind_int(sentencia.get(), 1, idInterprete);
    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    codigo = sqlite3_step(sentencia.get());

    if (codigo == SQLITE_DONE) {
        return true;
    }
    if (codigo != SQLITE_ROW) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }

    const auto leerTexto = [sentencia = sentencia.get()](int columna) {
        const unsigned char* texto = sqlite3_column_text(sentencia, columna);
        return texto == nullptr ? std::string() : std::string(reinterpret_cast<const char*>(texto));
    };

    datos.nombreReal = leerTexto(0);
    datos.fechaNacimiento = leerTexto(1);
    datos.fechaFallecimiento = leerTexto(2);
    mensajeError.clear();
    return true;
}

// Actualiza la información de un intérprete en la base de datos
bool EdicionInterpretes::actualizarInterprete(int idInterprete, const std::string& nombre, 
    int idTipo, const DatosPersona& datosPersona, std::string& mensajeError) const {
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
    if (sqlite3_exec(baseDatos, "BEGIN IMMEDIATE;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }

    const auto revertir = [&baseDatos, &mensajeError]() {
        sqlite3_exec(baseDatos, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    };

    // El nombre y la clasificación general permanecen en performers
    SentenciaSQLite actualizar(nullptr, &sqlite3_finalize);
    const char* actualizacion =
        "UPDATE performers SET name = ?, id_type = ? "
        "WHERE id_performer = ?;";

    if (!preparar(baseDatos, actualizacion, actualizar, mensajeError)) {
        return revertir();
    }
    int codigo = sqlite3_bind_text(actualizar.get(), 1, nombre.c_str(), -1, SQLITE_TRANSIENT);
    if (codigo == SQLITE_OK) codigo = sqlite3_bind_int(actualizar.get(), 2, idTipo);
    if (codigo == SQLITE_OK) codigo = sqlite3_bind_int(actualizar.get(), 3, idInterprete);
    if (codigo != SQLITE_OK || sqlite3_step(actualizar.get()) != SQLITE_DONE) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return revertir();
    }
    if (sqlite3_changes(baseDatos) == 0) {
        mensajeError = "No se encontró el intérprete que se desea actualizar.";
        return revertir();
    }

    // Los datos existentes se conservan si el intérprete cambia a otro tipo
    if (idTipo == 0) {
        SentenciaSQLite guardarPersona(nullptr, &sqlite3_finalize);
        const char* insercion =
            "INSERT INTO persons (id_person, stage_name, real_name, birth_date, death_date) "
            "VALUES (?, ?, ?, ?, ?) "
            "ON CONFLICT(id_person) DO UPDATE SET "
            "stage_name = excluded.stage_name, "
            "real_name = excluded.real_name, "
            "birth_date = excluded.birth_date, "
            "death_date = excluded.death_date;";

        if (!preparar(baseDatos, insercion, guardarPersona, mensajeError)) {
            return revertir();
        }
        codigo = sqlite3_bind_int(guardarPersona.get(), 1, idInterprete);

        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_text(guardarPersona.get(), 2, nombre.c_str(), -1, SQLITE_TRANSIENT);
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarPersona.get(), 3, datosPersona.nombreReal, 
            mensajeError) ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarPersona.get(), 4, datosPersona.fechaNacimiento, 
            mensajeError) ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarPersona.get(), 5, datosPersona.fechaFallecimiento, 
            mensajeError) ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo != SQLITE_OK) {
            if (mensajeError.empty()) mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }

        if (sqlite3_step(guardarPersona.get()) != SQLITE_DONE) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }
    }

    if (sqlite3_exec(baseDatos, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return revertir();
    }
    mensajeError.clear();
    return true;
}
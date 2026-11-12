#include "CrearInterp.h"
#include <memory>
#include <sqlite3.h>

namespace {
using SentenciaSQLite = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

bool preparar(sqlite3* baseDatos, const char* consulta,
    SentenciaSQLite& sentencia, std::string& mensajeError) {
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

// Función auxiliar para vincular un valor de texto a una sentencia SQL y manejar errores
bool vincularTexto(sqlite3* baseDatos, sqlite3_stmt* sentencia, int posicion,
    const std::string& valor, std::string& mensajeError) {
    const int codigo = valor.empty() ? sqlite3_bind_null(sentencia, posicion)
        : sqlite3_bind_text(sentencia, posicion, valor.c_str(), -1, SQLITE_TRANSIENT);

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    return true;
}
}

CrearInterpretes::CrearInterpretes(ConexionSQLite& conexion) : conexion(conexion) {
}

// Registrar una persona o grupo independiente de cualquier canción
bool CrearInterpretes::crearInterprete(const std::string& nombre, int idTipo,
    const DatosPersona& datosPersona, const DatosGrupo& datosGrupo,
    const std::vector<int>& integrantes, int& idInterprete, std::string& mensajeError) const {
    mensajeError.clear();
    idInterprete = 0;

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }

    const std::string nombreLimpio = [&nombre]() {
        const auto primero = nombre.find_first_not_of(" \t\n\r");
        if (primero == std::string::npos) {
            return std::string();
        }
        const auto ultimo = nombre.find_last_not_of(" \t\n\r");
        return nombre.substr(primero, ultimo - primero + 1);
    }();

    if (nombreLimpio.empty()) {
        mensajeError = "El nombre del intérprete no puede estar vacío.";
        return false;
    }

    // Los registros independientes deben ser personas o grupos, no el Unknown genérico
    if (idTipo != 0 && idTipo != 1) {
        mensajeError = "El tipo de intérprete debe ser Persona o Grupo.";
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

    // Evitar nombres duplicados entre tipos
    {
        SentenciaSQLite consultar(nullptr, &sqlite3_finalize);
        const char* consulta =
            "SELECT id_performer FROM performers "
            "WHERE lower(trim(name)) = lower(trim(?)) "
            "LIMIT 1;";

        if (!preparar(baseDatos, consulta, consultar, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_text(consultar.get(), 1, nombreLimpio.c_str(), -1, SQLITE_TRANSIENT);

        if (codigo != SQLITE_OK) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }
        codigo = sqlite3_step(consultar.get());

        if (codigo == SQLITE_ROW) {
            mensajeError = "Ya existe un intérprete con ese nombre. No se creó un registro duplicado; "
                "puedes asignar el existente desde el campo Artista al editar una canción.";
            return revertir();
        }

        if (codigo != SQLITE_DONE) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }
    }
}
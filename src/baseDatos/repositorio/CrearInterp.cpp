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

    // Crear el registro común del intérprete
    {
        SentenciaSQLite insertar(nullptr, &sqlite3_finalize);
        const char* insercion = "INSERT INTO performers (id_type, name) VALUES (?, ?);";
        if (!preparar(baseDatos, insercion, insertar, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_int(insertar.get(), 1, idTipo);
        
        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_text(insertar.get(), 2, nombreLimpio.c_str(), -1, SQLITE_TRANSIENT);
        }

        if (codigo != SQLITE_OK || sqlite3_step(insertar.get()) != SQLITE_DONE) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }
        idInterprete = static_cast<int>(sqlite3_last_insert_rowid(baseDatos));
    }

    // Crear la ficha específica aunque todavía no existan canciones o integrantes
    if (idTipo == 0) {
        SentenciaSQLite insertar(nullptr, &sqlite3_finalize);
        const char* insercion =
            "INSERT INTO persons (id_person, stage_name, real_name, birth_date, death_date) "
            "VALUES (?, ?, ?, ?, ?);";

        if (!preparar(baseDatos, insercion, insertar, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_int(insertar.get(), 1, idInterprete);

        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_text(insertar.get(), 2, nombreLimpio.c_str(), -1, SQLITE_TRANSIENT);
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, insertar.get(), 3, datosPersona.nombreReal, mensajeError)
                ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, insertar.get(), 4, datosPersona.fechaNacimiento, mensajeError)
                ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, insertar.get(), 5, datosPersona.fechaFallecimiento, mensajeError)
                ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo != SQLITE_OK || sqlite3_step(insertar.get()) != SQLITE_DONE) {
            if (mensajeError.empty()) {
                mensajeError = sqlite3_errmsg(baseDatos);
            }
            return revertir();
        }
    } else {
        SentenciaSQLite insertar(nullptr, &sqlite3_finalize);
        const char* insercion =
            "INSERT INTO groups (id_group, name, start_date, end_date) VALUES (?, ?, ?, ?);";

        if (!preparar(baseDatos, insercion, insertar, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_int(insertar.get(), 1, idInterprete);

        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_text(insertar.get(), 2, nombreLimpio.c_str(), -1, SQLITE_TRANSIENT);
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, insertar.get(), 3, datosGrupo.fechaCreacion, mensajeError)
                ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, insertar.get(), 4, datosGrupo.fechaSeparacion, mensajeError)
                ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo != SQLITE_OK || sqlite3_step(insertar.get()) != SQLITE_DONE) {
            if (mensajeError.empty()) {
                mensajeError = sqlite3_errmsg(baseDatos);
            }
            return revertir();
        }

        // Vincular solo personas que continúen clasificadas como tales
        SentenciaSQLite agregarIntegrante(nullptr, &sqlite3_finalize);
        const char* insercionIntegrante =
            "INSERT INTO in_group (id_person, id_group) "
            "SELECT pe.id_person, ? FROM persons AS pe "
            "INNER JOIN performers AS p ON p.id_performer = pe.id_person "
            "WHERE pe.id_person = ? AND p.id_type = 0;";

        if (!preparar(baseDatos, insercionIntegrante, agregarIntegrante, mensajeError)) {
            return revertir();
        }

        // Iterar sobre los integrantes y agregarlos al grupo
        for (int idPersona : integrantes) {
            sqlite3_reset(agregarIntegrante.get());
            sqlite3_clear_bindings(agregarIntegrante.get());
            codigo = sqlite3_bind_int(agregarIntegrante.get(), 1, idInterprete);

            if (codigo == SQLITE_OK) {
                codigo = sqlite3_bind_int(agregarIntegrante.get(), 2, idPersona);
            }

            if (codigo != SQLITE_OK || sqlite3_step(agregarIntegrante.get()) != SQLITE_DONE) {
                mensajeError = sqlite3_errmsg(baseDatos);
                return revertir();
            }

            if (sqlite3_changes(baseDatos) != 1) {
                mensajeError = "Uno de los integrantes seleccionados ya no está registrado como persona.";
                return revertir();
            }
        }
    }

    if (sqlite3_exec(baseDatos, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return revertir();
    }
    mensajeError.clear();
    return true;
}
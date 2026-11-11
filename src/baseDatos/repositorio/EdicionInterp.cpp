#include "EdicionInterp.h"
#include <algorithm>
#include <cctype>
#include <memory>
#include <sqlite3.h>

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

std::string normalizarNombre(const std::string& nombre) {
    auto primero = nombre.begin();
    while (primero != nombre.end() && std::isspace(static_cast<unsigned char>(*primero))) {
        ++primero;
    }

    auto ultimo = nombre.end();
    while (ultimo != primero && std::isspace(static_cast<unsigned char>(*(ultimo - 1)))) {
        --ultimo;
    }

    std::string resultado(primero, ultimo);
    std::transform(resultado.begin(), resultado.end(), resultado.begin(), [](unsigned char caracter) {
            return static_cast<char>(std::tolower(caracter));
        });
    return resultado;
}

bool esDesconocidoGenerico(int idTipo, const std::string& nombre) {
    return idTipo == 2 && normalizarNombre(nombre) == "unknown";
}
}

EdicionInterpretes::EdicionInterpretes(ConexionSQLite& conexion) : conexion(conexion) {
}

// Función para obtener los datos de una persona o solista a partir de su identificador
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
        "SELECT pe.real_name, pe.birth_date, pe.death_date "
        "FROM persons AS pe "
        "INNER JOIN performers AS p ON p.id_performer = pe.id_person "
        "WHERE pe.id_person = ? "
        "AND NOT (p.id_type = 2 AND lower(trim(p.name)) = 'unknown');";

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

// Función para obtener los datos de un grupo a partir de su identificador
bool EdicionInterpretes::obtenerGrupo(int idInterprete, DatosGrupo& datos, std::string& mensajeError) const {
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
        "SELECT g.start_date, g.end_date "
        "FROM groups AS g "
        "WHERE g.id_group = ?;";

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

    datos.fechaCreacion = leerTexto(0);
    datos.fechaSeparacion = leerTexto(1);
    mensajeError.clear();
    return true;
}

bool EdicionInterpretes::actualizarInterprete(int idInterprete, int idRola,
    const std::string& nombre, int idTipo, const DatosPersona& datosPersona,
    const DatosGrupo& datosGrupo, int& idInterpreteActualizado, std::string& mensajeError) const {
    mensajeError.clear();
    idInterpreteActualizado = 0;

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }
    
    if (idInterprete <= 0 || idRola <= 0) {
        mensajeError = "El identificador del intérprete o de la canción no es válido.";
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

    // Recuperar el intérprete real de la canción para evitar editar una selección obsoleta
    int idAsociado = 0;
    int tipoActual = 2;
    std::string nombreActual;
    {
        SentenciaSQLite consultar(nullptr, &sqlite3_finalize);
        const char* consulta =
            "SELECT p.id_performer, p.name, p.id_type "
            "FROM rolas AS r "
            "INNER JOIN performers AS p ON p.id_performer = r.id_performer "
            "WHERE r.id_rola = ?;";

        if (!preparar(baseDatos, consulta, consultar, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_int(consultar.get(), 1, idRola);

        if (codigo != SQLITE_OK) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }
        codigo = sqlite3_step(consultar.get());

        if (codigo == SQLITE_DONE) {
            mensajeError = "No se encontró la canción que se desea editar.";
            return revertir();
        }

        if (codigo != SQLITE_ROW) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }
        idAsociado = sqlite3_column_int(consultar.get(), 0);
        const unsigned char* nombre = sqlite3_column_text(consultar.get(), 1);
        nombreActual = nombre == nullptr ? std::string() : std::string(reinterpret_cast<const char*>(nombre));
        tipoActual = sqlite3_column_int(consultar.get(), 2);
    }

    if (idAsociado != idInterprete) {
        mensajeError = "La canción ya no está asociada con el intérprete seleccionado. "
            "Actualiza la biblioteca e inténtalo nuevamente.";
        return revertir();
    }

    // El registro genérico Unknown puede representar artistas distintos,
    // al personalizarlo, se crea otro performer y solo se reasigna esta canción
    const bool separarDesconocido = esDesconocidoGenerico(tipoActual, nombreActual)
        && (nombre != nombreActual || idTipo != tipoActual);

    if (separarDesconocido) {
        SentenciaSQLite insertar(nullptr, &sqlite3_finalize);
        const char* insercion =
            "INSERT INTO performers (id_type, name) VALUES (?, ?);";

        if (!preparar(baseDatos, insercion, insertar, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_int(insertar.get(), 1, idTipo);
        
        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_text(insertar.get(), 2, nombre.c_str(), -1, SQLITE_TRANSIENT);
        }

        if (codigo != SQLITE_OK || sqlite3_step(insertar.get()) != SQLITE_DONE) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }
        idInterpreteActualizado = static_cast<int>(sqlite3_last_insert_rowid(baseDatos));

        SentenciaSQLite reasignar(nullptr, &sqlite3_finalize);
        const char* actualizacion =
            "UPDATE rolas SET id_performer = ? "
            "WHERE id_rola = ? AND id_performer = ?;";

        if (!preparar(baseDatos, actualizacion, reasignar, mensajeError)) {
            return revertir();
        }
        codigo = sqlite3_bind_int(reasignar.get(), 1, idInterpreteActualizado);

        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_int(reasignar.get(), 2, idRola);
        }

        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_int(reasignar.get(), 3, idInterprete);
        }

        if (codigo != SQLITE_OK) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }

        if (sqlite3_step(reasignar.get()) != SQLITE_DONE) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }

        if (sqlite3_changes(baseDatos) != 1) {
            mensajeError = "No se pudo separar la canción del intérprete Unknown.";
            return revertir();
        }
    } else {
        // Para intérpretes identificados, el cambio se aplica a todas sus canciones
        SentenciaSQLite actualizar(nullptr, &sqlite3_finalize);
        const char* actualizacion =
            "UPDATE performers SET name = ?, id_type = ? "
            "WHERE id_performer = ?;";
        if (!preparar(baseDatos, actualizacion, actualizar, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_text(actualizar.get(), 1, nombre.c_str(), -1, SQLITE_TRANSIENT);

        if (codigo == SQLITE_OK) 
        {
            codigo = sqlite3_bind_int(actualizar.get(), 2, idTipo);
        }

        if (codigo == SQLITE_OK) 
        {
            codigo = sqlite3_bind_int(actualizar.get(), 3, idInterprete);
        }

        if (codigo != SQLITE_OK || sqlite3_step(actualizar.get()) != SQLITE_DONE) {
            mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }

        if (sqlite3_changes(baseDatos) == 0) {
            mensajeError = "No se encontró el intérprete que se desea actualizar.";
            return revertir();
        }
        idInterpreteActualizado = idInterprete;
    }

    // En el caso Unknown se usa el nuevo identificador, no el registro compartido
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
        int codigo = sqlite3_bind_int(guardarPersona.get(), 1, idInterpreteActualizado);

        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_text(guardarPersona.get(), 2, nombre.c_str(), -1, SQLITE_TRANSIENT);
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarPersona.get(), 3,
                datosPersona.nombreReal, mensajeError) ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarPersona.get(), 4,
                datosPersona.fechaNacimiento, mensajeError) ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarPersona.get(), 5,
                datosPersona.fechaFallecimiento, mensajeError) ? SQLITE_OK : SQLITE_ERROR;
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

    // Los datos del grupo utilizan el mismo identificador que performers
    if (idTipo == 1) {
        SentenciaSQLite guardarGrupo(nullptr, &sqlite3_finalize);
        const char* insercion =
            "INSERT INTO groups (id_group, name, start_date, end_date) "
            "VALUES (?, ?, ?, ?) "
            "ON CONFLICT(id_group) DO UPDATE SET "
            "name = excluded.name, "
            "start_date = excluded.start_date, "
            "end_date = excluded.end_date;";

        if (!preparar(baseDatos, insercion, guardarGrupo, mensajeError)) {
            return revertir();
        }
        int codigo = sqlite3_bind_int(guardarGrupo.get(), 1, idInterpreteActualizado);

        if (codigo == SQLITE_OK) {
            codigo = sqlite3_bind_text(guardarGrupo.get(), 2, nombre.c_str(), -1, SQLITE_TRANSIENT);
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarGrupo.get(), 3,
                datosGrupo.fechaCreacion, mensajeError) ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo == SQLITE_OK) {
            codigo = vincularTexto(baseDatos, guardarGrupo.get(), 4,
                datosGrupo.fechaSeparacion, mensajeError) ? SQLITE_OK : SQLITE_ERROR;
        }

        if (codigo != SQLITE_OK) {
            if (mensajeError.empty()) mensajeError = sqlite3_errmsg(baseDatos);
            return revertir();
        }

        if (sqlite3_step(guardarGrupo.get()) != SQLITE_DONE) {
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
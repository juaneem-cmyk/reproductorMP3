#include "EdicionAlbumes.h"
#include <memory>
#include <sqlite3.h>
#include <utility>

namespace {
using SentenciaSQLite = std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)>;

// Función auxiliar para preparar una sentencia SQL y manejar errores
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

std::string limpiarNombre(const std::string& nombre) {
    const auto primero = nombre.find_first_not_of(" \t\n\r");
    if (primero == std::string::npos) {
        return {};
    }
    const auto ultimo = nombre.find_last_not_of(" \t\n\r");
    return nombre.substr(primero, ultimo - primero + 1);
}
}

EdicionAlbumes::EdicionAlbumes(ConexionSQLite& conexion) : conexion(conexion) {
}

// Función para obtener todos los álbumes de la base de datos
bool EdicionAlbumes::obtenerAlbumes(std::vector<DatosAlbum>& albumes, std::string& mensajeError) const {
    mensajeError.clear();
    albumes.clear();

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();
    SentenciaSQLite sentencia(nullptr, &sqlite3_finalize);

    const char* consulta =
        "SELECT a.id_album, COALESCE(a.name, ''), COALESCE(a.path, ''), "
        "COALESCE(a.year, 0), COUNT(r.id_rola) "
        "FROM albums AS a "
        "LEFT JOIN rolas AS r ON r.id_album = a.id_album "
        "WHERE a.id_album > 0 "
        "GROUP BY a.id_album, a.name, a.path, a.year "
        "ORDER BY a.name COLLATE NOCASE, a.id_album;";

    if (!preparar(baseDatos, consulta, sentencia, mensajeError)) {
        return false;
    }

    int codigo = SQLITE_OK;

    // Iterar sobre los resultados de la consulta y llenar el vector de álbumes
    while ((codigo = sqlite3_step(sentencia.get())) == SQLITE_ROW) {
        DatosAlbum album;
        album.idAlbum = sqlite3_column_int(sentencia.get(), 0);
        const unsigned char* nombre = sqlite3_column_text(sentencia.get(), 1);
        const unsigned char* ruta = sqlite3_column_text(sentencia.get(), 2);
        album.nombre = nombre == nullptr ? std::string() : std::string(reinterpret_cast<const char*>(nombre));
        album.ruta = ruta == nullptr ? std::string() : std::string(reinterpret_cast<const char*>(ruta));
        album.year = sqlite3_column_int(sentencia.get(), 3);
        album.cantidadCanciones = sqlite3_column_int(sentencia.get(), 4);
        albumes.push_back(std::move(album));
    }

    if (codigo != SQLITE_DONE) {
        mensajeError = sqlite3_errmsg(baseDatos);
        albumes.clear();
        return false;
    }
    return true;
}

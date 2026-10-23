#include "RepositorioMusical.h"

#include <memory>
#include <sqlite3.h>
#include <filesystem>
#include <system_error>

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
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> sentencia(temporal, &sqlite3_finalize);
    codigo = sqlite3_bind_text(sentencia.get(), 1, nombreInterprete.c_str(), -1, SQLITE_TRANSIENT);

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    codigo = sqlite3_step(sentencia.get());

    if (codigo == SQLITE_ROW) {
        idInterprete = sqlite3_column_int(sentencia.get(), 0);
        mensajeError.clear();
        return true;
    }

    if (codigo != SQLITE_DONE) {
        mensajeError = sqlite3_errmsg(baseDatos);
        return false;
    }
    // El intérprete no existe: registrarlo como Unknown
    sentencia.reset();
    const char* insercion = "INSERT INTO performers (id_type, name) VALUES (?, ?);";
    temporal = nullptr;
    codigo = sqlite3_prepare_v2(baseDatos, insercion, -1, &temporal, nullptr);

    if (codigo != SQLITE_OK) {
        mensajeError = sqlite3_errmsg(baseDatos);

        if (temporal != nullptr) {
            sqlite3_finalize(temporal);
        }
        return false;
    }
    sentencia.reset(temporal);
    codigo = sqlite3_bind_int(sentencia.get(), 1, 2);

    if (codigo == SQLITE_OK) {
        codigo = sqlite3_bind_text(sentencia.get(), 2, nombreInterprete.c_str(), -1, SQLITE_TRANSIENT);
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
    idInterprete = static_cast<int>(sqlite3_last_insert_rowid(baseDatos));
    mensajeError.clear();
    return true;
}

bool RepositorioMusical::obtenerOCrearAlbum(const std::string& rutaArchivo, const std::string& nombreAlbum, unsigned int year, int& idAlbum, bool& tieneAlbum, std::string& mensajeError) const {
    idAlbum = 0;
    tieneAlbum = false;

    // Comprobar que exista una conexión abierta
    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }

    // Si el MP3 no tiene nombre de álbum, se deja sin asignar
    if (nombreAlbum.empty()) {
        mensajeError.clear();
        return true;
    }

    // Obtener la ruta normalizada del archivo
    std::error_code errorRuta;
    const std::filesystem::path rutaNormalizada = std::filesystem::absolute(rutaArchivo, errorRuta).lexically_normal();

    if (errorRuta) {
        mensajeError = "No se pudo obtener la ruta del archivo: " + errorRuta.message();
        return false;
    }
    // Buscar un directorio cuyo nombre coincida con el metadato del álbum
    std::filesystem::path directorio = rutaNormalizada.parent_path();
    std::filesystem::path rutaDirectorioAlbum;

    // Recorrer los directorios ascendentes hasta encontrar una coincidencia o llegar a la raíz
    while (!directorio.empty()) {
        if (directorio.filename().string() == nombreAlbum) {
            rutaDirectorioAlbum = directorio;
            break;
        }
        const std::filesystem::path padre = directorio.parent_path();

        if (padre == directorio) {
            break;
        }
        directorio = padre;
    }

    // Si no existe coincidencia, la canción permanecerá sin álbum
    if (rutaDirectorioAlbum.empty()) {
        mensajeError.clear();
        return true;
    }
}
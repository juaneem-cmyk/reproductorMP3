#include "EdicionCanciones.h"
#include "RepositorioMusical.h"

#include <memory>
#include <sqlite3.h>

// Recibir una conexión SQLite que ya existe y utilizarla para realizar operaciones de edición en la base de datos musical
EdicionCanciones::EdicionCanciones(ConexionSQLite& conexion) : conexion(conexion) {
}

bool EdicionCanciones::actualizarCancion(int idRola, const std::string& titulo, const std::string& artista,
    const std::string& album, int year, std::string& mensajeError) const {
    mensajeError.clear();

    if (!conexion.estaAbierta()) {
        mensajeError = "No hay una conexión abierta con SQLite.";
        return false;
    }

    if (idRola <= 0) {
        mensajeError = "El identificador de la canción no es válido.";
        return false;
    }
    sqlite3* baseDatos = conexion.obtenerManejador();

    // Iniciar una transacción para mantener consistentes los cambios
    if (!conexion.ejecutarSQL("BEGIN IMMEDIATE;", mensajeError)) {
        return false;
    }

    // Deshacer los cambios si ocurre algún error
    auto cancelarTransaccion = [&]() {
        std::string errorRollback;
        conexion.ejecutarSQL("ROLLBACK;", errorRollback);
    };
    int idInterpreteActual = 0;
    int idAlbumActual = 0;
    bool tieneAlbumActual = false;
    bool encontrada = false;

    std::string ruta;
    std::string artistaActual;
    std::string albumActual;

    // Consultar los datos actuales de la canción
    {
        const char* consulta =
            "SELECT r.id_performer, r.id_album, r.path, r.year, "
            "COALESCE(p.name, ''), COALESCE(a.name, '') "
            "FROM rolas AS r "
            "LEFT JOIN performers AS p "
            "ON r.id_performer = p.id_performer "
            "LEFT JOIN albums AS a "
            "ON r.id_album = a.id_album "
            "WHERE r.id_rola = ?;";

        sqlite3_stmt* temporal = nullptr;
        int codigo = sqlite3_prepare_v2(
            baseDatos, consulta, -1, &temporal, nullptr
        );

        if (codigo != SQLITE_OK) {
            mensajeError = sqlite3_errmsg(baseDatos);

            if (temporal != nullptr) {
                sqlite3_finalize(temporal);
            }
        } else {
            std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize) > sentencia(temporal, &sqlite3_finalize);
            codigo = sqlite3_bind_int(sentencia.get(), 1, idRola);

            if (codigo != SQLITE_OK) {
                mensajeError = sqlite3_errmsg(baseDatos);
            } else {
                codigo = sqlite3_step(sentencia.get());

                if (codigo == SQLITE_ROW) {
                    idInterpreteActual = sqlite3_column_int(sentencia.get(), 0);
                    tieneAlbumActual = sqlite3_column_type(sentencia.get(), 1) != SQLITE_NULL;

                    if (tieneAlbumActual) {
                        idAlbumActual = sqlite3_column_int(sentencia.get(), 1);
                    }

                    auto obtenerTexto = [&sentencia](int columna) {
                        const unsigned char* texto = sqlite3_column_text(sentencia.get(), columna);
                        return texto == nullptr ? std::string() : std::string(reinterpret_cast<const char*>(texto));
                    };

                    ruta = obtenerTexto(2);
                    artistaActual = obtenerTexto(4);
                    albumActual = obtenerTexto(5);
                    encontrada = true;
                } else if (codigo == SQLITE_DONE) {
                    mensajeError = "No existe una canción con ese identificador.";
                } else {
                    mensajeError = sqlite3_errmsg(baseDatos);
                }
            }
        }
    }

    if (!mensajeError.empty()) {
        cancelarTransaccion();
        return false;
    }

    if (!encontrada) {
        cancelarTransaccion();
        return false;
    }

    // Reutilizar las operaciones existentes para intérpretes y álbumes
    RepositorioMusical repositorio(conexion);
    int idInterprete = idInterpreteActual;

    // Si cambió el artista, buscarlo o registrarlo
    if (artista != artistaActual || idInterprete <= 0) {
        if (!repositorio.obtenerOCrearInterprete(artista, idInterprete, mensajeError)) {
            cancelarTransaccion();
            return false;
        }
    }
    int idAlbum = idAlbumActual;
    bool tieneAlbum = tieneAlbumActual;

    // Si cambió el álbum, buscar su directorio correspondiente
    if (album != albumActual) {
        if (!repositorio.obtenerOCrearAlbum(ruta, album, static_cast<unsigned int>(year),
            idAlbum, tieneAlbum, mensajeError)) {
            cancelarTransaccion();
            return false;
        }
    }

    // Actualizar únicamente la canción seleccionada
    {
        const char* actualizacion =
            "UPDATE rolas "
            "SET id_performer = ?, id_album = ?, title = ?, year = ? "
            "WHERE id_rola = ?;";        
        sqlite3_stmt* temporal = nullptr;

        int codigo = sqlite3_prepare_v2(baseDatos, actualizacion, -1, &temporal, nullptr);

        if (codigo != SQLITE_OK) {
            mensajeError = sqlite3_errmsg(baseDatos);

            if (temporal != nullptr) {
                sqlite3_finalize(temporal);
            }
        } else {
            std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize) > sentencia(temporal, &sqlite3_finalize);
            codigo = sqlite3_bind_int(sentencia.get(), 1, idInterprete);

            if (codigo == SQLITE_OK) {
                if (tieneAlbum) {
                    codigo = sqlite3_bind_int(sentencia.get(), 2, idAlbum);
                } else {
                    codigo = sqlite3_bind_null(sentencia.get(), 2);
                }
            }

            if (codigo == SQLITE_OK) {
                codigo = sqlite3_bind_text(sentencia.get(), 3, titulo.c_str(), -1, SQLITE_TRANSIENT);
            }

            if (codigo == SQLITE_OK) {
                codigo = sqlite3_bind_int(sentencia.get(), 4, year);
            }

            if (codigo == SQLITE_OK) {
                codigo = sqlite3_bind_int(sentencia.get(), 5, idRola);
            }

            // Comprobar que todos los parámetros se asociaron correctamente
            if (codigo != SQLITE_OK) {
                mensajeError = sqlite3_errmsg(baseDatos);
            } else {
                codigo = sqlite3_step(sentencia.get());

                if (codigo != SQLITE_DONE) {
                    mensajeError = sqlite3_errmsg(baseDatos);
                } else if (sqlite3_changes(baseDatos) == 0) {
                    mensajeError = "SQLite no actualizó ningún registro.";
                }
            }
        }
    }

    if (!mensajeError.empty()) {
        cancelarTransaccion();
        return false;
    }

    // Confirmar los cambios
    if (!conexion.ejecutarSQL("COMMIT;", mensajeError)) {
        cancelarTransaccion();
        return false;
    }
    mensajeError.clear();
    return true;
}
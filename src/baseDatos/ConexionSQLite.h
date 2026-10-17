#ifndef CONEXION_SQLITE_H
#define CONEXION_SQLITE_H

#include <sqlite3.h>
#include <memory>
#include <string>

/* Clase para gestionar la conexión a la base de datos SQLite */
class ConexionSQLite {
private:
    struct CerradorSQLite {
        void operator()(sqlite3* conexion) const noexcept;
    };
    std::unique_ptr<sqlite3, CerradorSQLite> conexion;

public:
    ConexionSQLite() = default;
    ConexionSQLite(const ConexionSQLite&) = delete;
    ConexionSQLite& operator=(const ConexionSQLite&) = delete;

    bool abrir(
        const std::string& rutaArchivo,
        std::string& mensajeError
    );
    bool estaAbierta() const noexcept;
    sqlite3* obtenerManejador() const noexcept;
};

#endif
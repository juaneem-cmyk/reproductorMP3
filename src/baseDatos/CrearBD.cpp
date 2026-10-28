#include "CrearBD.h"
#include "ConexionSQLite.h"

// Crear la base de datos y ejecutar su esquema
bool CrearBaseDatos::crearBaseDatos(const std::string& rutaBaseDatos, const std::string& esquema, std::string& mensajeError) const {
    ConexionSQLite conexion;

    // Abrir o crear el archivo de la base de datos
    if (!conexion.abrir(rutaBaseDatos, mensajeError)) {
        return false;
    }

    // Activar las claves foráneas
    if (!conexion.ejecutarSQL(
            "PRAGMA foreign_keys = ON;", mensajeError)) {
        return false;
    }

    // Iniciar una transacción
    if (!conexion.ejecutarSQL("BEGIN IMMEDIATE;", mensajeError)) {
        return false;
    }

    // Ejecutar el esquema SQL original
    if (!conexion.ejecutarSQL(esquema, mensajeError)) {
        std::string errorRollback;
        conexion.ejecutarSQL("ROLLBACK;", errorRollback);
        return false;
    }

    // Confirmar la creación del esquema
    if (!conexion.ejecutarSQL("COMMIT;", mensajeError)) {
        std::string errorRollback;
        conexion.ejecutarSQL("ROLLBACK;", errorRollback);
        return false;
    }
    mensajeError.clear();
    return true;
}
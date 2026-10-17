#include "InicializarBD.h"
#include "ConexionSQLite.h"
#include <filesystem>
#include <fstream>
#include <iterator>
#include <string>

bool InicializadorBaseDatos::inicializar(
    const std::string& rutaBaseDatos, const std::string& rutaEsquema,
    bool& baseDatosCreada, std::string& mensajeError) const {
    namespace fs = std::filesystem;
    baseDatosCreada = false;
    std::error_code errorFS;
    const bool existe = fs::exists(rutaBaseDatos, errorFS);

    // Comprobar si hubo un error al verificar la existencia del archivo
    if (errorFS) {
        mensajeError = "No se pudo comprobar la base de datos: " + errorFS.message();
        return false;
    }

    // Si el archivo de la base de datos ya existe, no volver a ejecutar el esquema
    if (existe) {
        if (!fs::is_regular_file(rutaBaseDatos, errorFS) || errorFS) {
            mensajeError = "La ruta de la base de datos no es un archivo regular.";
            return false;
        }
        mensajeError.clear();
        return true;
    }
    // Leer el esquema antes de crear el archivo de la base de datos
    std::ifstream archivoEsquema(rutaEsquema, std::ios::binary);

    if (!archivoEsquema) {
        mensajeError = "No se pudo abrir el archivo del esquema SQL: " +
            rutaEsquema;
        return false;
    }
    const std::string esquema((std::istreambuf_iterator<char>(archivoEsquema)), std::istreambuf_iterator<char>());

    // Comprobar si hubo un error al leer el archivo del esquema
    if (archivoEsquema.bad() || esquema.empty()) {
        mensajeError = "El archivo del esquema SQL está vacío o no se pudo leer.";
        return false;
    }
    bool inicializacionCorrecta = false;
    std::string detalleError; {
        ConexionSQLite conexion;

        // Intentar abrir o crear la base de datos y ejecutar el esquema
        if (!conexion.abrir(rutaBaseDatos, detalleError)) {
            // No se pudo abrir o crear la base de datos
        } else if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", detalleError)) {
            // No se pudieron activar las claves foráneas
        } else if (!conexion.ejecutarSQL("BEGIN IMMEDIATE;", detalleError)) {
            // No se pudo comenzar la transacción
        } else {
            if (!conexion.ejecutarSQL(esquema, detalleError)) {
                std::string errorRollback;
                conexion.ejecutarSQL("ROLLBACK;", errorRollback);
            } else if (!conexion.ejecutarSQL("COMMIT;", detalleError)) {
                std::string errorRollback;
                conexion.ejecutarSQL("ROLLBACK;", errorRollback);
            } else {
                inicializacionCorrecta = true;
            }
        }
    }

    // Si la inicialización no fue correcta, eliminar el archivo de la base de datos creado
    if (!inicializacionCorrecta) {
        mensajeError = "No se pudo inicializar la base de datos: " + detalleError;
        std::error_code errorEliminar;
        fs::remove(rutaBaseDatos, errorEliminar);

        // Comprobar si hubo un error al eliminar el archivo de la base de datos
        if (errorEliminar) {
            mensajeError += " Además, no se pudo eliminar el archivo creado: " + errorEliminar.message();
        }
        return false;
    }
    baseDatosCreada = true;
    mensajeError.clear();
    return true;
}
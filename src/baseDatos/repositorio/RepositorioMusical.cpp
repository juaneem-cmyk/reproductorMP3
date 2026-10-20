#include "RepositorioMusical.h"

#include <memory>
#include <sqlite3.h>

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

}
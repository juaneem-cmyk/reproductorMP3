#include "RepositorioMusical.h"

// Cuando creamos un objeto RepositorioMusical, le pasamos una conexión SQLite que ya existe
RepositorioMusical::RepositorioMusical(ConexionSQLite& conexion) : conexion(conexion) {
}

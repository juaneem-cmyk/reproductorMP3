#ifndef REPOSITORIO_MUSICAL_H
#define REPOSITORIO_MUSICAL_H

#include "ConexionSQLite.h"
#include <string>

class RepositorioMusical {
public:
    explicit RepositorioMusical(ConexionSQLite& conexion);

    bool existeCancionPorRuta(const std::string& ruta, bool& existe, std::string& mensajeError) const;

    bool obtenerOCrearInterprete(const std::string& nombre, int& idInterprete, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};

#endif
#ifndef REPOSITORIO_MUSICAL_H
#define REPOSITORIO_MUSICAL_H

#include "ConexionSQLite.h"
#include <string>

class RepositorioMusical {
public:
    explicit RepositorioMusical(ConexionSQLite& conexion);

private:
    ConexionSQLite& conexion;
};

#endif
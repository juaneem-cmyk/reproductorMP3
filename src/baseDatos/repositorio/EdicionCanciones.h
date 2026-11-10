#ifndef EDICION_CANCIONES_H
#define EDICION_CANCIONES_H

#include "ConexionSQLite.h"
#include <string>

class EdicionCanciones {
public:
    explicit EdicionCanciones(ConexionSQLite& conexion);

    bool actualizarCancion(int idRola, const std::string& titulo, const std::string& artista,
    const std::string& album, int year, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};
#endif
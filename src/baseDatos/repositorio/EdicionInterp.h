#ifndef EDICION_INTERPRETES_H
#define EDICION_INTERPRETES_H

#include "ConexionSQLite.h"
#include <string>

class EdicionInterpretes {
public:
    explicit EdicionInterpretes(ConexionSQLite& conexion);
    bool actualizarInterprete(int idInterprete, const std::string& nombre,
    int idTipo, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};
#endif
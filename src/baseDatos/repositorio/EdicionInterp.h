#ifndef EDICION_INTERPRETES_H
#define EDICION_INTERPRETES_H

#include "ConexionSQLite.h"
#include <string>

struct DatosPersona {
    std::string nombreReal;
    std::string fechaNacimiento;
    std::string fechaFallecimiento;
};

class EdicionInterpretes {
public:
    explicit EdicionInterpretes(ConexionSQLite& conexion);
    bool obtenerPersona(int idInterprete, DatosPersona& datos, std::string& mensajeError) const;
    bool actualizarInterprete(int idInterprete, const std::string& nombre, int idTipo, 
    const DatosPersona& datosPersona, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};
#endif
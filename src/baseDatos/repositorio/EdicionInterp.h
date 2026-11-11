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
    bool actualizarInterprete(int idInterprete, int idRola,
        const std::string& nombre, int idTipo, const DatosPersona& datosPersona,
        int& idInterpreteActualizado, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};
#endif
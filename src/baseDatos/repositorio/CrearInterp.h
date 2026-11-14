#ifndef CREAR_INTERPRETES_H
#define CREAR_INTERPRETES_H

#include "EdicionInterp.h"
#include <string>
#include <vector>

class CrearInterpretes {
public:
    explicit CrearInterpretes(ConexionSQLite& conexion);

    bool crearInterprete(const std::string& nombre, int idTipo,
        const DatosPersona& datosPersona, const DatosGrupo& datosGrupo,
        const std::vector<int>& integrantes, int& idInterprete,
        std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};
#endif
#ifndef EDICION_INTERPRETES_H
#define EDICION_INTERPRETES_H

#include "ConexionSQLite.h"
#include <string>
#include <vector>

struct DatosPersona {
    std::string nombreReal;
    std::string fechaNacimiento;
    std::string fechaFallecimiento;
};

struct DatosGrupo {
    std::string fechaCreacion;
    std::string fechaSeparacion;
};

struct DatosIntegrante {
    int idPersona;
    std::string nombre;
};

class EdicionInterpretes {
public:
    explicit EdicionInterpretes(ConexionSQLite& conexion);
    bool obtenerPersona(int idInterprete, DatosPersona& datos, std::string& mensajeError) const;
    bool obtenerGrupo(int idInterprete, DatosGrupo& datos, std::string& mensajeError) const;
    bool obtenerIntegrantes(int idGrupo, std::vector<DatosIntegrante>& integrantes, std::string& mensajeError) const;
    bool obtenerPersonasDisponibles(std::vector<DatosIntegrante>& personas, std::string& mensajeError) const;
    bool actualizarInterprete(int idInterprete, int idRola,
        const std::string& nombre, int idTipo, const DatosPersona& datosPersona,
        const DatosGrupo& datosGrupo, const std::vector<int>& integrantes,
        int& idInterpreteActualizado, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};
#endif
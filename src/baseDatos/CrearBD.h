#ifndef CREAR_BASE_DATOS_H
#define CREAR_BASE_DATOS_H

#include <string>

class CrearBaseDatos {
public:
    bool crearBaseDatos(
        const std::string& rutaBaseDatos,
        const std::string& esquema,
        std::string& mensajeError) const;
};

#endif
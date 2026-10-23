#ifndef INICIALIZADORBD_H
#define INICIALIZADORBD_H

#include <string>

class InicializadorBaseDatos {
public:
    bool inicializar(
        const std::string& rutaBaseDatos,
        const std::string& rutaEsquema,
        bool& baseDatosCreada,
        std::string& mensajeError
    ) const;
};

#endif
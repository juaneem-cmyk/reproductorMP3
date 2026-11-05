#ifndef CONTROLADOR_MINERO_H
#define CONTROLADOR_MINERO_H

#include <string>

class ControladorMinero {
public:
    int ejecutar(
        int numeroArgumentos,
        char* listaArgumentos[]
    ) const;

private:
    int procesarRuta(const std::string& argumento) const;
};

#endif
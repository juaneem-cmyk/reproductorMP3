#ifndef BIBLIOTECA_H
#define BIBLIOTECA_H

#include <string>

class Biblioteca {
public:
    bool preparar(bool& baseDatosCreada, std::string& mensajeError) const;
};

#endif
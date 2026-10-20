#ifndef RUTAS_H
#define RUTAS_H

#include <filesystem>
#include <string>

class Rutas {
public:
    static std::filesystem::path rutaBaseDatos();
    static std::filesystem::path rutaEsquemaSQL();
    static bool prepararDirectorioDatos(std::string& mensajeError);
};

#endif
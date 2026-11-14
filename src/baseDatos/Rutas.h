#ifndef RUTAS_H
#define RUTAS_H

#include <filesystem>
#include <string>

class Rutas {
public:
    static std::filesystem::path rutaBaseDatos();
    static std::filesystem::path rutaEsquemaSQL();
    static std::filesystem::path rutaDirectorioDatos();
    static std::filesystem::path rutaImagenCancion(int idRola);
    static std::filesystem::path rutaImagenInterprete(int idInterprete);
    static bool prepararDirectorioDatos(std::string& mensajeError);
    static bool prepararDirectoriosImagenes(std::string& mensajeError);
};
#endif
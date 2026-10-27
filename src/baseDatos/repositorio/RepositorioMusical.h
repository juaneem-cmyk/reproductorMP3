#ifndef REPOSITORIO_MUSICAL_H
#define REPOSITORIO_MUSICAL_H

#include "ConexionSQLite.h"
#include <string>

struct MetadatosMusicales;

class RepositorioMusical {
public:
    static bool crearBaseDatos(
    const std::string& rutaBaseDatos,
    const std::string& esquema,
    std::string& mensajeError);
    
    explicit RepositorioMusical(ConexionSQLite& conexion);

    bool existeCancionPorRuta(const std::string& ruta, bool& existe, std::string& mensajeError) const;

    bool obtenerOCrearInterprete(const std::string& nombre, int& idInterprete, std::string& mensajeError) const;

    bool obtenerOCrearAlbum(const std::string& rutaArchivo, const std::string& nombreAlbum, unsigned int year, int& idAlbum, bool& tieneAlbum, std::string& mensajeError) const;

    bool registrarCancion(const std::string& rutaArchivo, const MetadatosMusicales& metadatos, bool& insertada, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};

#endif
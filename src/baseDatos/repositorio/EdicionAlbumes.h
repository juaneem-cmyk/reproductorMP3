#ifndef EDICION_ALBUMES_H
#define EDICION_ALBUMES_H

#include "ConexionSQLite.h"
#include <string>
#include <vector>

struct DatosAlbum {
    int idAlbum = 0;
    std::string nombre;
    std::string ruta;
    int year = 0;
    int cantidadCanciones = 0;
};

class EdicionAlbumes {
public:
    explicit EdicionAlbumes(ConexionSQLite& conexion);
    bool obtenerAlbumes(std::vector<DatosAlbum>& albumes, std::string& mensajeError) const;
    bool actualizarAlbumes(const std::vector<DatosAlbum>& albumes, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};
#endif
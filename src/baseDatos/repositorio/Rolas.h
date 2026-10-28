#ifndef ROLAS_H
#define ROLAS_H

#include "ConexionSQLite.h"
#include <string>
#include <vector>

// Información de una canción para mostrar en la biblioteca
struct CancionBiblioteca {
    int idRola = 0;
    std::string titulo;
    std::string artista;
    std::string album;
    std::string ruta;
    int pista = 0;
    int anio = 0;
    std::string genero;
};

class Rolas {
public:
    explicit Rolas(ConexionSQLite& conexion);

    bool obtenerCanciones(std::vector<CancionBiblioteca>& canciones, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};

#endif
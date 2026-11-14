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
    int year = 0;
    std::string genero;
    int idInterprete = 0;
    int idTipoInterprete = 2;
};

class Rolas {
public:
    explicit Rolas(ConexionSQLite& conexion);

    bool obtenerCanciones(std::vector<CancionBiblioteca>& canciones, std::string& mensajeError) const;

private:
    ConexionSQLite& conexion;
};

#endif
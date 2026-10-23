#ifndef LECTOR_METADATOS_H
#define LECTOR_METADATOS_H

#include <string>

struct MetadatosMusicales {
    std::string titulo;
    std::string artista;
    std::string album;
    std::string genero;

    unsigned int year = 0;
    unsigned int numeroPista = 0;
    unsigned int duracionSegundos = 0;
};

class LectorMetadatosMP3 {
public:
    // Lee los metadatos sin modificar el archivo original.
    bool leerArchivo(const std::string& rutaArchivo, MetadatosMusicales& metadatos, std::string& mensajeError) const;
};

#endif
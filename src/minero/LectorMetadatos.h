#ifndef LECTOR_METADATOS_H
#define LECTOR_METADATOS_H

#include <string>
#include <vector>

struct ComentarioMusical {
    std::string idioma;
    std::string descripcion;
    std::string texto;
};

struct ImagenIncrustada {
    std::string descripcion;
    std::string tipoMime;
    int tipo = -1;
    std::vector<unsigned char> datos;
};

struct MetadatosMusicales {
    std::string titulo;
    std::string artista;
    std::string album;
    std::string genero;
    // Información adicional de las etiquetas ID3
    std::string artistaAlbum;
    std::string textoPista;
    std::string textoDisco;
    std::string compositor;
    std::string artistaOriginal;

    std::vector<ComentarioMusical> comentarios;
    ImagenIncrustada portadaFrontal;
    ImagenIncrustada imagenArtista;

    unsigned int year = 0;
    unsigned int numeroPista = 0;
    unsigned int duracionSegundos = 0;
    unsigned int totalPistas = 0;
    unsigned int numeroDisco = 0;
    unsigned int totalDiscos = 0;
};

class LectorMetadatosMP3 {
public:
    // Lee los metadatos sin modificar el archivo original.
    bool leerArchivo(const std::string& rutaArchivo, MetadatosMusicales& metadatos, std::string& mensajeError) const;
};

#endif
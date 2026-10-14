#include "LectorMetadatos.h"
#include <taglib/mpegfile.h>
#include <taglib/tag.h>
#include <taglib/audioproperties.h>

bool LectorMetadatosMP3::leerArchivo(
    const std::string& rutaArchivo, MetadatosMusicales& metadatos, std::string& mensajeError) const {
    TagLib::MPEG::File archivo(rutaArchivo.c_str());

    if (!archivo.isValid()) {
        mensajeError = "El archivo no es un MP3 válido o no se pudo leer.";
        return false;
    }
    TagLib::Tag* etiqueta = archivo.tag();

    if (etiqueta != nullptr) {
        metadatos.titulo = etiqueta->title().to8Bit(true);
        metadatos.artista = etiqueta->artist().to8Bit(true);
        metadatos.album = etiqueta->album().to8Bit(true);
        metadatos.year = etiqueta->year();
        metadatos.numeroPista = etiqueta->track();
    }
    TagLib::AudioProperties* propiedades = archivo.audioProperties();

    if (propiedades != nullptr) {
        metadatos.duracionSegundos = static_cast<unsigned int>(propiedades->lengthInSeconds());
    }
    mensajeError.clear();
    return true;
}
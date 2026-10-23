#include "LectorMetadatos.h"
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2header.h>
#include <taglib/audioproperties.h>
#include <string>

/** Lee los metadatos de un archivo MP3 y los almacena en una estructura.
    Devuelve true si los metadatos se leen correctamente, false en caso contrario. */
bool LectorMetadatosMP3::leerArchivo(const std::string& rutaArchivo, MetadatosMusicales& metadatos, std::string& mensajeError) const {
    // Evita conservar datos de una lectura anterior si se reutiliza el objeto
    metadatos = MetadatosMusicales{};
    TagLib::MPEG::File archivo(rutaArchivo.c_str());

    if (!archivo.isValid()) {
        mensajeError = "El archivo no es un MP3 válido o no se pudo leer.";
        return false;
    }
    TagLib::ID3v2::Tag* etiqueta = archivo.ID3v2Tag();

    if (etiqueta == nullptr) {
        mensajeError = "El archivo no contiene una etiqueta ID3v2.4.0.";
        return false;
    }
    TagLib::ID3v2::Header* cabecera = etiqueta->header();

    // Se verifica que la etiqueta sea de la versión requerida (ID3v2.4.0)
    if (cabecera == nullptr) {
        mensajeError = "No se pudo leer la cabecera de la etiqueta ID3v2.";
        return false;
    }
    const unsigned int versionMayor = cabecera->majorVersion();
    const unsigned int revision = cabecera->revisionNumber();

    if (versionMayor != 4 || revision != 0) {
        mensajeError = "La etiqueta encontrada es ID3v2." + std::to_string(versionMayor) + "." + std::to_string(revision) + ", pero el proyecto requiere ID3v2.4.0.";
        return false;
    }
    // Los campos pueden faltar aunque la etiqueta sí sea ID3v2.4.0
    metadatos.titulo = etiqueta->title().to8Bit(true);
    metadatos.artista = etiqueta->artist().to8Bit(true);
    metadatos.album = etiqueta->album().to8Bit(true);
    metadatos.year = etiqueta->year();
    metadatos.numeroPista = etiqueta->track();
    metadatos.genero = etiqueta->genre().to8Bit(true);
    TagLib::AudioProperties* propiedades = archivo.audioProperties();

    // Se obtiene la duración del archivo en segundos si está disponible
    if (propiedades != nullptr) {
        const int duracion = propiedades->lengthInSeconds();
        if (duracion > 0) {
            metadatos.duracionSegundos = static_cast<unsigned int>(duracion);
        }
    }
    mensajeError.clear();
    return true;
}
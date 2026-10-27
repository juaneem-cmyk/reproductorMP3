#include "LectorMetadatos.h"
#include <taglib/mpegfile.h>
#include <taglib/id3v2tag.h>
#include <taglib/id3v2header.h>
#include <taglib/audioproperties.h>
#include <taglib/attachedpictureframe.h>
#include <taglib/commentsframe.h>
#include <taglib/textidentificationframe.h>
#include <limits>
#include <string>

namespace {
// Leer el texto de un marco de identificación de texto
std::string leerTextoMarco(
    TagLib::ID3v2::Tag* etiqueta, const char* identificador) {
    const TagLib::ID3v2::FrameList marcos = etiqueta->frameList(identificador);

    // Se toma el primer marco que coincida con el identificador
    for (auto* marco : marcos) {
        auto* marcoTexto = dynamic_cast<TagLib::ID3v2::TextIdentificationFrame*>(marco);

        if (marcoTexto != nullptr) {
            return marcoTexto->toString().to8Bit(true);
        }
    }
    return {};
}

// Convertir una cadena numérica en un entero sin perder el valor por errores
unsigned int convertirNumero(const std::string& texto) {
    if (texto.empty()) {
        return 0;
    }

    // Se utiliza stoul para convertir la cadena a un número sin signo
    try {
        std::size_t caracteresLeidos = 0;
        const unsigned long valor = std::stoul(texto, &caracteresLeidos);

        if (caracteresLeidos != texto.size() || valor > std::numeric_limits<unsigned int>::max()) {
            return 0;
        }
        return static_cast<unsigned int>(valor);
    } catch (...) {
        return 0;
    }
}

// Leer valores como 03/07, que representan pista 3 de 7
void separarPosicion(
    const std::string& texto, unsigned int& numero, unsigned int& total) {
    numero = 0;
    total = 0;
    const std::size_t separador = texto.find('/');

    if (separador == std::string::npos) {
        numero = convertirNumero(texto);
        return;
    }
    numero = convertirNumero(texto.substr(0, separador));
    total = convertirNumero(texto.substr(separador + 1));
}

// Copiar los datos de un marco APIC sin modificar el MP3
ImagenIncrustada copiarImagen(
    const TagLib::ID3v2::AttachedPictureFrame& marco) {
    ImagenIncrustada imagen;
    imagen.descripcion = marco.description().to8Bit(true);
    imagen.tipoMime = marco.mimeType().to8Bit(true);
    imagen.tipo = static_cast<int>(marco.type());
    const TagLib::ByteVector datos = marco.picture();

    if (datos.size() > 0) {
        const auto* inicio = reinterpret_cast<const unsigned char*>(datos.data());
        imagen.datos.assign(inicio, inicio + datos.size());
    }
    return imagen;
}
} 

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
        mensajeError = "La etiqueta encontrada es ID3v2." + std::to_string(versionMayor) + "." + 
        std::to_string(revision) + ", pero el proyecto requiere ID3v2.4.0.";
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

    // Leer los campos de texto adicionales de ID3v2.4.0
    metadatos.artistaAlbum = leerTextoMarco(etiqueta, "TPE2");
    metadatos.compositor = leerTextoMarco(etiqueta, "TCOM");
    metadatos.artistaOriginal = leerTextoMarco(etiqueta, "TOPE");
    // Conservar el texto original de la pista y separar sus números
    metadatos.textoPista = leerTextoMarco(etiqueta, "TRCK");

    if (!metadatos.textoPista.empty()) {
        unsigned int numeroPista = 0;
        unsigned int totalPistas = 0;
        separarPosicion(metadatos.textoPista, numeroPista, totalPistas);

        if (numeroPista != 0) {
            metadatos.numeroPista = numeroPista;
        }
        metadatos.totalPistas = totalPistas;
    }
    // Leer el número de disco y el total, si están presentes
    metadatos.textoDisco = leerTextoMarco(etiqueta, "TPOS");

    if (!metadatos.textoDisco.empty()) {
        separarPosicion(metadatos.textoDisco, metadatos.numeroDisco, metadatos.totalDiscos);
    }

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
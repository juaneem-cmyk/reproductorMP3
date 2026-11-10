#include "GestorImagenes.h"
#include "Rutas.h"
#include <QSaveFile>
#include <QIODevice>
#include <system_error>

// Cargar la imagen personalizada de una canción
QImage GestorImagenes::cargarCancion(int idRola) {
    const std::filesystem::path ruta = Rutas::rutaImagenCancion(idRola);

    if (ruta.empty()) {
        return {};
    }
    return QImage(QString::fromStdString(ruta.string()));
}

// Cargar la imagen personalizada de un intérprete
QImage GestorImagenes::cargarInterprete(int idInterprete) {
    const std::filesystem::path ruta = Rutas::rutaImagenInterprete(idInterprete);

    if (ruta.empty()) {
        return {};
    }
    return QImage(QString::fromStdString(ruta.string()));
}

// Guardar la imagen personalizada de una canción
bool GestorImagenes::guardarCancion(int idRola, const QString& rutaImagen, QString& mensajeError) {
    if (idRola <= 0) {
        mensajeError = "El identificador de la canción no es válido.";
        return false;
    }
    return guardarImagen(rutaImagen, Rutas::rutaImagenCancion(idRola), mensajeError);
}

// Guardar la imagen personalizada de un intérprete
bool GestorImagenes::guardarInterprete(int idInterprete, const QString& rutaImagen, QString& mensajeError) {
    if (idInterprete <= 0) {
        mensajeError = "El identificador del intérprete no es válido.";
        return false;
    }
    return guardarImagen(rutaImagen, Rutas::rutaImagenInterprete(idInterprete), mensajeError);
}

// Eliminar la imagen personalizada de una canción para restaurar la incrustada
bool GestorImagenes::eliminarCancion(int idRola, QString& mensajeError) {
    if (idRola <= 0) {
        mensajeError = "El identificador de la canción no es válido.";
        return false;
    }
    return eliminarImagen(Rutas::rutaImagenCancion(idRola), mensajeError);
}

// Eliminar la imagen personalizada de un intérprete para restaurar la incrustada
bool GestorImagenes::eliminarInterprete(int idInterprete, QString& mensajeError) {
    if (idInterprete <= 0) {
        mensajeError = "El identificador del intérprete no es válido.";
        return false;
    }
    return eliminarImagen(Rutas::rutaImagenInterprete(idInterprete), mensajeError);
}

// Validar y guardar la imagen en formato PNG sin modificar el archivo de origen
bool GestorImagenes::guardarImagen(const QString& rutaOrigen,
    const std::filesystem::path& rutaDestino, QString& mensajeError) {
    QImage imagen(rutaOrigen);

    if (imagen.isNull()) {
        mensajeError = "No se pudo leer la imagen seleccionada.";
        return false;
    }
    std::string errorDirectorios;

    if (!Rutas::prepararDirectoriosImagenes(errorDirectorios)) {
        mensajeError = QString::fromStdString(errorDirectorios);
        return false;
    }
    QSaveFile archivo(QString::fromStdString(rutaDestino.string()));

    if (!archivo.open(QIODevice::WriteOnly)) {
        mensajeError = "No se pudo abrir el destino para guardar la imagen: " + archivo.errorString();
        return false;
    }

    if (!imagen.save(&archivo, "PNG")) {
        archivo.cancelWriting();
        mensajeError = "No se pudo convertir la imagen al formato PNG.";
        return false;
    }

    if (!archivo.commit()) {
        mensajeError = "No se pudo guardar la imagen: " + archivo.errorString();
        return false;
    }
    mensajeError.clear();
    return true;
}

// Eliminar la imagen personalizada; si no existe, la operación ya está satisfecha
bool GestorImagenes::eliminarImagen(const std::filesystem::path& rutaImagen, QString& mensajeError) {
    std::error_code error;
    std::filesystem::remove(rutaImagen, error);

    if (error) {
        mensajeError = "No se pudo eliminar la imagen personalizada: " + QString::fromStdString(error.message());
        return false;
    }
    mensajeError.clear();
    return true;
}
#ifndef GESTOR_IMAGENES_H
#define GESTOR_IMAGENES_H

#include <QImage>
#include <filesystem>
#include <QString>

class GestorImagenes {
public:
    static QImage cargarCancion(int idRola);
    static QImage cargarInterprete(int idInterprete);
    static bool guardarCancion(int idRola, const QString& rutaImagen, QString& mensajeError);
    static bool guardarInterprete(int idInterprete, const QString& rutaImagen, QString& mensajeError);
    static bool eliminarCancion(int idRola, QString& mensajeError);
    static bool eliminarInterprete(int idInterprete, QString& mensajeError);

private:
    static bool guardarImagen(const QString& rutaOrigen, 
        const std::filesystem::path& rutaDestino, QString& mensajeError);
    static bool eliminarImagen(const std::filesystem::path& rutaImagen, QString& mensajeError);
};
#endif
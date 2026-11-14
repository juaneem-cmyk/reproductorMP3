#include "Canciones.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include "ConsultaRolas.h"
#include "LectorMetadatos.h"
#include "../servicios/GestorImagenes.h"
#include <QDebug>
#include <QImage>
#include <QPixmap>

namespace {
constexpr int TAMANO_PORTADA = 256;

// Reserva el espacio de la portada sin dibujar nada cuando no hay imagen
QImage crearPortadaVacia() {
    QImage imagen(TAMANO_PORTADA, TAMANO_PORTADA, QImage::Format_ARGB32);
    imagen.fill(Qt::transparent);
    return imagen;
}

// Recorta las portadas a un cuadrado de buena resolución para ambas vistas
QImage ajustarPortada(const QImage &original) {
    if (original.isNull()) {
        return crearPortadaVacia();
    }

    QImage escalada = original.scaled(TAMANO_PORTADA, TAMANO_PORTADA,
        Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    int x = (escalada.width() - TAMANO_PORTADA) / 2;
    int y = (escalada.height() - TAMANO_PORTADA) / 2;
    return escalada.copy(x, y, TAMANO_PORTADA, TAMANO_PORTADA);
}
}

Canciones::Canciones(QObject *parent) : QAbstractTableModel(parent) {
    cargarCanciones();
}

// Carga las canciones desde la base de datos y las almacena en el vector "canciones"
void Canciones::cargarCanciones() {
    ConexionSQLite conexion;
    std::string mensajeError;

    // Abrimos la base de datos de la biblioteca
    if (!conexion.abrir(Rutas::rutaBaseDatos().string(), mensajeError)) {
        qWarning() << "No se pudo abrir la biblioteca:" << QString::fromStdString(mensajeError);
        return;
    }

    // Consultamos las canciones registradas
    Rolas consulta(conexion);
    std::vector<CancionBiblioteca> resultados;

    if (!consulta.obtenerCanciones(resultados, mensajeError)) {
        qWarning() << "No se pudieron consultar las canciones:" << QString::fromStdString(mensajeError);
        return;
    }

    // Transferimos los resultados al modelo de Qt
    canciones.reserve(resultados.size());
    portadas.reserve(resultados.size());
    LectorMetadatosMP3 lector;

    for (const auto &cancion : resultados) {
        MetadatosMusicales metadatos;
        std::string errorLectura;
        QString duracion = "--:--";
        // Priorizar la portada personalizada de la canción
        QImage portada = GestorImagenes::cargarCancion(cancion.idRola);

        // Intentamos leer la duración y, si hace falta, la portada incrustada del MP3
        if (lector.leerArchivo(cancion.ruta, metadatos, errorLectura)) {
            unsigned int segundos = metadatos.duracionSegundos;
            unsigned int minutos = segundos / 60;
            segundos %= 60;
            duracion = QString("%1:%2").arg(minutos).arg(segundos, 2, 10, QChar('0'));

            if (portada.isNull() && !metadatos.portadaFrontal.datos.empty()) {
                portada.loadFromData(metadatos.portadaFrontal.datos.data(),
                static_cast<int>(metadatos.portadaFrontal.datos.size()));
            }
        }

        // Todas las filas reciben una miniatura cuadrada para mantener alineados los títulos
        QImage portadaAjustada = ajustarPortada(portada);
        portadas.emplace_back(QPixmap::fromImage(portadaAjustada));

        // Agregamos la canción al vector de canciones del modelo
        canciones.push_back({
            cancion.idRola,
            QString::fromStdString(cancion.titulo),
            QString::fromStdString(cancion.artista),
            QString::fromStdString(cancion.album),
            cancion.year,
            QString::fromStdString(cancion.ruta),
            duracion,
            cancion.idInterprete, cancion.idTipoInterprete
        });
    }
}

int Canciones::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(canciones.size());
}

int Canciones::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return 5;
}

// Se encarga de proporcionar los datos de cada celda de la tabla
QVariant Canciones::data(const QModelIndex &indice, int rol) const {
    if (!indice.isValid()) {
        return {};
    }

    const CancionVista &cancion = canciones[indice.row()];

    // Texto combinado para las búsquedas
    if (rol == TextoBusquedaRole) {
        return QString("%1 %2 %3 %4")
        .arg(cancion.titulo).arg(cancion.artista).arg(cancion.album)
        .arg(cancion.year > 0 ? QString::number(cancion.year) : QString());
    }

    // Mostrar la miniatura junto al título de cada canción
    if (rol == Qt::DecorationRole && indice.column() == 0
    && indice.row() < static_cast<int>(portadas.size())) {
        return portadas[indice.row()];
    }

    if (rol == Qt::ToolTipRole) {
        QString informacion = cancion.titulo;

        if (!cancion.artista.isEmpty()) {
            informacion += "\n" + cancion.artista;
        }
        if (!cancion.album.isEmpty()) {
            informacion += "\n" + cancion.album;
        }
        if (cancion.year > 0) {
            informacion += " · " + QString::number(cancion.year);
        }
        return informacion;
    }

    if (rol == Qt::TextAlignmentRole && (indice.column() == 3 || indice.column() == 4)) {
        return Qt::AlignCenter;
    }

    if (rol != Qt::DisplayRole) {
        return {};
    }

    switch (indice.column()) {
        case 0:
        return cancion.titulo;
        case 1:
        return cancion.artista;
        case 2:
        return cancion.album;
        case 3:
        return cancion.year;
        case 4:
        return cancion.duracion;
        default:
        return {};
    }
}

// Se encarga de proporcionar los encabezados de las columnas de la tabla
QVariant Canciones::headerData(int seccion, Qt::Orientation orientacion, int rol) const {
    if (rol != Qt::DisplayRole) {
        return {};
    }

    if (orientacion == Qt::Horizontal) {
        switch (seccion) {
            case 0:
            return "Título";
            case 1:
            return "Artista";
            case 2:
            return "Álbum";
            case 3:
            return "Año";
            case 4:
            return "Duración";
        }
    }
    return {};
}

QString Canciones::rutaEnFila(int fila) const {
    if (fila < 0 || fila >= static_cast<int>(canciones.size())) {
        return {};
    }
    return canciones[fila].ruta;
}

bool Canciones::obtenerCancion(int fila, CancionVista &cancion) const {
    if (fila < 0 || fila >= static_cast<int>(canciones.size())) {
        return false;
    }

    cancion = canciones[fila];
    return true;
}

void Canciones::recargar() {
    beginResetModel();
    canciones.clear();
    portadas.clear();
    cargarCanciones();
    endResetModel();
}
#include "Canciones.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include "ConsultaRolas.h"
#include <QDebug>

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

    for (const auto &cancion : resultados) {
        canciones.push_back({
            QString::fromStdString(cancion.titulo),
            QString::fromStdString(cancion.artista),
            QString::fromStdString(cancion.album),
            cancion.year,
            QString::fromStdString(cancion.ruta)
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
    return 4;
}

// Se encarga de proporcionar los datos de cada celda de la tabla
QVariant Canciones::data(const QModelIndex &indice, int rol) const {
    if (!indice.isValid() || rol != Qt::DisplayRole) {
        return {};
    }
    const CancionVista &cancion = canciones[indice.row()];

    switch (indice.column()) {
        case 0:
        return cancion.titulo;
        case 1:
        return cancion.artista;
        case 2:
        return cancion.album;
        case 3:
        return cancion.year;
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
        }
    }
    return {};
}
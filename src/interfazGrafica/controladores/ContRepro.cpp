#include "ContRepro.h"
#include "../paneles/PanelAlbum.h"
#include "../paneles/PanelCanciones.h"
#include "../paneles/PanelControles.h"
#include "../modelos/Canciones.h"
#include "../servicios/GestorImagenes.h"
#include "../reproduccion/ControladoRep.h"
#include "LectorMetadatos.h"
#include <QImage>
#include <QMediaPlayer>
#include <QModelIndex>
#include <QPushButton>
#include <QSlider>
#include <QTableView>

ControladorRepUI::ControladorRepUI(PanelControles *panelControles, PanelAlbum *panelAlbum,
    PanelCanciones *panelCanciones, QWidget *ventanaPadre)
    : QObject(ventanaPadre), panelControles(panelControles), panelAlbum(panelAlbum),
    panelCanciones(panelCanciones), controlador(new ControladorReproduccion(this)), filaActual(-1) {
    conectarControles();
}

void ControladorRepUI::conectarControles() {
    // Doble clic para reproducir
    connect(panelCanciones->obtenerTabla(), &QTableView::doubleClicked, this, [this](const QModelIndex& indice) {
            reproducirFila(indice.row());
        }
    );

    // Botón reproducir / pausar
    connect(panelControles->obtenerBotonReproducir(), &QPushButton::clicked, this, [this]() {
        if (controlador->estaReproduciendo()){
            controlador->pausar();
        } else {
            controlador->reproducir();
        }
    });

    // Canción anterior
    connect(panelControles->obtenerBotonAnterior(), &QPushButton::clicked, this, [this]() {
        reproducirFila(filaActual - 1);
    });

    // Canción siguiente
    connect(panelControles->obtenerBotonSiguiente(), &QPushButton::clicked, this, [this]() {
        reproducirFila(filaActual + 1);
    });

    // Control de volumen
    QSlider *controlVolumen = panelControles->obtenerControlVolumen();
    controlador->establecerVolumen(controlVolumen->value() / 100.0f);

    connect(controlVolumen, &QSlider::valueChanged, this, [this](int volumen) {
        controlador->establecerVolumen(volumen / 100.0f);
    });

    // Actualizar el tiempo de reproducción
    QMediaPlayer *reproductor = controlador->obtenerReproductor();
    connect(reproductor, &QMediaPlayer::positionChanged, this, [this](qint64 posicion) {
        panelControles->actualizarTiempo(posicion, controlador->obtenerDuracion());
    });

    connect(reproductor, &QMediaPlayer::durationChanged, this, [this](qint64 duracion) {
        panelControles->actualizarTiempo(controlador->obtenerPosicion(), duracion);
    });

    // Actualizar el icono de reproducción / pausa
    connect(reproductor, &QMediaPlayer::playbackStateChanged, this, [this](QMediaPlayer::PlaybackState estado) {
            panelControles->establecerEstadoReproduccion(estado == QMediaPlayer::PlayingState);
        }
    );

    // Permitir adelantar o retroceder desde la barra de progreso
    connect(panelControles->obtenerBarraProgreso(), &QSlider::sliderMoved, this, [this](int valor) {
        qint64 duracion = controlador->obtenerDuracion();

        if (duracion > 0) {
            qint64 posicion = (duracion * valor) / panelControles->obtenerBarraProgreso()->maximum();
            controlador->establecerPosicion(posicion);
        }
    });

    // Aplicar también la posición final al soltar la barra
    connect(panelControles->obtenerBarraProgreso(), &QSlider::sliderReleased, this, [this]() {
        qint64 duracion = controlador->obtenerDuracion();
        QSlider *barraProgreso = panelControles->obtenerBarraProgreso();

        if (duracion > 0) {
            qint64 posicion = (duracion * barraProgreso->value()) / barraProgreso->maximum();
            controlador->establecerPosicion(posicion);
        }
    });

    // Reproducir automáticamente la siguiente canción
    connect(reproductor, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus estado) {
            if (estado == QMediaPlayer::EndOfMedia){
                reproducirFila(filaActual + 1);
            }
        }
    );
}

void ControladorRepUI::reproducirFila(int fila) {
    int totalCanciones = panelCanciones->obtenerTabla()->model()->rowCount();

    if (fila < 0 || fila >= totalCanciones) {
        return;
    }
    filaActual = fila;

    // Seleccionar la canción que se está reproduciendo
    QTableView *tabla = panelCanciones->obtenerTabla();
    tabla->selectRow(fila);
    tabla->scrollTo(tabla->model()->index(fila, 0));

    QString ruta = panelCanciones->obtenerRutaCancion(fila);

    if (ruta.isEmpty()) {
        return;
    }
    controlador->cargarCancion(ruta);
    controlador->reproducir();

    // Priorizar la portada personalizada y, si no existe, usar la incrustada en el MP3
    QImage imagen;
    CancionVista cancionSeleccionada;

    if (panelCanciones->CancionSelect(cancionSeleccionada)) {
        imagen = GestorImagenes::cargarCancion(cancionSeleccionada.idRola);
    }

    if (imagen.isNull()) {
        LectorMetadatosMP3 lector;
        MetadatosMusicales metadatos;
        std::string error;

        if (lector.leerArchivo(ruta.toStdString(), metadatos, error)
            && !metadatos.portadaFrontal.datos.empty()) {
            imagen.loadFromData(metadatos.portadaFrontal.datos.data(),
                static_cast<int>(metadatos.portadaFrontal.datos.size()));
        }
    }

    if (imagen.isNull()) {
        panelAlbum->limpiarPortada();
    } else {
        panelAlbum->establecerPortada(imagen);
    }
}
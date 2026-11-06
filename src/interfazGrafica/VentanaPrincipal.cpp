#include "VentanaPrincipal.h"
#include "componentes/Portada.h"
#include "paneles/PanelAlbum.h"
#include "paneles/PanelCanciones.h"
#include "paneles/PanelControles.h"
#include "reproduccion/ControladoRep.h"
#include "../minero/LectorMetadatos.h"
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QModelIndex>
#include <QImage>
#include <QDebug>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <Qt>

VentanaPrincipal::VentanaPrincipal(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Reproductor");
    resize(1000, 650);
    setMinimumSize(800, 500);
    // Contenedor principal
    QWidget *contenedor = new QWidget(this);
    contenedor->setObjectName("contenedorPrincipal");
    QVBoxLayout *layout = new QVBoxLayout(contenedor);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    // Encabezado
    QLabel *encabezado = new QLabel("Reproductor", contenedor);
    encabezado->setObjectName("encabezado");
    encabezado->setMinimumHeight(60);
    encabezado->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);

    // Reproductor
    QWidget *areaTrabajo = new QWidget(contenedor);
    areaTrabajo->setObjectName("areaTrabajo");    
    QHBoxLayout *layoutPrincipal = new QHBoxLayout(areaTrabajo);
    layoutPrincipal->setContentsMargins(20, 20, 20, 20);
    layoutPrincipal->setSpacing(15);

    // Panel izquierdo: información del álbum
    PanelAlbum *panelIzquierdo = new PanelAlbum(areaTrabajo);

    // Panel derecho: lista de canciones
    PanelCanciones *panelDerecho = new PanelCanciones(areaTrabajo);

    // Agregar paneles al área de trabajo
    layoutPrincipal->addWidget(panelIzquierdo, 2);
    layoutPrincipal->addWidget(panelDerecho, 3);

    // Agregar los elementos al contenedor principal
    layout->addWidget(encabezado);
    layout->addWidget(areaTrabajo, 1);

    // Controles inferiores
    PanelControles *panelControles = new PanelControles(contenedor);
    layout->addWidget(panelControles);
    ControladorReproduccion *controlador = new ControladorReproduccion(this);

    // Reproducir una canción al hacer doble clic
    LectorMetadatosMP3 lector;    
    connect(panelDerecho->obtenerTabla(), &QTableView::doubleClicked, this, [=](const QModelIndex &indice) {
        if (!indice.isValid())
            return;

        QString ruta = panelDerecho->obtenerRutaCancion(indice.row());

        // Cargar y reproducir la canción
        controlador->cargarCancion(ruta);
        controlador->reproducir();

        // Leer los metadatos para obtener la portada
        MetadatosMusicales metadatos;
        std::string error;

        if (!lector.leerArchivo(ruta.toStdString(), metadatos, error)) {
            qWarning() << "No se pudieron leer los metadatos:" << QString::fromStdString(error);
            panelIzquierdo->limpiarPortada();
            return;
        }
        const auto &datos = metadatos.portadaFrontal.datos;

        if (datos.empty()) {
            panelIzquierdo->limpiarPortada();
            return;
        }
        QImage imagen = QImage::fromData(reinterpret_cast<const uchar *>(datos.data()), static_cast<int>(datos.size()));

        if (imagen.isNull()) {
            panelIzquierdo->limpiarPortada();
            return;
        }
        panelIzquierdo->establecerPortada(imagen);
    });

    // Botón reproducir / pausar
    connect(panelControles->obtenerBotonReproducir(), &QPushButton::clicked, this, [=]() {
        if (controlador->estaReproduciendo())
            controlador->pausar();
        else
            controlador->reproducir();
    });

    // Botón anterior
    connect(panelControles->obtenerBotonAnterior(), &QPushButton::clicked, this, []() {
        qDebug() << "Canción anterior: pendiente";
    });

    // Botón siguiente
    connect(panelControles->obtenerBotonSiguiente(), &QPushButton::clicked, this, []() {
        qDebug() << "Canción siguiente: pendiente";
    });

    // Control de volumen
    connect(panelControles->obtenerControlVolumen(), &QSlider::valueChanged, this, [=](int volumen) {
        controlador->establecerVolumen(volumen / 100.0f);
    });

    // Actualizar el tiempo de reproducción
    QMediaPlayer *reproductor = controlador->obtenerReproductor();
    connect(reproductor, &QMediaPlayer::positionChanged, this, [=](qint64 posicion) {
        panelControles->actualizarTiempo(posicion, controlador->obtenerDuracion());
    });

    connect(reproductor, &QMediaPlayer::durationChanged, this, [=](qint64 duracion) {
        panelControles->actualizarTiempo(controlador->obtenerPosicion(), duracion);
    });

    // Actualizar el icono de reproducción / pausa
    connect(reproductor, &QMediaPlayer::playbackStateChanged, this, [=](QMediaPlayer::PlaybackState estado) {
        panelControles->establecerEstadoReproduccion(estado == QMediaPlayer::PlayingState);
    });

    // Permitir adelantar o retroceder desde la barra de progreso
    connect(panelControles->obtenerBarraProgreso(), &QSlider::sliderMoved, this, [=](int valor) {
        qint64 duracion = controlador->obtenerDuracion();

        if (duracion > 0) {
            qint64 posicion = (duracion * valor) / 1000;
            controlador->establecerPosicion(posicion);
        }
    });

    // Estilos iniciales
    setStyleSheet(R"(
        QMainWindow {
            background-color: #20251B;
        }
        QWidget#contenedorPrincipal {
            background-color: #20251B;
        }
        QLabel#encabezado {
            background-color: #303B27;
            color: #E6E5D7;
            border-bottom: 3px solid #503923;
            font-size: 18px;
            font-weight: bold;
            padding-left: 20px;
        }
        QWidget#panelIzquierdo {
        background-color: #303B27;
        border: 2px solid #503923;
        }
        QFrame#marcoPortada {
        background-color: transparent;
        border: none;
        }
        QLabel#portada {
        background-color: #39452E;
        color: #E6E5D7;
        font-size: 20px;
        font-weight: bold;
        border: 1px solid #503923;
        }
        QWidget#panelDerecho {
        background-color: #292F23;
        border: 2px solid #503923;
        }
    )");
    // Establecer el contenedor principal como el widget central de la ventana
    setCentralWidget(contenedor);
}
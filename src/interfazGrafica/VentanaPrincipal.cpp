#include "VentanaPrincipal.h"
#include "componentes/Portada.h"
#include "paneles/PanelAlbum.h"
#include "paneles/PanelCanciones.h"
#include "paneles/PanelControles.h"
#include "paneles/BarraHerra.h"
#include "reproduccion/ControladoRep.h"
#include "../minero/LectorMetadatos.h"
#include "../minero/ImportarMusica.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QFutureWatcher>
#include <QtConcurrent/QtConcurrentRun>
#include <QDir>
#include <QMediaPlayer>
#include <QPushButton>
#include <QSlider>
#include <QModelIndex>
#include <QImage>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QLineEdit>
#include <Qt>
#include <memory>
#include <filesystem>
#include <exception>

struct ResultadoImportacionUI {
    bool correcto = false;
    ResultadoMinado resultado;
    std::string mensajeError;
};

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

    // Barra de herramientas superior
    BarraHerramientas *barraHerramientas = new BarraHerramientas(contenedor);

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
    layout->addWidget(barraHerramientas);
    layout->addWidget(areaTrabajo, 1);

    // Conectar la barra de búsqueda con el filtro de canciones
    connect(barraHerramientas->obtenerBarraBusqueda(), &QLineEdit::textChanged,
        panelDerecho, &PanelCanciones::establecerTextoBusqueda
    );

    // Importar música desde un directorio
    QToolButton *botonImportar = barraHerramientas->obtenerBotonImportarMusica();
    connect(botonImportar, &QToolButton::clicked, this, [this, botonImportar, panelDerecho]() {

            // Seleccionar el directorio que contiene la música
            QString directorio = QFileDialog::getExistingDirectory(this, "Seleccionar carpeta de música", QDir::homePath());

            if (directorio.isEmpty()) {
                return;
            }

            // Convertir la ruta de Qt a filesystem
            const QByteArray rutaUTF8 = directorio.toUtf8();
            const std::filesystem::path ruta = std::filesystem::u8path(rutaUTF8.constData());

            // Evitar que se inicie otra importación simultáneamente
            botonImportar->setEnabled(false);
            botonImportar->setToolTip("Importando música...");

            // Observar el trabajo en segundo plano
            auto *observador = new QFutureWatcher<ResultadoImportacionUI>(this);
            connect(observador, &QFutureWatcher<ResultadoImportacionUI>::finished, 
                this, [this, observador, panelDerecho, botonImportar]() {
                    ResultadoImportacionUI salida = observador->result();
                    observador->deleteLater();
                    botonImportar->setEnabled(true);
                    botonImportar->setToolTip("Importar música");

                    // Mostrar un error si no se pudo completar la importación
                    if (!salida.correcto) {
                        QMessageBox::critical(this, "Error de importación", QString::fromStdString(salida.mensajeError));
                        return;
                    }

                    // Actualizar la biblioteca visible
                    panelDerecho->recargarCanciones();
                    const ResultadoMinado& r = salida.resultado;

                    if (r.archivosEncontrados == 0) {
                        QMessageBox::information(this, "Importación finalizada",
                            "No se encontraron archivos MP3 en el directorio seleccionado."
                        );
                        return;
                    }
        }
    );

    // Controles inferiores
    PanelControles *panelControles = new PanelControles(contenedor);
    layout->addWidget(panelControles);
    ControladorReproduccion *controlador = new ControladorReproduccion(this);

    // Función para cargar y reproducir una canción
    auto filaActual = std::make_shared<int>(-1);
    auto reproducirFila = [panelDerecho, panelIzquierdo, controlador, filaActual](int fila) {
        int totalCanciones = panelDerecho->obtenerTabla()->model()->rowCount();

        if (fila < 0 || fila >= totalCanciones){
            return;
        }
        *filaActual = fila;
        // Seleccionar la canción que se está reproduciendo
        QTableView *tabla = panelDerecho->obtenerTabla();           
        tabla->selectRow(fila);
        tabla->scrollTo(tabla->model()->index(fila, 0));
        
        QString ruta = panelDerecho->obtenerRutaCancion(fila);
        controlador->cargarCancion(ruta);
        controlador->reproducir();
        LectorMetadatosMP3 lector;
        MetadatosMusicales metadatos;
        std::string error;

        // Leer los metadatos y actualizar la portada
        if (lector.leerArchivo(ruta.toStdString(), metadatos, error)) {
            if (!metadatos.portadaFrontal.datos.empty()) {
                QImage imagen;
                imagen.loadFromData(metadatos.portadaFrontal.datos.data(), 
                static_cast<int>(metadatos.portadaFrontal.datos.size()));
                panelIzquierdo->establecerPortada(imagen);
            } else {
                panelIzquierdo->limpiarPortada();
            }
        }
    };

    // Doble clic para reproducir
    connect(panelDerecho->obtenerTabla(), &QTableView::doubleClicked, this, [reproducirFila](const QModelIndex& indice) {
        reproducirFila(indice.row());
    });

    // Botón reproducir / pausar
    connect(panelControles->obtenerBotonReproducir(), &QPushButton::clicked, this, [=]() {
        if (controlador->estaReproduciendo())
            controlador->pausar();
        else
            controlador->reproducir();
    });

    // Canción anterior
    connect(panelControles->obtenerBotonAnterior(), &QPushButton::clicked, this, [reproducirFila, filaActual]() {
        reproducirFila(*filaActual - 1);
    });  

    // Canción siguiente
    connect(panelControles->obtenerBotonSiguiente(), &QPushButton::clicked, this, [reproducirFila, filaActual]() {
        reproducirFila(*filaActual + 1);
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

    // Reproducir automáticamente la siguiente canción
    connect(controlador->obtenerReproductor(), &QMediaPlayer::mediaStatusChanged, 
    this, [reproducirFila, filaActual](QMediaPlayer::MediaStatus estado) {
        if (estado == QMediaPlayer::EndOfMedia)
            reproducirFila(*filaActual + 1);
    });

    // Estilos iniciales
    setStyleSheet(R"(
        QMainWindow {
            background-color: #20251B;
        }
        QWidget#contenedorPrincipal {
            background-color: #20251B;
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
        QTableView::item:selected {
        background-color: #5E873B;
        color: #E6E5D7;
        }
        QTableView::item:hover {
        background-color: #39452E;
        }
    )");
    // Establecer el contenedor principal como el widget central de la ventana
    setCentralWidget(contenedor);
}
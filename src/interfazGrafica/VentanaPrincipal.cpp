#include "VentanaPrincipal.h"
#include "controladores/ContEditCanciones.h"
#include "controladores/ContEditInterp.h"
#include "controladores/ContEditAlbumes.h"
#include "controladores/ContImport.h"
#include "controladores/ContRepro.h"
#include "paneles/PanelAlbum.h"
#include "paneles/PanelCanciones.h"
#include "paneles/PanelControles.h"
#include "paneles/BarraHerra.h"
#include <QItemSelectionModel>
#include <QToolButton>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableView>
#include <QLineEdit>
#include <QFont>
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

    // Barra de herramientas superior
    BarraHerramientas *barraHerramientas = new BarraHerramientas(contenedor);

    // Reproductor
    QWidget *areaTrabajo = new QWidget(contenedor);
    areaTrabajo->setObjectName("areaTrabajo");
    // Tipografía cómoda para los datos musicales; los encabezados usan una fuente retro
    QFont fuenteContenido("DejaVu Sans", 10);
    areaTrabajo->setFont(fuenteContenido);
    QHBoxLayout *layoutPrincipal = new QHBoxLayout(areaTrabajo);
    // El contenido comienza inmediatamente debajo de la barra superior
    layoutPrincipal->setContentsMargins(12, 0, 12, 0);
    layoutPrincipal->setSpacing(6);

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

    // La galería ocupa toda el área central y la vista de lista conserva los dos paneles
    connect(barraHerramientas->obtenerBotonLista(), &QToolButton::clicked, this,
        [panelIzquierdo, panelDerecho, layoutPrincipal]() {
            panelDerecho->establecerVistaGaleria(false);
            panelIzquierdo->show();
            layoutPrincipal->setStretch(0, 2);
            layoutPrincipal->setStretch(1, 3);
        }
    );

    connect(barraHerramientas->obtenerBotonGaleria(), &QToolButton::clicked, this,
        [panelIzquierdo, panelDerecho, layoutPrincipal]() {
            panelDerecho->establecerVistaGaleria(true);
            panelIzquierdo->hide();
            layoutPrincipal->setStretch(0, 0);
            layoutPrincipal->setStretch(1, 1);
        }
    );

    // Botón para editar la canción seleccionada
    QToolButton *botonEditarCancion = barraHerramientas->obtenerBotonEditarCancion();
    QToolButton *botonEditarInterprete = barraHerramientas->obtenerBotonEditarInterprete();

    // Inicialmente no hay ninguna canción seleccionada
    botonEditarCancion->setEnabled(false);
    // Sin canción seleccionada, este botón permite registrar una persona o grupo
    botonEditarInterprete->setEnabled(true);
    botonEditarInterprete->setToolTip("Agregar persona o grupo");

    // Habilitar o deshabilitar los botones según la selección
    auto actualizarEstadoEdicion = [botonEditarCancion, botonEditarInterprete, panelDerecho]() {
        bool haySeleccion = !panelDerecho->obtenerTabla()->selectionModel()->selectedIndexes().isEmpty();
        botonEditarCancion->setEnabled(haySeleccion);
        botonEditarInterprete->setEnabled(true);
        botonEditarInterprete->setToolTip(haySeleccion
            ? "Editar intérprete de la canción seleccionada"
            : "Agregar persona o grupo");
    };

    connect(panelDerecho->obtenerTabla()->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, [actualizarEstadoEdicion](const QItemSelection&, const QItemSelection&) {
            actualizarEstadoEdicion();
        }
    );

    // Delegar la edición de canciones a ContEditCanciones
    new ControladorEdicionCanciones(botonEditarCancion, panelDerecho, this);

    // Delegar la edición de intérpretes a ContEditInterp
    new ControladorEditInterp(botonEditarInterprete, panelDerecho, this);

    // Administrar los datos de los álbumes sin modificar archivos MP3
    new ControladorEditAlbumes(barraHerramientas->obtenerBotonAdministrarAlbumes(), panelDerecho, this);

    // Delegar la importación de música a ContImport
    QToolButton *botonImportar = barraHerramientas->obtenerBotonImportarMusica();
    new ControladorImport(botonImportar, panelDerecho, this);

    // Delegar la reproducción a un único controlador de interfaz
    PanelControles *panelControles = new PanelControles(contenedor);
    layout->addWidget(panelControles);
    new ControladorRepUI(panelControles, panelIzquierdo, panelDerecho, this);

    // Identidad visual de la ventana principal: verdes de césped y marrones de tierra
    setStyleSheet(R"(
        QMainWindow {
            background-color: #263A27;
        }
        QWidget#contenedorPrincipal {
            background-color: #263A27;
        }
        QWidget#areaTrabajo {
            background-color: #263A27;
        }
        QWidget#panelIzquierdo {
            background-color: #314A30;
            border: none;
        }
        QFrame#marcoPortada {
            background-color: transparent;
            border: none;
        }
        QLabel#portada {
            background-color: #45683A;
            color: #F2EBD5;
            border: 2px solid #956332;
            padding: 2px;
        }
        QWidget#panelDerecho {
            background-color: #314A30;
            border: none;
        }
    )");
    // Establecer el contenedor principal como el widget central de la ventana
    setCentralWidget(contenedor);
}
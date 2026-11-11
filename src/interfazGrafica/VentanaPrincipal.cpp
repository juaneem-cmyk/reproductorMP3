#include "VentanaPrincipal.h"
#include "controladores/ContEditCanciones.h"
#include "controladores/ContEditInterp.h"
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
    botonEditarInterprete->setEnabled(false);

    // Habilitar o deshabilitar los botones según la selección
    auto actualizarEstadoEdicion = [botonEditarCancion, botonEditarInterprete, panelDerecho]() {
        bool haySeleccion = !panelDerecho->obtenerTabla()->selectionModel()->selectedIndexes().isEmpty();
        botonEditarCancion->setEnabled(haySeleccion);
        botonEditarInterprete->setEnabled(haySeleccion);
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

    // Delegar la importación de música a ContImport
    QToolButton *botonImportar = barraHerramientas->obtenerBotonImportarMusica();
    new ControladorImport(botonImportar, panelDerecho, this);

    // Delegar la reproducción a un único controlador de interfaz
    PanelControles *panelControles = new PanelControles(contenedor);
    layout->addWidget(panelControles);
    new ControladorRepUI(panelControles, panelIzquierdo, panelDerecho, this);

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
#include "BarraHerra.h"
#include <QToolButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QCursor>

BarraHerramientas::BarraHerramientas(QWidget *parent) : QWidget(parent) {
    setObjectName("barraHerramientas");
    setFixedHeight(50);

    // Distribución horizontal
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 6, 10, 6);
    layout->setSpacing(7);

    // Botones de edición
    botonEditarCancion = new QToolButton(this);
    botonEditarCancion->setText("♫");
    botonEditarCancion->setToolTip("Editar canción");
    botonEditarInterprete = new QToolButton(this);
    botonEditarInterprete->setText("♙");
    botonEditarInterprete->setToolTip("Editar artista o grupo");

    // Botón para importar música
    botonImportarMusica = new QToolButton(this);
    botonImportarMusica->setText("+");
    botonImportarMusica->setToolTip("Importar música");
    botonAdministrarAlbumes = new QToolButton(this);
    botonAdministrarAlbumes->setText("▤");
    botonAdministrarAlbumes->setToolTip("Administrar álbumes");

    // Barra de búsqueda
    barraBusqueda = new QLineEdit(this);
    barraBusqueda->setPlaceholderText("Buscar canciones, artistas o álbumes...");
    barraBusqueda->setClearButtonEnabled(true);

    // Botones para cambiar la vista
    botonLista = new QToolButton(this);
    botonLista->setText("☰");
    botonLista->setToolTip("Vista de lista");
    botonLista->setCheckable(true);
    botonGaleria = new QToolButton(this);
    botonGaleria->setText("▦");
    botonGaleria->setToolTip("Vista de galería");
    botonGaleria->setCheckable(true);

    // Las vistas son mutuamente excluyentes
    QButtonGroup *grupoVistas = new QButtonGroup(this);
    grupoVistas->setExclusive(true);
    grupoVistas->addButton(botonLista);
    grupoVistas->addButton(botonGaleria);

    // La vista de lista inicia seleccionada
    botonLista->setChecked(true);

    // Configuración de botones
    QToolButton *botones[] = {botonEditarCancion, botonEditarInterprete, botonLista,
        botonGaleria, botonImportarMusica, botonAdministrarAlbumes};

    for (QToolButton *boton : botones) {
        boton->setFixedSize(34, 34);
        boton->setCursor(Qt::PointingHandCursor);
    }
    barraBusqueda->setMinimumHeight(34);

    // Distribución de los elementos
    layout->addWidget(botonImportarMusica);
    layout->addWidget(botonEditarCancion);
    layout->addWidget(botonEditarInterprete);
    layout->addWidget(botonAdministrarAlbumes);
    layout->addWidget(barraBusqueda, 1);
    layout->addWidget(botonLista);
    layout->addWidget(botonGaleria);
    // Estilos
    setStyleSheet(R"(
        QWidget#barraHerramientas {
            background-color: #303B27;
            border-bottom: 2px solid #503923;
        }
        QToolButton {
            background-color: #5E873B;
            color: #E6E5D7;
            border: 1px solid #805638;
            border-radius: 5px;
            font-size: 17px;
            font-weight: bold;
        }
        QToolButton:hover {
            background-color: #86A94F;
        }
        QToolButton:checked {
            background-color: #805638;
            border: 1px solid #E6E5D7;
        }
        QLineEdit {
            background-color: #20251B;
            color: #E6E5D7;
            border: 1px solid #805638;
            border-radius: 5px;
            padding-left: 9px;
            font-size: 13px;
        }
        QLineEdit:focus {
            border: 1px solid #86A94F;
        }
    )");
}

// Obtener controles
QToolButton *BarraHerramientas::obtenerBotonEditarCancion() const {
    return botonEditarCancion;
}

QToolButton *BarraHerramientas::obtenerBotonEditarInterprete() const {
    return botonEditarInterprete;
}

QLineEdit *BarraHerramientas::obtenerBarraBusqueda() const {
    return barraBusqueda;
}

QToolButton *BarraHerramientas::obtenerBotonLista() const {
    return botonLista;
}

QToolButton *BarraHerramientas::obtenerBotonGaleria() const {
    return botonGaleria;
}

QToolButton *BarraHerramientas::obtenerBotonImportarMusica() const {
    return botonImportarMusica;
}

QToolButton *BarraHerramientas::obtenerBotonAdministrarAlbumes() const {
    return botonAdministrarAlbumes;
}
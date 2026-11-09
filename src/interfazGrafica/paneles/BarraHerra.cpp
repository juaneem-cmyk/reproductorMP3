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
    QToolButton *botones[] = {botonEditarCancion, botonEditarInterprete, botonLista, botonGaleria};

    for (QToolButton *boton : botones) {
        boton->setFixedSize(34, 34);
        boton->setCursor(Qt::PointingHandCursor);
    }
    barraBusqueda->setMinimumHeight(34);

    // Distribución de los elementos
    layout->addWidget(botonEditarCancion);
    layout->addWidget(botonEditarInterprete);
    layout->addWidget(barraBusqueda, 1);
    layout->addWidget(botonLista);
    layout->addWidget(botonGaleria);

}
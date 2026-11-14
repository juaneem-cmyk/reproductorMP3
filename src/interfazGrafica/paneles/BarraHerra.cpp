#include "BarraHerra.h"
#include <QToolButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QButtonGroup>
#include <QCursor>
#include <QIcon>
#include <QPixmap>
#include <QSize>
#include <QFont>

namespace {

// Escala los sprites con vecino más próximo para conservar los píxeles definidos
QIcon cargarIconoPixelado(const QString& ruta) {
    const QPixmap original(ruta);
    if (original.isNull()) {
        return QIcon();
    }

    const QPixmap icono = original.scaled(28, 28, Qt::KeepAspectRatio,
        Qt::FastTransformation);
    return QIcon(icono);
}

// Si un sprite no está disponible, conserva un símbolo legible en la casilla
void configurarIcono(QToolButton *boton, const QString& ruta, const QString& respaldo) {
    const QIcon icono = cargarIconoPixelado(ruta);
    if (icono.isNull()) {
        boton->setIcon(QIcon());
        boton->setText(respaldo);
    } else {
        boton->setIcon(icono);
        boton->setText(QString());
    }
}

}

BarraHerramientas::BarraHerramientas(QWidget *parent) : QWidget(parent) {
    setObjectName("barraHerramientas");
    setFixedHeight(50);

    // Distribución horizontal
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 6, 10, 6);
    layout->setSpacing(7);

    // Botones de edición: los iconos se cargan desde los recursos de Qt
    botonEditarCancion = new QToolButton(this);
    configurarIcono(botonEditarCancion, ":/iconos/musica.png", "♫");
    botonEditarCancion->setToolTip("Editar canción");
    botonEditarCancion->setAccessibleName("Editar canción");

    botonEditarInterprete = new QToolButton(this);
    configurarIcono(botonEditarInterprete, ":/iconos/artistas.png", "♟");
    botonEditarInterprete->setToolTip("Editar artista o grupo");
    botonEditarInterprete->setAccessibleName("Editar artista o grupo");

    // Botón para importar música
    botonImportarMusica = new QToolButton(this);
    configurarIcono(botonImportarMusica, ":/iconos/importar.png", "+");
    botonImportarMusica->setToolTip("Importar música");
    botonImportarMusica->setAccessibleName("Importar música");

    botonAdministrarAlbumes = new QToolButton(this);
    configurarIcono(botonAdministrarAlbumes, ":/iconos/albumes.png", "▣");
    botonAdministrarAlbumes->setToolTip("Administrar álbumes");
    botonAdministrarAlbumes->setAccessibleName("Administrar álbumes");

    // Barra de búsqueda
    barraBusqueda = new QLineEdit(this);
    barraBusqueda->setPlaceholderText("Buscar canciones, artistas o álbumes...");
    barraBusqueda->setClearButtonEnabled(true);

    // Botones para cambiar la vista: libro para la lista y marco para la galería
    botonLista = new QToolButton(this);
    configurarIcono(botonLista, ":/iconos/lista.png", "☰");
    botonLista->setToolTip("Vista de lista");
    botonLista->setAccessibleName("Vista de lista");
    botonLista->setCheckable(true);

    botonGaleria = new QToolButton(this);
    configurarIcono(botonGaleria, ":/iconos/galeria.png", "▦");
    botonGaleria->setToolTip("Vista de galería");
    botonGaleria->setAccessibleName("Vista de galería");
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
        boton->setIconSize(QSize(28, 28));
        if (boton->icon().isNull()) {
            boton->setToolButtonStyle(Qt::ToolButtonTextOnly);
            boton->setFont(QFont("DejaVu Sans", 16, QFont::DemiBold));
        } else {
            boton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        }
        boton->setCursor(Qt::PointingHandCursor);
        boton->setAutoRaise(false);
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
    // Estilos: casillas de inventario con relieve y bordes cuadrados
    setStyleSheet(R"(
        QWidget#barraHerramientas {
            background-color: #426536;
            border-bottom: 2px solid #95622F;
        }
        QToolButton {
            background-color: #454545;
            color: #F1EBD8;
            border-top: 2px solid #858585;
            border-left: 2px solid #858585;
            border-right: 2px solid #242424;
            border-bottom: 2px solid #242424;
            border-radius: 0px;
            padding: 0px;
        }
        QToolButton:hover {
            background-color: #5A5A5A;
            border-top: 2px solid #B0B0B0;
            border-left: 2px solid #B0B0B0;
            border-right: 2px solid #303030;
            border-bottom: 2px solid #303030;
        }
        QToolButton:pressed {
            background-color: #343434;
            border-top: 2px solid #242424;
            border-left: 2px solid #242424;
            border-right: 2px solid #858585;
            border-bottom: 2px solid #858585;
            padding-top: 2px;
            padding-left: 2px;
        }
        QToolButton:checked {
            background-color: #526D38;
            border: 2px solid #C3D99A;
        }
        QToolButton:disabled {
            background-color: #393939;
            border-color: #555555;
        }
        QLineEdit {
            background-color: #263B25;
            color: #F1EBD8;
            border: 2px solid #704522;
            border-radius: 0px;
            padding-left: 9px;
            font-size: 13px;
            selection-background-color: #648F3D;
        }
        QLineEdit:focus {
            border: 2px solid #B98B53;
        }
        QToolTip {
            background-color: #292929;
            color: #F1EBD8;
            border: 1px solid #A58A5A;
            padding: 4px 6px;
            border-radius: 0px;
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
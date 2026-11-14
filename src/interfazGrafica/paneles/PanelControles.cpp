#include "PanelControles.h"
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QLabel>
#include <QStringList>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QSize>

PanelControles::PanelControles(QWidget *parent) : QWidget(parent) {
    // Barra de reproducción
    setObjectName("panelControles");
    barraProgreso = new QSlider(Qt::Horizontal, this);
    barraProgreso->setEnabled(false);
    barraProgreso->setObjectName("barraProgreso");
    barraProgreso->setRange(0, 1000);
    barraProgreso->setValue(0);
    barraProgreso->setFixedHeight(10);

    // Botones de reproducción
    botonAnterior = new QPushButton(this);
    botonReproducir = new QPushButton(this);
    botonSiguiente = new QPushButton(this);
    botonAnterior->setIcon(style()->standardIcon(QStyle::SP_MediaSkipBackward));
    botonReproducir->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    botonSiguiente->setIcon(style()->standardIcon(QStyle::SP_MediaSkipForward));
    botonAnterior->setObjectName("botonAnterior");
    botonReproducir->setObjectName("botonReproducir");
    botonSiguiente->setObjectName("botonSiguiente");
    botonAnterior->setIconSize(QSize(20, 20));
    botonReproducir->setIconSize(QSize(25, 25));
    botonSiguiente->setIconSize(QSize(20, 20));

    // Tiempo de reproducción
    tiempoReproduccion = new QLabel("0:00/0:00", this);
    tiempoReproduccion->setObjectName("tiempoReproduccion");

    // Información de la canción
    tituloCancion = new QLabel("Sin reproducción", this);
    detallesCancion = new QLabel("", this);
    tituloCancion->setObjectName("tituloCancion");
    detallesCancion->setObjectName("detallesCancion");

    // Control de volumen
    botonVolumen = new QToolButton(this);
    botonVolumen->setIcon(style()->standardIcon(QStyle::SP_MediaVolume));
    botonVolumen->setObjectName("botonVolumen");
    botonVolumen->setIconSize(QSize(20, 20));
    controlVolumen = new QSlider(Qt::Horizontal, this);
    controlVolumen->setObjectName("controlVolumen");
    controlVolumen->setRange(0, 100);
    controlVolumen->setValue(70);
    controlVolumen->setFixedWidth(100);

    // Distribución izquierda:
    QHBoxLayout *layoutIzquierdo = new QHBoxLayout;
    layoutIzquierdo->setSpacing(8);
    layoutIzquierdo->addWidget(botonAnterior);
    layoutIzquierdo->addWidget(botonReproducir);
    layoutIzquierdo->addWidget(botonSiguiente);
    layoutIzquierdo->addSpacing(8);
    layoutIzquierdo->addWidget(tiempoReproduccion);

    // Distribución central:
    QVBoxLayout *layoutInformacion = new QVBoxLayout;
    layoutInformacion->setSpacing(2);
    tituloCancion->setAlignment(Qt::AlignCenter);
    detallesCancion->setAlignment(Qt::AlignCenter);
    layoutInformacion->addWidget(tituloCancion);
    layoutInformacion->addWidget(detallesCancion);

    // Distribución derecha:
    QHBoxLayout *layoutDerecho = new QHBoxLayout;
    layoutDerecho->setSpacing(8);
    layoutDerecho->addStretch();
    layoutDerecho->addWidget(botonVolumen);
    layoutDerecho->addWidget(controlVolumen);

    // Fila inferior completa
    QHBoxLayout *layoutInferior = new QHBoxLayout;
    layoutInferior->setContentsMargins(18, 6, 18, 8);
    layoutInferior->setSpacing(10);
    layoutInferior->addLayout(layoutIzquierdo, 1);
    layoutInferior->addLayout(layoutInformacion, 2);
    layoutInferior->addLayout(layoutDerecho, 1);

    // Layout principal
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(0, 0, 0, 0);
    layoutPrincipal->setSpacing(0);
    // La línea de progreso queda arriba de los controles y funciona como la división visual
    layoutPrincipal->addWidget(barraProgreso);
    layoutPrincipal->addLayout(layoutInferior);

    // Estilo de inventario: franja de tierra y controles cuadrados con relieve
    setStyleSheet(R"(
        QWidget#panelControles {
            background-color: #684327;
            border-top: 3px solid #B47A43;
            border-left: none;
            border-right: none;
            border-bottom: none;
        }
        QLabel {
            color: #F2EBD5;
            background: transparent;
        }
        QLabel#tituloCancion {
            font-weight: bold;
        }
        QPushButton {
            background-color: #527C37;
            color: #F2EBD5;
            border: 2px solid;
            border-color: #93B968 #314A2B #314A2B #93B968;
            border-radius: 0px;
            padding: 3px 7px;
        }
        QPushButton:hover {
            background-color: #6D9844;
            border-color: #B7D58A #456332 #456332 #B7D58A;
        }
        QPushButton:pressed {
            background-color: #426133;
            border-color: #314A2B #93B968 #93B968 #314A2B;
            padding-top: 5px;
            padding-left: 9px;
        }
        QPushButton:disabled {
            background-color: #46543B;
            color: #A9B19A;
            border-color: #65745A #35432D #35432D #65745A;
        }
        QToolButton#botonVolumen {
            background-color: #527C37;
            color: #F2EBD5;
            border: 2px solid;
            border-color: #93B968 #314A2B #314A2B #93B968;
            border-radius: 0px;
            padding: 3px;
        }
        QToolButton#botonVolumen:hover {
            background-color: #6D9844;
            border-color: #B7D58A #456332 #456332 #B7D58A;
        }
        QToolButton#botonVolumen:pressed {
            background-color: #426133;
            border-color: #314A2B #93B968 #93B968 #314A2B;
        }
        QSlider#barraProgreso {
            background: transparent;
            padding: 0px;
        }
        QSlider#barraProgreso::groove:horizontal {
            height: 5px;
            background-color: #49301F;
            border: 1px solid #9C683B;
        }
        QSlider#barraProgreso::sub-page:horizontal {
            background-color: #C18A4E;
        }
        QSlider#barraProgreso::handle:horizontal {
            width: 12px;
            margin: -4px 0px;
            border: 2px solid #F2EBD5;
            border-radius: 0px;
            background-color: #78A848;
        }
        QSlider#barraProgreso:disabled::groove:horizontal {
            background-color: #49301F;
        }
        QSlider#controlVolumen::groove:horizontal {
            height: 5px;
            background-color: #49301F;
            border: 1px solid #9C683B;
        }
        QSlider#controlVolumen::sub-page:horizontal {
            background-color: #C18A4E;
        }
        QSlider#controlVolumen::handle:horizontal {
            width: 12px;
            margin: -4px 0px;
            border: 2px solid #F2EBD5;
            border-radius: 0px;
            background-color: #78A848;
        }
    )");
}

// Actualizar el tiempo de reproducción
void PanelControles::actualizarTiempo(qint64 posicion, qint64 duracion) {

    // Mantener la barra sincronizada con la posición del reproductor.
    if (duracion > 0) {
        barraProgreso->setEnabled(true);

        // No mover el control mientras el usuario está buscando otra posición
        if (!barraProgreso->isSliderDown()) {
            int valor = static_cast<int>(static_cast<double>(posicion) * barraProgreso->maximum() / duracion
            );
            barraProgreso->setValue(valor);
        }
    } else {
        barraProgreso->setEnabled(false);
        barraProgreso->setValue(0);
    }

    // Convertir milisegundos a segundos
    qint64 segundosActuales = posicion / 1000;
    qint64 segundosTotales = duracion / 1000;

    // Obtener minutos y segundos
    qint64 minutosActuales = segundosActuales / 60;
    qint64 segundosRestantes = segundosActuales % 60;
    qint64 minutosTotales = segundosTotales / 60;
    qint64 segundosTotalesRestantes = segundosTotales % 60;

    // Formatear el tiempo
    QString tiempo = QString("%1:%2/%3:%4").arg(minutosActuales)
    .arg(segundosRestantes, 2, 10, QChar('0')).arg(minutosTotales)
    .arg(segundosTotalesRestantes, 2, 10, QChar('0'));
    tiempoReproduccion->setText(tiempo);
}

// Actualizar el icono del botón de reproducción
void PanelControles::establecerEstadoReproduccion(bool reproduciendo) {

    if (reproduciendo) {
        botonReproducir->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        botonReproducir->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

// Actualizar el icono y la ayuda del control de volumen
void PanelControles::establecerEstadoVolumen(bool silenciado, int volumen) {
    bool sinSonido = silenciado || volumen == 0;
    QStyle::StandardPixmap icono = sinSonido ? QStyle::SP_MediaVolumeMuted : QStyle::SP_MediaVolume;
    botonVolumen->setIcon(style()->standardIcon(icono));
    botonVolumen->setToolTip(silenciado ? "Activar sonido" : "Silenciar");
}

// Actualizar la información de la canción que se está reproduciendo
void PanelControles::establecerInformacionCancion(const QString &titulo, const QString &artista,
    const QString &album, int year) {
    tituloCancion->setText(titulo.isEmpty() ? "Sin título" : titulo);

    // Mostrar los datos disponibles sin dejar separadores vacíos
    QStringList detalles;
    detalles.append(artista.isEmpty() ? "Artista desconocido" : artista);

    if (!album.isEmpty()) {
        detalles.append(album);
    }

    if (year > 0) {
        detalles.append(QString::number(year));
    }
    detallesCancion->setText(detalles.join(" · "));
}

QPushButton *PanelControles::obtenerBotonAnterior() const {
    return botonAnterior;
}

QPushButton *PanelControles::obtenerBotonReproducir() const {
    return botonReproducir;
}

QPushButton *PanelControles::obtenerBotonSiguiente() const {
    return botonSiguiente;
}

QSlider *PanelControles::obtenerBarraProgreso() const {
    return barraProgreso;
}

QSlider *PanelControles::obtenerControlVolumen() const {
    return controlVolumen;
}

QToolButton *PanelControles::obtenerBotonVolumen() const {
    return botonVolumen;
}
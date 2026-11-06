#include "PanelControles.h"
#include <QPushButton>
#include <QToolButton>
#include <QSlider>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QStyle>
#include <QSize>

PanelControles::PanelControles(QWidget *parent) : QWidget(parent) {
    // Barra de reproducción
    setObjectName("panelControles");
    barraProgreso = new QSlider(Qt::Horizontal, this);
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
    layoutPrincipal->addWidget(barraProgreso);
    layoutPrincipal->addLayout(layoutInferior);
}
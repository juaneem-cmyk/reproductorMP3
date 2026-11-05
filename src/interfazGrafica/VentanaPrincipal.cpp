#include "VentanaPrincipal.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QPixmap>
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

    // Panel izquierdo: portada y datos del álbum
    QWidget *panelIzquierdo = new QWidget(areaTrabajo);
    panelIzquierdo->setObjectName("panelIzquierdo");
    // Distribución interna
    QVBoxLayout *layoutIzquierdo = new QVBoxLayout(panelIzquierdo);
    layoutIzquierdo->setContentsMargins(10, 10, 10, 10);    
    layoutIzquierdo->setSpacing(15);

    // Marco de la portada
    QFrame *marcoPortada = new QFrame(panelIzquierdo);
    marcoPortada->setObjectName("marcoPortada");
    marcoPortada->setFixedSize(400, 400);

    // Imagen de portada
    QLabel *portada = new QLabel(marcoPortada);
    portada->setObjectName("portada");
    portada->setAlignment(Qt::AlignCenter);
    portada->setText("PORTADA");

    // Distribución del marco
    QVBoxLayout *layoutPortada = new QVBoxLayout(marcoPortada);
    layoutPortada->setContentsMargins(0, 0, 0, 0);
    layoutPortada->addWidget(portada);

    // Centrar la portada
    layoutIzquierdo->addWidget(marcoPortada, 0, Qt::AlignHCenter);
    layoutIzquierdo->addStretch();

    // Panel derecho: lista de canciones
    QWidget *panelDerecho = new QWidget(areaTrabajo);
    panelDerecho->setObjectName("panelDerecho");
    layoutPrincipal->addWidget(panelIzquierdo, 2);
    layoutPrincipal->addWidget(panelDerecho, 3);

    // Agregar los elementos al contenedor principal
    layout->addWidget(encabezado);
    layout->addWidget(areaTrabajo, 1);

setCentralWidget(contenedor);

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
        background-color: #303B27;
        border: 1px solid #503923;
        }
        QLabel#portada {
        background-color: #39452E;
        color: #E6E5D7;
        font-size: 20px;
        font-weight: bold;
        }
        QWidget#panelDerecho {
        background-color: #292F23;
        border: 2px solid #503923;
        }
    )");
}
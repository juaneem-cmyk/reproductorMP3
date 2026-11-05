#include "VentanaPrincipal.h"
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
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
        QWidget#areaTrabajo {
            background-color: #20251B;
        }
    )");
}
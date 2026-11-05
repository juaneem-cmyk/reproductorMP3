#include "Portada.h"
#include <QLabel>
#include <QResizeEvent>
#include <QSizePolicy>

Portada::Portada(QWidget *parent) : QFrame(parent) {
    setObjectName("marcoPortada");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    portada = new QLabel(this);
    portada->setObjectName("portada");
    portada->setAlignment(Qt::AlignCenter);
    portada->setText("PORTADA");
}

void Portada::resizeEvent(QResizeEvent *evento) {
    QFrame::resizeEvent(evento);    
    // Obtener el tamaño del lado de la portada
    int lado = qMin(width(), height());
    // Centrar la portada dentro del espacio disponible
    int x = (width() - lado) / 2;
    int y = (height() - lado) / 2;
    portada->setGeometry(x, y, lado, lado);
}
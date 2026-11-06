#include "Portada.h"
#include <QLabel>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QImage>
#include <Qt>

// Clase Portada: Representa un marco que muestra la portada de un álbum o una imagen
Portada::Portada(QWidget *parent) : QFrame(parent) {
    setObjectName("marcoPortada");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    portada = new QLabel(this);
    portada->setObjectName("portada");
    portada->setAlignment(Qt::AlignCenter);
    portada->setText("PORTADA");
}

// Establece la imagen de la portada a partir de un QImage
void Portada::establecerImagen(const QImage &imagen) {
    if (imagen.isNull()) {
        limpiarImagen();
        return;
    }
    imagenOriginal = QPixmap::fromImage(imagen);
    actualizarImagen();
}

void Portada::limpiarImagen() {
    imagenOriginal = QPixmap();
    portada->clear();
    portada->setText("PORTADA");
}

void Portada::actualizarImagen() {
    if (imagenOriginal.isNull()) {
        return;
    }
    portada->setText("");
    portada->setPixmap(imagenOriginal.scaled(portada->size(),
    Qt::KeepAspectRatio, Qt::SmoothTransformation)
    );
}

// Maneja el redimensionamiento del marco para ajustar la imagen de la portada
void Portada::resizeEvent(QResizeEvent *evento) {
    QFrame::resizeEvent(evento);
    int lado = qMin(width(), height());
    int x = (width() - lado) / 2;
    int y = (height() - lado) / 2;
    portada->setGeometry(x, y, lado, lado);
    actualizarImagen();
}
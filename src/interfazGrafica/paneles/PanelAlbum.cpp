#include "PanelAlbum.h"
#include "../componentes/Portada.h"
#include <QVBoxLayout>

PanelAlbum::PanelAlbum(QWidget *parent) : QWidget(parent) {
    setObjectName("panelIzquierdo");

    // Distribución interna
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);
    layoutPrincipal->setSpacing(15);
    portada = new Portada(this);
    layoutPrincipal->addWidget(portada, 1);
}

// Establece la imagen de la portada a partir de un QImage
void PanelAlbum::establecerPortada(const QImage &imagen) {
    portada->establecerImagen(imagen);
}

// Limpia la imagen de la portada
void PanelAlbum::limpiarPortada() {
    portada->limpiarImagen();
}
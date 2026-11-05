#include "PanelAlbum.h"
#include "../componentes/Portada.h"
#include <QVBoxLayout>

PanelAlbum::PanelAlbum(QWidget *parent) : QWidget(parent) {
    setObjectName("panelIzquierdo");

    // Distribución interna del panel
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);
    layoutPrincipal->setSpacing(15);

    // Componente de portada
    Portada *portada = new Portada(this);
    layoutPrincipal->addWidget(portada, 1);
}
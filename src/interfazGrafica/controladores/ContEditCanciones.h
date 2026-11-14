#ifndef CONTROLADOR_EDICION_CANCIONES_H
#define CONTROLADOR_EDICION_CANCIONES_H

#include <QObject>

class QToolButton;
class QWidget;
class PanelCanciones;

class ControladorEdicionCanciones : public QObject {
public:
    ControladorEdicionCanciones(QToolButton *botonEditarCancion,
    PanelCanciones *panelCanciones, QWidget *ventanaPadre);

private:
    PanelCanciones *panelCanciones;
    QWidget *ventanaPadre;

    void editarCancion();
};
#endif
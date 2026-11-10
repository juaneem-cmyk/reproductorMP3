#ifndef CONTROLADOR_EDITAR_INTERPRETE_H
#define CONTROLADOR_EDITAR_INTERPRETE_H

#include <QObject>

class QToolButton;
class QWidget;
class PanelCanciones;

class ControladorEditInterp : public QObject {
public:
    ControladorEditInterp(QToolButton *botonEditarInterprete,
    PanelCanciones *panelCanciones, QWidget *ventanaPadre);

private:
    PanelCanciones *panelCanciones;
    QWidget *ventanaPadre;

    void editarInterprete();
};
#endif
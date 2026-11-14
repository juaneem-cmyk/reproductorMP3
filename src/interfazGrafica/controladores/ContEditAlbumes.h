#ifndef CONTROLADOR_EDITAR_ALBUMES_H
#define CONTROLADOR_EDITAR_ALBUMES_H

#include <QObject>

class QToolButton;
class QWidget;
class PanelCanciones;

class ControladorEditAlbumes : public QObject {
public:
    ControladorEditAlbumes(QToolButton *botonAdministrarAlbumes,
        PanelCanciones *panelCanciones, QWidget *ventanaPadre);

private:
    PanelCanciones *panelCanciones;
    QWidget *ventanaPadre;

    void editarAlbumes();
};
#endif
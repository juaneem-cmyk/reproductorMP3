#ifndef CONTROLADOR_REP_UI_H
#define CONTROLADOR_REP_UI_H

#include <QObject>

class PanelAlbum;
class PanelCanciones;
class PanelControles;
class ControladorReproduccion;
class QWidget;

class ControladorRepUI : public QObject {
public:
    ControladorRepUI(PanelControles *panelControles, PanelAlbum *panelAlbum,
    PanelCanciones *panelCanciones, QWidget *ventanaPadre);

private:
    PanelControles *panelControles;
    PanelAlbum *panelAlbum;
    PanelCanciones *panelCanciones;
    ControladorReproduccion *controlador;
    int filaActual;

    void conectarControles();
    void reproducirFila(int fila);
};
#endif
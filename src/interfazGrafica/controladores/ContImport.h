#ifndef CONTROLADOR_IMPORT_H
#define CONTROLADOR_IMPORT_H

#include <QObject>

class QToolButton;
class QWidget;
class PanelCanciones;

class ControladorImport : public QObject {
public:
    ControladorImport(QToolButton *botonImportar, PanelCanciones *panelCanciones, QWidget *ventanaPadre);

private:
    PanelCanciones *panelCanciones;
    QWidget *ventanaPadre;

    void importarMusica(QToolButton *botonImportar);
};
#endif
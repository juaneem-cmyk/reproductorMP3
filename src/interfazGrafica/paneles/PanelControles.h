#ifndef PANEL_CONTROLES_H
#define PANEL_CONTROLES_H

#include <QWidget>

class QPushButton;
class QToolButton;
class QSlider;
class QLabel;

class PanelControles : public QWidget {
public:
    explicit PanelControles(QWidget *parent = nullptr);

private:
    // Barra de reproducción
    QSlider *barraProgreso;

    // Controles de reproducción
    QPushButton *botonAnterior;
    QPushButton *botonReproducir;
    QPushButton *botonSiguiente;

    // Información de la canción
    QLabel *tiempoReproduccion;
    QLabel *tituloCancion;
    QLabel *detallesCancion;

    // Control de volumen
    QToolButton *botonVolumen;
    QSlider *controlVolumen;
};
#endif
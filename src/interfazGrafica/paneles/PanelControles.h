#ifndef PANEL_CONTROLES_H
#define PANEL_CONTROLES_H

#include <QWidget>
#include <QString>

class QPushButton;
class QToolButton;
class QSlider;
class QLabel;

class PanelControles : public QWidget {
public:
    explicit PanelControles(QWidget *parent = nullptr);

    void actualizarTiempo(qint64 posicion, qint64 duracion);
    void establecerEstadoReproduccion(bool reproduciendo);
    void establecerEstadoVolumen(bool silenciado, int volumen);
    void establecerInformacionCancion(const QString &titulo, const QString &artista,
    const QString &album, int year);

    // Métodos para obtener los botones y controles
    QPushButton *obtenerBotonAnterior() const;
    QPushButton *obtenerBotonReproducir() const;
    QPushButton *obtenerBotonSiguiente() const;    
    QSlider *obtenerBarraProgreso() const;
    QSlider *obtenerControlVolumen() const;
    QToolButton *obtenerBotonVolumen() const;

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
#ifndef PORTADA_H
#define PORTADA_H

#include <QFrame>
#include <QPixmap>

class QLabel;
class QResizeEvent;
class Portada : public QFrame {
public:
    explicit Portada(QWidget *parent = nullptr);
    void establecerImagen(const QImage &imagen);
    void limpiarImagen();

protected:
    void resizeEvent(QResizeEvent *evento) override;

private:
    QLabel *portada;
    QPixmap imagenOriginal;
    void actualizarImagen();
};

#endif
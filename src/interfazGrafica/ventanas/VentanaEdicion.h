#ifndef VENTANA_EDICION_H
#define VENTANA_EDICION_H

#include <QDialog>
#include <QString>
#include <QImage>

struct CancionVista;
class QLineEdit;
class QSpinBox;
class QLabel;

class VentanaEdit : public QDialog {
public:
    explicit VentanaEdit(const CancionVista& cancion, QWidget *parent = nullptr);
    int obtenerYear() const;
    bool debeEliminarImagenPersonalizada() const;

    QString obtenerTitulo() const;
    QString obtenerArtista() const;
    QString obtenerAlbum() const;
    QString obtenerRutaImagenSeleccionada() const;

private:
    QLineEdit *campoTitulo;
    QLineEdit *campoArtista;
    QLineEdit *campoAlbum;
    QSpinBox *campoYear;
    QLabel *vistaPortada;
    QImage imagenIncrustada;
    QString rutaImagenSeleccionada;
    
    bool eliminarImagenPersonalizada = false;
    void guardarCambios();
    void seleccionarPortada();
    void restaurarPortadaOriginal();
    void mostrarPortada(const QImage& imagen);
};
#endif
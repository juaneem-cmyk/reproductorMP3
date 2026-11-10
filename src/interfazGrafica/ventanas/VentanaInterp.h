#ifndef VENTANA_INTERPRETE_H
#define VENTANA_INTERPRETE_H

#include <QDialog>
#include <QImage>
#include <QString>

struct CancionVista;
class QComboBox;
class QLineEdit;
class QLabel;
class QWidget;

class VentanaInterprete : public QDialog {
public:
    explicit VentanaInterprete(const CancionVista& cancion, QWidget *parent = nullptr);
    int obtenerTipo() const;
    bool debeEliminarImagenPersonalizada() const;

    QString obtenerNombre() const;
    QString obtenerRutaImagenSeleccionada() const;

private:
    QLineEdit *campoNombre;
    QComboBox *campoTipo;
    QLabel *vistaImagen;
    QWidget *seccionImagen;
    QImage imagenIncrustada;
    QString rutaImagenSeleccionada;
    
    bool eliminarImagenPersonalizada = false;
    void guardarCambios();
    void seleccionarImagen();
    void restaurarImagenOriginal();
    void mostrarImagen(const QImage& imagen);
    void actualizarVisibilidadImagen();
};
#endif
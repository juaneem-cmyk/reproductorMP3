#ifndef VENTANA_INTERPRETE_H
#define VENTANA_INTERPRETE_H

#include <QDialog>
#include <QImage>
#include <QString>

struct CancionVista;
struct DatosPersona;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QLabel;
class QWidget;

class VentanaInterprete : public QDialog {
public:
    explicit VentanaInterprete(const CancionVista& cancion, const DatosPersona& datosPersona, QWidget *parent = nullptr);
    int obtenerTipo() const;
    bool debeEliminarImagenPersonalizada() const;

    QString obtenerNombre() const;
    QString obtenerNombreReal() const;
    QString obtenerFechaNacimiento() const;
    QString obtenerFechaFallecimiento() const;
    QString obtenerRutaImagenSeleccionada() const;

private:
    QLineEdit *campoNombre;
    QComboBox *campoTipo;
    QLabel *etiquetaNombre;
    QLabel *vistaImagen;
    QWidget *seccionImagen;
    QWidget *seccionPersona;
    QLineEdit *campoNombreReal;
    QCheckBox *fechaNacimientoActiva;
    QDateEdit *campoFechaNacimiento;
    QCheckBox *fechaFallecimientoActiva;
    QDateEdit *campoFechaFallecimiento;
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
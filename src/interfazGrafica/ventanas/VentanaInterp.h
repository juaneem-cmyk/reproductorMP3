#ifndef VENTANA_INTERPRETE_H
#define VENTANA_INTERPRETE_H

#include "../../baseDatos/repositorio/EdicionInterp.h"
#include <QDialog>
#include <QImage>
#include <QString>
#include <vector>

struct CancionVista;
class QCheckBox;
class QComboBox;
class QDateEdit;
class QLineEdit;
class QLabel;
class QListWidget;
class QPushButton;
class QWidget;

class VentanaInterprete : public QDialog {
public:
    explicit VentanaInterprete(const std::vector<DatosIntegrante>& personasDisponibles, QWidget *parent = nullptr);
    VentanaInterprete(const CancionVista& cancion, const DatosPersona& datosPersona,
        const DatosGrupo& datosGrupo, const std::vector<DatosIntegrante>& integrantes,
        const std::vector<DatosIntegrante>& personasDisponibles, QWidget *parent = nullptr);
    int obtenerTipo() const;
    bool debeEliminarImagenPersonalizada() const;

    QString obtenerNombre() const;
    QString obtenerNombreReal() const;
    QString obtenerFechaNacimiento() const;
    QString obtenerFechaFallecimiento() const;
    QString obtenerFechaCreacion() const;
    QString obtenerFechaSeparacion() const;
    std::vector<int> obtenerIdsIntegrantes() const;
    QString obtenerRutaImagenSeleccionada() const;

private:
    QLineEdit *campoNombre;
    QComboBox *campoTipo;
    QLabel *etiquetaNombre;
    QLabel *vistaImagen;
    QWidget *seccionImagen;
    QWidget *seccionPersona;
    QWidget *seccionGrupo;
    QLineEdit *campoNombreReal;
    QCheckBox *fechaNacimientoActiva;
    QDateEdit *campoFechaNacimiento;
    QCheckBox *fechaFallecimientoActiva;
    QDateEdit *campoFechaFallecimiento;
    QCheckBox *fechaCreacionActiva;
    QDateEdit *campoFechaCreacion;
    QCheckBox *fechaSeparacionActiva;
    QDateEdit *campoFechaSeparacion;
    QListWidget *listaIntegrantes;
    QLabel *etiquetaCantidadIntegrantes;
    QLabel *etiquetaSinIntegrantes;
    QPushButton *botonQuitarIntegrante;
    std::vector<DatosIntegrante> integrantesGrupo;
    std::vector<DatosIntegrante> personasDisponibles;
    QImage imagenIncrustada;
    QString rutaImagenSeleccionada;
    
    int idInterprete;
    bool nuevoInterprete = false;
    bool eliminarImagenPersonalizada = false;
    VentanaInterprete(const CancionVista& cancion, const DatosPersona& datosPersona,
        const DatosGrupo& datosGrupo, const std::vector<DatosIntegrante>& integrantes,
        const std::vector<DatosIntegrante>& personasDisponibles, QWidget *parent, bool nuevo);
    void guardarCambios();
    void seleccionarImagen();
    void restaurarImagenOriginal();
    void mostrarImagen(const QImage& imagen);
    void actualizarVisibilidadImagen();
    void actualizarListaIntegrantes();
    void agregarIntegrante();
    void quitarIntegrante();
};
#endif
#ifndef VENTANA_INTERPRETE_H
#define VENTANA_INTERPRETE_H

#include <QDialog>
#include <QString>

struct CancionVista;
class QComboBox;
class QLineEdit;

class VentanaInterprete : public QDialog {
public:
    explicit VentanaInterprete(const CancionVista& cancion, QWidget *parent = nullptr);
    int obtenerTipo() const;

    QString obtenerNombre() const;

private:
    QLineEdit *campoNombre;
    QComboBox *campoTipo;

    void guardarCambios();
};
#endif
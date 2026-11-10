#ifndef VENTANA_EDICION_H
#define VENTANA_EDICION_H

#include <QDialog>
#include <QString>

struct CancionVista;
class QLineEdit;
class QSpinBox;

class VentanaEdit : public QDialog {
public:
    explicit VentanaEdit(const CancionVista& cancion, QWidget *parent = nullptr);
    int obtenerYear() const;

    QString obtenerTitulo() const;
    QString obtenerArtista() const;
    QString obtenerAlbum() const;

private:
    QLineEdit *campoTitulo;
    QLineEdit *campoArtista;
    QLineEdit *campoAlbum;
    QSpinBox *campoYear;

    void guardarCambios();
};
#endif
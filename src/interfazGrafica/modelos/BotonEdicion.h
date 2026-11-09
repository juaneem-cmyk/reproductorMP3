#ifndef BOTON_EDICION_H
#define BOTON_EDICION_H

#include <QDialog>
#include <QString>

struct CancionVista;
class QLineEdit;

class BotonEdit : public QDialog {
public:
    explicit BotonEdit(const CancionVista& cancion, QWidget *parent = nullptr);

    QString obtenerTitulo() const;
    QString obtenerArtista() const;
    QString obtenerAlbum() const;

private:
    QLineEdit *campoTitulo;
    QLineEdit *campoArtista;
    QLineEdit *campoAlbum;

    void guardarCambios();
};
#endif
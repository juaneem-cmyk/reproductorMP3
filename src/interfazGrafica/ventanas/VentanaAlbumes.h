#ifndef VENTANA_ALBUMES_H
#define VENTANA_ALBUMES_H

#include "../../baseDatos/repositorio/EdicionAlbumes.h"
#include <QDialog>
#include <vector>

class QTableWidget;
class QPushButton;

class VentanaAlbumes : public QDialog {
public:
    explicit VentanaAlbumes(const std::vector<DatosAlbum>& albumes, QWidget *parent = nullptr);
    
    std::vector<DatosAlbum> obtenerAlbumes() const;

private:
    QTableWidget *tablaAlbumes;
    QPushButton *botonCambiarCarpeta;
    std::vector<DatosAlbum> albumesOriginales;

    void guardarCambios();
    void cambiarCarpeta();
};
#endif
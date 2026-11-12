#include "VentanaAlbumes.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>
#include <utility>

// Clase auxiliar para almacenar los datos de un álbum en la tabla
VentanaAlbumes::VentanaAlbumes(const std::vector<DatosAlbum>& albumes, QWidget *parent)
    : QDialog(parent), tablaAlbumes(new QTableWidget(this)),
    botonCambiarCarpeta(new QPushButton("Cambiar carpeta...", this)), albumesOriginales(albumes) {
    setWindowTitle("Administrar álbumes");
    setMinimumSize(780, 430);
    setModal(true);
}
#include "VentanaInterp.h"
#include "../modelos/Canciones.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

VentanaInterprete::VentanaInterprete(const CancionVista& cancion, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Editar intérprete");
    setMinimumWidth(400);
    setModal(true);
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    QFormLayout *formulario = new QFormLayout();

    // Campo para modificar el nombre
    campoNombre = new QLineEdit(cancion.artista, this);
    campoNombre->setClearButtonEnabled(true);

    // Clasificación del intérprete
    campoTipo = new QComboBox(this);
    campoTipo->addItem("Persona / solista", 0);
    campoTipo->addItem("Grupo", 1);
    campoTipo->addItem("Desconocido", 2);

    int indiceTipo = campoTipo->findData(cancion.idTipoInterprete);
    campoTipo->setCurrentIndex(indiceTipo >= 0 ? indiceTipo : 2);
    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Clasificación:", campoTipo);

    // Información sobre los cambios
    QLabel *informacion = new QLabel(
        "Los cambios se aplicarán a todas las canciones asociadas "
        "con este intérprete.",
        this
    );
    informacion->setWordWrap(true);

    // Botones
    QDialogButtonBox *botones = new QDialogButtonBox(this);
    QPushButton *guardar = botones->addButton("Guardar", QDialogButtonBox::AcceptRole);
    QPushButton *cancelar = botones->addButton("Cancelar", QDialogButtonBox::RejectRole);

    // Organización
    layoutPrincipal->addLayout(formulario);
    layoutPrincipal->addWidget(informacion);
    layoutPrincipal->addWidget(botones);
    connect(guardar, &QPushButton::clicked, this, &VentanaInterprete::guardarCambios);
    connect(cancelar, &QPushButton::clicked, this, &QDialog::reject);

    // Estilos
    setStyleSheet(R"(
        QDialog {
            background-color: #20251B;
            color: #E6E5D7;
        }
        QLabel {
            color: #E6E5D7;
        }
        QLineEdit, QComboBox {
            background-color: #303B27;
            color: #E6E5D7;
            border: 1px solid #805638;
            border-radius: 4px;
            padding: 7px;
        }
        QComboBox QAbstractItemView {
            background-color: #303B27;
            color: #E6E5D7;
            selection-background-color: #5E873B;
        }
        QPushButton {
            background-color: #5E873B;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover {
            background-color: #86A94F;
        }
    )");
}

void VentanaInterprete::guardarCambios() {
    if (campoNombre->text().trimmed().isEmpty()) {
        QMessageBox::warning(this,
            "Dato inválido",
            "El nombre del intérprete no puede estar vacío."
        );
        campoNombre->setFocus();
        return;
    }
    accept();
}

QString VentanaInterprete::obtenerNombre() const {
    return campoNombre->text().trimmed();
}

int VentanaInterprete::obtenerTipo() const {
    return campoTipo->currentData().toInt();
}
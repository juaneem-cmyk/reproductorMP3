#include "../ventanas/VentanaEdicion.h"
#include "../modelos/Canciones.h"
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

VentanaEdit::VentanaEdit(const CancionVista& cancion, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Editar canción");
    setMinimumWidth(400);
    setModal(true);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    QFormLayout *formulario = new QFormLayout();

    // Campos de edición
    campoTitulo = new QLineEdit(cancion.titulo, this);
    campoArtista = new QLineEdit(cancion.artista, this);
    campoAlbum = new QLineEdit(cancion.album, this);
    campoTitulo->setClearButtonEnabled(true);
    campoArtista->setClearButtonEnabled(true);
    campoAlbum->setClearButtonEnabled(true);
    formulario->addRow("Título:", campoTitulo);
    formulario->addRow("Artista:", campoArtista);
    formulario->addRow("Álbum:", campoAlbum);

    // Información sobre la asociación del álbum
    QLabel *informacion = new QLabel(
        "El álbum se asociará únicamente si su nombre coincide "
        "con el de un directorio en la ruta de la canción. "
        "Déjalo vacío para quitar la asociación.",
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

    connect(guardar, &QPushButton::clicked, this, &VentanaEdit::guardarCambios);
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
        QLineEdit {
            background-color: #303B27;
            color: #E6E5D7;
            border: 1px solid #805638;
            border-radius: 4px;
            padding: 7px;
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

void VentanaEdit::guardarCambios() {
    if (campoTitulo->text().trimmed().isEmpty()) {
        QMessageBox::warning(this,
            "Dato inválido",
            "El título de la canción no puede estar vacío."
        );
        campoTitulo->setFocus();
        return;
    }
    accept();
}

QString VentanaEdit::obtenerTitulo() const {
    return campoTitulo->text().trimmed();
}

QString VentanaEdit::obtenerArtista() const {
    return campoArtista->text().trimmed();
}

QString VentanaEdit::obtenerAlbum() const {
    return campoAlbum->text().trimmed();
}
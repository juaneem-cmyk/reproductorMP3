#include "BotonEdicion.h"
#include "../modelos/Canciones.h"
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>

BotonEdit::BotonEdit(const CancionVista& cancion, QWidget *parent) : QDialog(parent) {
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

    connect(guardar, &QPushButton::clicked, this, &BotonEdit::guardarCambios);
    connect(cancelar, &QPushButton::clicked, this, &QDialog::reject);

}
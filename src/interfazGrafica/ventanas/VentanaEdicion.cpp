#include "VentanaEdicion.h"
#include "../modelos/Canciones.h"
#include "../servicios/GestorImagenes.h"
#include "../../minero/LectorMetadatos.h"
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QMessageBox>
#include <QSpinBox>
#include <QFileDialog>
#include <QPixmap>
#include <QHBoxLayout>

VentanaEdit::VentanaEdit(const CancionVista& cancion, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Editar canción");
    setMinimumWidth(430);
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
    campoYear = new QSpinBox(this);
    campoYear->setRange(0, 9999);
    campoYear->setSpecialValueText("Desconocido");
    campoYear->setValue(cancion.year);

    formulario->addRow("Título:", campoTitulo);
    formulario->addRow("Artista:", campoArtista);
    formulario->addRow("Álbum:", campoAlbum);
    formulario->addRow("Año:", campoYear);

    // Cargar la portada incrustada para poder restaurarla posteriormente
    LectorMetadatosMP3 lector;
    MetadatosMusicales metadatos;
    std::string mensajeError;

    if (lector.leerArchivo(cancion.ruta.toStdString(), metadatos, mensajeError)
        && !metadatos.portadaFrontal.datos.empty()) {
        imagenIncrustada.loadFromData(metadatos.portadaFrontal.datos.data(),
            static_cast<int>(metadatos.portadaFrontal.datos.size())
        );
    }
    QImage imagenActual = GestorImagenes::cargarCancion(cancion.idRola);

    if (imagenActual.isNull()) {
        imagenActual = imagenIncrustada;
    }

    // Vista previa y controles de portada
    QLabel *etiquetaPortada = new QLabel("Portada:", this);
    vistaPortada = new QLabel(this);
    vistaPortada->setFixedSize(180, 180);
    vistaPortada->setAlignment(Qt::AlignCenter);
    vistaPortada->setObjectName("vistaPortada");
    mostrarPortada(imagenActual);

    QPushButton *cambiarPortada = new QPushButton("Cambiar portada", this);
    QPushButton *restaurarPortada = new QPushButton("Restaurar original", this);
    QHBoxLayout *botonesPortada = new QHBoxLayout();
    botonesPortada->addWidget(cambiarPortada);
    botonesPortada->addWidget(restaurarPortada);

    // Información sobre la asociación del álbum
    QLabel *informacion = new QLabel(
        "El álbum se asociará únicamente si su nombre coincide "
        "con el de un directorio en la ruta de la canción. "
        "Déjalo vacío para quitar la asociación.",
        this
    );
    informacion->setWordWrap(true);

    // Botones del diálogo
    QDialogButtonBox *botones = new QDialogButtonBox(this);
    QPushButton *guardar = botones->addButton("Guardar", QDialogButtonBox::AcceptRole);
    QPushButton *cancelar = botones->addButton("Cancelar", QDialogButtonBox::RejectRole);

    // Organización
    layoutPrincipal->addLayout(formulario);
    layoutPrincipal->addWidget(etiquetaPortada);
    layoutPrincipal->addWidget(vistaPortada, 0, Qt::AlignHCenter);
    layoutPrincipal->addLayout(botonesPortada);
    layoutPrincipal->addWidget(informacion);
    layoutPrincipal->addWidget(botones);

    connect(cambiarPortada, &QPushButton::clicked, this, &VentanaEdit::seleccionarPortada);
    connect(restaurarPortada, &QPushButton::clicked, this, &VentanaEdit::restaurarPortadaOriginal);
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
        QLabel#vistaPortada {
            background-color: #303B27;
            border: 1px solid #805638;
        }
        QLineEdit, QSpinBox {
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

void VentanaEdit::seleccionarPortada() {
    const QString ruta = QFileDialog::getOpenFileName(this,
        "Seleccionar portada",
        QString(),
        "Imágenes (*.png *.jpg *.jpeg *.bmp *.webp)"
    );

    if (ruta.isEmpty()) {
        return;
    }
    QImage imagen(ruta);

    if (imagen.isNull()) {
        QMessageBox::warning(this, "Imagen inválida", "No se pudo abrir la imagen seleccionada.");
        return;
    }
    rutaImagenSeleccionada = ruta;
    eliminarImagenPersonalizada = false;
    mostrarPortada(imagen);
}

void VentanaEdit::restaurarPortadaOriginal() {
    rutaImagenSeleccionada.clear();
    eliminarImagenPersonalizada = true;
    mostrarPortada(imagenIncrustada);
}

void VentanaEdit::mostrarPortada(const QImage& imagen) {
    if (imagen.isNull()) {
        vistaPortada->setPixmap(QPixmap());
        vistaPortada->setText("Sin portada");
        return;
    }
    vistaPortada->setText(QString());
    vistaPortada->setPixmap(QPixmap::fromImage(imagen).scaled(vistaPortada->size(), 
    Qt::KeepAspectRatio, Qt::SmoothTransformation
    ));
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

int VentanaEdit::obtenerYear() const {
    return campoYear->value();
}

QString VentanaEdit::obtenerRutaImagenSeleccionada() const {
    return rutaImagenSeleccionada;
}

bool VentanaEdit::debeEliminarImagenPersonalizada() const {
    return eliminarImagenPersonalizada;
}
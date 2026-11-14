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
#include <QGroupBox>

VentanaEdit::VentanaEdit(const CancionVista& cancion, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Editar canción");
    setMinimumSize(700, 400);
    resize(760, 440);
    setModal(true);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(14, 14, 14, 14);
    layoutPrincipal->setSpacing(10);
    QHBoxLayout *columnas = new QHBoxLayout();
    columnas->setContentsMargins(0, 0, 0, 0);
    columnas->setSpacing(14);
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

    QGroupBox *grupoInformacion = new QGroupBox("Información de la canción", this);
    QVBoxLayout *layoutInformacion = new QVBoxLayout(grupoInformacion);
    layoutInformacion->addLayout(formulario);

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
    QGroupBox *grupoPortada = new QGroupBox("Portada", this);
    QVBoxLayout *layoutPortada = new QVBoxLayout(grupoPortada);
    vistaPortada = new QLabel(grupoPortada);
    vistaPortada->setFixedSize(190, 190);
    vistaPortada->setAlignment(Qt::AlignCenter);
    vistaPortada->setObjectName("vistaPortada");
    mostrarPortada(imagenActual);

    QPushButton *cambiarPortada = new QPushButton("Cambiar portada", grupoPortada);
    QPushButton *restaurarPortada = new QPushButton("Restaurar original", grupoPortada);
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
    layoutInformacion->addWidget(informacion);
    layoutInformacion->addStretch();

    layoutPortada->setContentsMargins(12, 14, 12, 12);
    layoutPortada->setSpacing(12);
    layoutPortada->addWidget(vistaPortada, 0, Qt::AlignHCenter);
    layoutPortada->addLayout(botonesPortada);

    // Botones del diálogo
    QDialogButtonBox *botones = new QDialogButtonBox(this);
    QPushButton *guardar = botones->addButton("Guardar", QDialogButtonBox::AcceptRole);
    QPushButton *cancelar = botones->addButton("Cancelar", QDialogButtonBox::RejectRole);

    // Organización: datos a la izquierda y portada a la derecha
    columnas->addWidget(grupoInformacion, 3);
    columnas->addWidget(grupoPortada, 2);
    layoutPrincipal->addLayout(columnas, 1);
    layoutPrincipal->addWidget(botones);

    connect(cambiarPortada, &QPushButton::clicked, this, &VentanaEdit::seleccionarPortada);
    connect(restaurarPortada, &QPushButton::clicked, this, &VentanaEdit::restaurarPortadaOriginal);
    connect(guardar, &QPushButton::clicked, this, &VentanaEdit::guardarCambios);
    connect(cancelar, &QPushButton::clicked, this, &QDialog::reject);

    // Estética de madera y tocadiscos
    setStyleSheet(R"(
        QDialog {
            background-color: #2B1D13;
            color: #F0DFC5;
        }
        QLabel {
            color: #F0DFC5;
            background: transparent;
        }
        QGroupBox {
            color: #F5D9AD;
            background-color: #3B281B;
            border: 2px solid #80522F;
            border-top-color: #B4844B;
            border-left-color: #B4844B;
            border-radius: 0px;
            margin-top: 12px;
            padding: 10px 8px 8px 8px;
            font-weight: bold;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 6px;
            color: #E6BD82;
            font-family: "Press Start 2P";
            font-size: 8px;
        }
        QLabel#vistaPortada {
            background-color: #24180F;
            border: 3px solid #98683B;
            border-top-color: #C0945B;
            border-left-color: #C0945B;
        }
        QLineEdit, QSpinBox {
            background-color: #2D2016;
            color: #F6E7D0;
            border: 2px solid #80522F;
            border-top-color: #B4844B;
            border-left-color: #B4844B;
            border-radius: 0px;
            padding: 7px;
            selection-background-color: #A66E37;
            selection-color: #FFFFFF;
        }
        QLineEdit:focus, QSpinBox:focus {
            border: 2px solid #D0A064;
        }
        QPushButton {
            background-color: #80522F;
            color: #FFF0D8;
            border: 2px solid #C0945B;
            border-right-color: #4C301C;
            border-bottom-color: #4C301C;
            border-radius: 0px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #A66E37;
            border-color: #E0B77A;
        }
        QPushButton:pressed {
            background-color: #5D3A21;
            border-top-color: #4C301C;
            border-left-color: #4C301C;
            border-right-color: #C0945B;
            border-bottom-color: #C0945B;
            padding-top: 9px;
            padding-left: 17px;
        }
        QDialogButtonBox QPushButton {
            min-width: 92px;
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
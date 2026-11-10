#include "VentanaInterp.h"
#include "../modelos/Canciones.h"
#include "../servicios/GestorImagenes.h"
#include "../../minero/LectorMetadatos.h"
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

VentanaInterprete::VentanaInterprete(const CancionVista& cancion, QWidget *parent) : QDialog(parent) {
    setWindowTitle("Editar intérprete");
    setMinimumWidth(430);
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

    // Leer la imagen de artista incrustada en la canción seleccionada
    LectorMetadatosMP3 lector;
    MetadatosMusicales metadatos;
    std::string mensajeError;

    if (lector.leerArchivo(cancion.ruta.toStdString(), metadatos, mensajeError)
        && !metadatos.imagenArtista.datos.empty()) {
        imagenIncrustada.loadFromData(metadatos.imagenArtista.datos.data(),
            static_cast<int>(metadatos.imagenArtista.datos.size())
        );
    }

    // Priorizar la imagen personalizada y después la incrustada
    QImage imagenActual = GestorImagenes::cargarInterprete(cancion.idInterprete);
    if (imagenActual.isNull()) {
        imagenActual = imagenIncrustada;
    }
    QLabel *etiquetaImagen = new QLabel("Imagen del intérprete:", this);
    vistaImagen = new QLabel(this);
    vistaImagen->setFixedSize(180, 180);
    vistaImagen->setAlignment(Qt::AlignCenter);
    vistaImagen->setObjectName("vistaImagenInterprete");
    mostrarImagen(imagenActual);

    QPushButton *cambiarImagen = new QPushButton("Cambiar imagen", this);
    QPushButton *restaurarImagen = new QPushButton("Restaurar original", this);
    QHBoxLayout *botonesImagen = new QHBoxLayout();
    botonesImagen->addWidget(cambiarImagen);
    botonesImagen->addWidget(restaurarImagen);

    // Agrupar los controles de imagen para ocultarlos cuando el intérprete es desconocido
    seccionImagen = new QWidget(this);
    QVBoxLayout *layoutImagen = new QVBoxLayout(seccionImagen);
    layoutImagen->setContentsMargins(0, 0, 0, 0);
    layoutImagen->setSpacing(12);
    layoutImagen->addWidget(etiquetaImagen);
    layoutImagen->addWidget(vistaImagen, 0, Qt::AlignHCenter);
    layoutImagen->addLayout(botonesImagen);

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
    layoutPrincipal->addWidget(seccionImagen);
    layoutPrincipal->addWidget(informacion);
    layoutPrincipal->addWidget(botones);

    connect(cambiarImagen, &QPushButton::clicked, this, &VentanaInterprete::seleccionarImagen);
    connect(restaurarImagen, &QPushButton::clicked, this, &VentanaInterprete::restaurarImagenOriginal);
    connect(campoTipo, qOverload<int>(&QComboBox::currentIndexChanged),
    this, [this](int) { actualizarVisibilidadImagen(); });
    actualizarVisibilidadImagen();
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
        QLabel#vistaImagenInterprete {
            background-color: #303B27;
            border: 1px solid #805638;
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

void VentanaInterprete::seleccionarImagen() {
    const QString ruta = QFileDialog::getOpenFileName(this,
        "Seleccionar imagen del intérprete",
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
    mostrarImagen(imagen);
}

void VentanaInterprete::restaurarImagenOriginal() {
    rutaImagenSeleccionada.clear();
    eliminarImagenPersonalizada = true;
    mostrarImagen(imagenIncrustada);
}

void VentanaInterprete::actualizarVisibilidadImagen() {
    // Los intérpretes desconocidos solo conservan los campos generales
    seccionImagen->setVisible(campoTipo->currentData().toInt() != 2);
}

void VentanaInterprete::mostrarImagen(const QImage& imagen) {
    if (imagen.isNull()) {
        vistaImagen->setPixmap(QPixmap());
        vistaImagen->setText("Sin imagen");
        return;
    }
    vistaImagen->setText(QString());
    vistaImagen->setPixmap(QPixmap::fromImage(imagen).scaled(vistaImagen->size(), 
    Qt::KeepAspectRatio, Qt::SmoothTransformation
    ));
}

QString VentanaInterprete::obtenerNombre() const {
    return campoNombre->text().trimmed();
}

int VentanaInterprete::obtenerTipo() const {
    return campoTipo->currentData().toInt();
}

QString VentanaInterprete::obtenerRutaImagenSeleccionada() const {
    return rutaImagenSeleccionada;
}

bool VentanaInterprete::debeEliminarImagenPersonalizada() const {
    return eliminarImagenPersonalizada;
}
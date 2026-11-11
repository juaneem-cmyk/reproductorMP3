#include "VentanaInterp.h"
#include "../../baseDatos/repositorio/EdicionInterp.h"
#include "../modelos/Canciones.h"
#include "../servicios/GestorImagenes.h"
#include "../../minero/LectorMetadatos.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
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

// Funciones auxiliares para manejar fechas y crear campos de fecha con casilla de verificación
namespace {
void cargarFecha(const std::string& valor, QCheckBox *activa, QDateEdit *campo) {
    const QDate fecha = QDate::fromString(QString::fromStdString(valor), Qt::ISODate);
    if (fecha.isValid()) {
        campo->setDate(fecha);
        activa->setChecked(true);
    } else {
        campo->setDate(QDate::currentDate());
        activa->setChecked(false);
    }
}

QWidget* crearCampoFecha(QCheckBox *&activa, QDateEdit *&campo, QWidget *parent) {
    QWidget *contenedor = new QWidget(parent);
    QHBoxLayout *layout = new QHBoxLayout(contenedor);
    layout->setContentsMargins(0, 0, 0, 0);
    activa = new QCheckBox("Indicar fecha", contenedor);
    campo = new QDateEdit(QDate::currentDate(), contenedor);
    campo->setCalendarPopup(true);
    campo->setDisplayFormat("dd/MM/yyyy");
    campo->setEnabled(false);

    QObject::connect(activa, &QCheckBox::toggled, campo, &QDateEdit::setEnabled);
    layout->addWidget(activa);
    layout->addWidget(campo, 1);
    return contenedor;
}
}

VentanaInterprete::VentanaInterprete(const CancionVista& cancion, const DatosPersona& datosPersona, 
    QWidget *parent) : QDialog(parent) {
    setWindowTitle("Editar intérprete");
    setMinimumWidth(430);
    setModal(true);

    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    QFormLayout *formulario = new QFormLayout();

    // El nombre general también funciona como nombre artístico para personas
    etiquetaNombre = new QLabel("Nombre:", this);
    campoNombre = new QLineEdit(cancion.artista, this);
    campoNombre->setClearButtonEnabled(true);
    campoTipo = new QComboBox(this);
    campoTipo->addItem("Persona / solista", 0);
    campoTipo->addItem("Grupo", 1);
    campoTipo->addItem("Desconocido", 2);
    const int indiceTipo = campoTipo->findData(cancion.idTipoInterprete);
    campoTipo->setCurrentIndex(indiceTipo >= 0 ? indiceTipo : 2);
    formulario->addRow(etiquetaNombre, campoNombre);
    formulario->addRow("Clasificación:", campoTipo);

    // Datos específicos de una persona o solista
    seccionPersona = new QWidget(this);
    QFormLayout *formularioPersona = new QFormLayout(seccionPersona);
    formularioPersona->setContentsMargins(0, 0, 0, 0);
    campoNombreReal = new QLineEdit(QString::fromStdString(datosPersona.nombreReal), this);
    campoNombreReal->setClearButtonEnabled(true);
    formularioPersona->addRow("Nombre real:", campoNombreReal);

    QWidget *filaNacimiento = crearCampoFecha(fechaNacimientoActiva, campoFechaNacimiento, seccionPersona);
    QWidget *filaFallecimiento = crearCampoFecha(fechaFallecimientoActiva, campoFechaFallecimiento, seccionPersona);
    cargarFecha(datosPersona.fechaNacimiento, fechaNacimientoActiva, campoFechaNacimiento);
    cargarFecha(datosPersona.fechaFallecimiento, fechaFallecimientoActiva, campoFechaFallecimiento);
    formularioPersona->addRow("Nacimiento:", filaNacimiento);
    formularioPersona->addRow("Fallecimiento:", filaFallecimiento);

    // Leer el marco APIC de tipo Artist de la canción seleccionada
    LectorMetadatosMP3 lector;
    MetadatosMusicales metadatos;
    std::string mensajeError;

    if (lector.leerArchivo(cancion.ruta.toStdString(), metadatos, mensajeError)
        && !metadatos.imagenArtista.datos.empty()) {
        imagenIncrustada.loadFromData(metadatos.imagenArtista.datos.data(), 
        static_cast<int>(metadatos.imagenArtista.datos.size()));
    }

    // El performer genérico "Unknown" puede representar artistas distintos
    const bool esDesconocidoGenerico = cancion.idTipoInterprete == 2 
    && cancion.artista.trimmed().compare("Unknown", Qt::CaseInsensitive) == 0;

    // Priorizar la imagen personalizada y después la incrustada
    QImage imagenActual;

    if (!esDesconocidoGenerico) {
        imagenActual = GestorImagenes::cargarInterprete(cancion.idInterprete);
    }

    if (imagenActual.isNull()) {
        imagenActual = imagenIncrustada;
    }
    QLabel *etiquetaImagen = new QLabel("Imagen del intérprete:", this);
    vistaImagen = new QLabel(this);
    vistaImagen->setFixedSize(160, 160);
    vistaImagen->setAlignment(Qt::AlignCenter);
    vistaImagen->setObjectName("vistaImagenInterprete");
    mostrarImagen(imagenActual);

    QPushButton *cambiarImagen = new QPushButton("Cambiar imagen", this);
    QPushButton *restaurarImagen = new QPushButton("Restaurar original", this);
    QHBoxLayout *botonesImagen = new QHBoxLayout();
    botonesImagen->addWidget(cambiarImagen);
    botonesImagen->addWidget(restaurarImagen);
    seccionImagen = new QWidget(this);
    QVBoxLayout *layoutImagen = new QVBoxLayout(seccionImagen);
    layoutImagen->setContentsMargins(0, 0, 0, 0);
    layoutImagen->setSpacing(16);
    layoutImagen->addWidget(etiquetaImagen);
    layoutImagen->addWidget(vistaImagen, 0, Qt::AlignHCenter);
    layoutImagen->addLayout(botonesImagen);

    QLabel *informacion = new QLabel(this);

    if (esDesconocidoGenerico) {
        informacion->setText(
            "Si personalizas este intérprete, esta canción se separará de las "
            "demás canciones que todavía aparecen como Unknown.");
    } else {
        informacion->setText(
            "Los cambios se aplicarán a todas las canciones asociadas "
            "con este intérprete.");
    }
    informacion->setWordWrap(true);
    QDialogButtonBox *botones = new QDialogButtonBox(this);
    QPushButton *guardar = botones->addButton("Guardar", QDialogButtonBox::AcceptRole);
    QPushButton *cancelar = botones->addButton("Cancelar", QDialogButtonBox::RejectRole);

    layoutPrincipal->addLayout(formulario);
    layoutPrincipal->addWidget(seccionPersona);
    layoutPrincipal->addWidget(seccionImagen);
    layoutPrincipal->addWidget(informacion);
    layoutPrincipal->addWidget(botones);

    connect(cambiarImagen, &QPushButton::clicked, this, &VentanaInterprete::seleccionarImagen);
    connect(restaurarImagen, &QPushButton::clicked, this, &VentanaInterprete::restaurarImagenOriginal);
    connect(campoTipo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { actualizarVisibilidadImagen(); });
    actualizarVisibilidadImagen();
    connect(guardar, &QPushButton::clicked, this, &VentanaInterprete::guardarCambios);
    connect(cancelar, &QPushButton::clicked, this, &QDialog::reject);

    setStyleSheet(R"(
        QDialog { background-color: #20251B; color: #E6E5D7; }
        QLabel { color: #E6E5D7; }
        QLabel#vistaImagenInterprete {
            background-color: #303B27;
            border: 1px solid #805638;
        }
        QLineEdit, QComboBox, QDateEdit {
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
        QCheckBox { color: #E6E5D7; }
        QPushButton {
            background-color: #5E873B;
            color: white;
            border: none;
            border-radius: 4px;
            padding: 8px 16px;
        }
        QPushButton:hover { background-color: #86A94F; }
    )");
}

void VentanaInterprete::guardarCambios() {
    if (campoNombre->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Dato inválido",
            "El nombre del intérprete no puede estar vacío.");
        campoNombre->setFocus();
        return;
    }

    if (obtenerTipo() == 0) {
        const QDate hoy = QDate::currentDate();
        const bool nacimientoValido = !fechaNacimientoActiva->isChecked() || campoFechaNacimiento->date() <= hoy;
        const bool fallecimientoValido = !fechaFallecimientoActiva->isChecked() || campoFechaFallecimiento->date() <= hoy;
        const bool ordenValido = !fechaNacimientoActiva->isChecked() || !fechaFallecimientoActiva->isChecked()
        || campoFechaFallecimiento->date() >= campoFechaNacimiento->date();

        if (!nacimientoValido || !fallecimientoValido || !ordenValido) {
            QMessageBox::warning(this, "Fechas inválidas",
                "Comprueba las fechas: no pueden estar en el futuro y "
                "el fallecimiento no puede ser anterior al nacimiento.");
            return;
        }
    }
    accept();
}

void VentanaInterprete::seleccionarImagen() {
    const QString ruta = QFileDialog::getOpenFileName(this,
        "Seleccionar imagen del intérprete", QString(),
        "Imágenes (*.png *.jpg *.jpeg *.bmp *.webp)");

    if (ruta.isEmpty()) {
        return;
    }
    QImage imagen(ruta);

    if (imagen.isNull()) {
        QMessageBox::warning(this, "Imagen inválida",
            "No se pudo abrir la imagen seleccionada.");
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
    const int tipo = campoTipo->currentData().toInt();
    etiquetaNombre->setText(tipo == 0 ? "Nombre artístico:" : "Nombre:");
    seccionPersona->setVisible(tipo == 0);
    seccionImagen->setVisible(tipo != 2);
}

void VentanaInterprete::mostrarImagen(const QImage& imagen) {
    if (imagen.isNull()) {
        vistaImagen->setPixmap(QPixmap());
        vistaImagen->setText("Sin imagen");
        return;
    }
    vistaImagen->setText(QString());
    vistaImagen->setPixmap(QPixmap::fromImage(imagen).scaled(vistaImagen->size(), 
    Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

QString VentanaInterprete::obtenerNombre() const {
    return campoNombre->text().trimmed();
}

QString VentanaInterprete::obtenerNombreReal() const {
    return campoNombreReal->text().trimmed();
}

QString VentanaInterprete::obtenerFechaNacimiento() const {
    return fechaNacimientoActiva->isChecked() ? campoFechaNacimiento->date().toString(Qt::ISODate) : QString();
}

QString VentanaInterprete::obtenerFechaFallecimiento() const {
    return fechaFallecimientoActiva->isChecked() ? campoFechaFallecimiento->date().toString(Qt::ISODate) : QString();
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
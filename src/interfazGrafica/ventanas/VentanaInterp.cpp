#include "VentanaInterp.h"
#include "../../baseDatos/repositorio/EdicionInterp.h"
#include "../modelos/Canciones.h"
#include "../servicios/GestorImagenes.h"
#include "../../minero/LectorMetadatos.h"
#include <algorithm>
#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QAbstractItemView>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
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
    const DatosGrupo& datosGrupo, const std::vector<DatosIntegrante>& integrantes,
    const std::vector<DatosIntegrante>& personasDisponibles, QWidget *parent)
    : QDialog(parent), idInterprete(cancion.idInterprete),
    integrantesGrupo(integrantes), personasDisponibles(personasDisponibles) {
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

    // Datos específicos de un grupo
    QGroupBox *informacionGrupo = new QGroupBox("Información del grupo", this);
    seccionGrupo = informacionGrupo;
    QVBoxLayout *layoutGrupo = new QVBoxLayout(informacionGrupo);
    QFormLayout *formularioGrupo = new QFormLayout();
    QWidget *filaCreacion = crearCampoFecha(fechaCreacionActiva, campoFechaCreacion, informacionGrupo);
    QWidget *filaSeparacion = crearCampoFecha(fechaSeparacionActiva, campoFechaSeparacion, informacionGrupo);
    cargarFecha(datosGrupo.fechaCreacion, fechaCreacionActiva, campoFechaCreacion);
    cargarFecha(datosGrupo.fechaSeparacion, fechaSeparacionActiva, campoFechaSeparacion);
    formularioGrupo->addRow("Fecha de creación:", filaCreacion);
    formularioGrupo->addRow("Fecha de separación:", filaSeparacion);
    layoutGrupo->addLayout(formularioGrupo);

    // Personas asociadas al grupo
    QGroupBox *seccionIntegrantes = new QGroupBox("Integrantes", informacionGrupo);
    QVBoxLayout *layoutIntegrantes = new QVBoxLayout(seccionIntegrantes);
    QLabel *descripcionIntegrantes = new QLabel("Personas asociadas al grupo", seccionIntegrantes);
    etiquetaCantidadIntegrantes = new QLabel(seccionIntegrantes);
    QHBoxLayout *encabezadoIntegrantes = new QHBoxLayout();
    encabezadoIntegrantes->addWidget(descripcionIntegrantes);
    encabezadoIntegrantes->addStretch();
    encabezadoIntegrantes->addWidget(etiquetaCantidadIntegrantes);
    listaIntegrantes = new QListWidget(seccionIntegrantes);
    listaIntegrantes->setSelectionMode(QAbstractItemView::SingleSelection);
    listaIntegrantes->setMaximumHeight(110);
    etiquetaSinIntegrantes = new QLabel("Todavía no hay integrantes.", seccionIntegrantes);
    etiquetaSinIntegrantes->setAlignment(Qt::AlignCenter);
    QPushButton *botonAgregarIntegrante = new QPushButton("Agregar integrante", seccionIntegrantes);
    botonQuitarIntegrante = new QPushButton("Quitar seleccionado", seccionIntegrantes);
    botonQuitarIntegrante->setEnabled(false);
    QHBoxLayout *botonesIntegrantes = new QHBoxLayout();
    botonesIntegrantes->addWidget(botonAgregarIntegrante);
    botonesIntegrantes->addWidget(botonQuitarIntegrante);
    layoutIntegrantes->addLayout(encabezadoIntegrantes);
    layoutIntegrantes->addWidget(listaIntegrantes);
    layoutIntegrantes->addWidget(etiquetaSinIntegrantes);
    layoutIntegrantes->addLayout(botonesIntegrantes);
    layoutGrupo->addWidget(seccionIntegrantes);
    actualizarListaIntegrantes();

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
    layoutPrincipal->addWidget(seccionGrupo);
    layoutPrincipal->addWidget(seccionImagen);
    layoutPrincipal->addWidget(informacion);
    layoutPrincipal->addWidget(botones);

    connect(cambiarImagen, &QPushButton::clicked, this, &VentanaInterprete::seleccionarImagen);
    connect(restaurarImagen, &QPushButton::clicked, this, &VentanaInterprete::restaurarImagenOriginal);
    connect(campoTipo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this](int) { actualizarVisibilidadImagen(); });
    connect(botonAgregarIntegrante, &QPushButton::clicked, this, &VentanaInterprete::agregarIntegrante);
    connect(botonQuitarIntegrante, &QPushButton::clicked, this, &VentanaInterprete::quitarIntegrante);
    connect(listaIntegrantes, &QListWidget::itemSelectionChanged, this, [this]() {
        botonQuitarIntegrante->setEnabled(listaIntegrantes->currentItem() != nullptr);
    });
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
        QLineEdit, QComboBox, QDateEdit, QListWidget {
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
        QListWidget::item:selected {
            background-color: #5E873B;
            color: white;
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
    } else if (obtenerTipo() == 1) {
        const QDate hoy = QDate::currentDate();
        const bool creacionValida = !fechaCreacionActiva->isChecked() || campoFechaCreacion->date() <= hoy;
        const bool separacionValida = !fechaSeparacionActiva->isChecked() || campoFechaSeparacion->date() <= hoy;
        const bool ordenValido = !fechaCreacionActiva->isChecked() || !fechaSeparacionActiva->isChecked()
        || campoFechaSeparacion->date() >= campoFechaCreacion->date();

        if (!creacionValida || !separacionValida || !ordenValido) {
            QMessageBox::warning(this, "Fechas inválidas",
                "Comprueba las fechas: no pueden estar en el futuro y "
                "la separación no puede ser anterior a la creación.");
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
    etiquetaNombre->setText(tipo == 0 ? "Nombre artístico:" : tipo == 1 ? "Nombre del grupo:" : "Nombre:");
    seccionPersona->setVisible(tipo == 0);
    seccionGrupo->setVisible(tipo == 1);
    seccionImagen->setVisible(tipo != 2);
}

void VentanaInterprete::actualizarListaIntegrantes() {
    listaIntegrantes->clear();

    for (const DatosIntegrante& integrante : integrantesGrupo) {
        listaIntegrantes->addItem(QString::fromStdString(integrante.nombre));
    }
    const int cantidad = static_cast<int>(integrantesGrupo.size());
    etiquetaCantidadIntegrantes->setText(cantidad == 1 ? QStringLiteral("1 integrante") 
    : QString("%1 integrantes").arg(cantidad));
    listaIntegrantes->setVisible(cantidad > 0);
    etiquetaSinIntegrantes->setVisible(cantidad == 0);
    botonQuitarIntegrante->setEnabled(false);
}

void VentanaInterprete::agregarIntegrante() {
    std::vector<DatosIntegrante> disponibles;

    for (const DatosIntegrante& persona : personasDisponibles) {
        if (persona.idPersona == idInterprete) {
            continue;
        }

        const bool yaAgregado = std::any_of(integrantesGrupo.begin(), integrantesGrupo.end(),
            [&persona](const DatosIntegrante& integrante) {
                return integrante.idPersona == persona.idPersona;
            });

        if (!yaAgregado) {
            disponibles.push_back(persona);
        }
    }

    if (disponibles.empty()) {
        QMessageBox::information(this, "Sin personas disponibles",
            "No hay más personas registradas que se puedan agregar a este grupo.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Agregar integrante");
    dialog.setMinimumWidth(340);
    QVBoxLayout *layout = new QVBoxLayout(&dialog);
    QLabel *descripcion = new QLabel("Selecciona una persona:", &dialog);
    QLineEdit *filtro = new QLineEdit(&dialog);
    filtro->setPlaceholderText("Buscar persona...");
    QListWidget *lista = new QListWidget(&dialog);

    for (const DatosIntegrante& persona : disponibles) {
        QListWidgetItem *elemento = new QListWidgetItem(
            QString::fromStdString(persona.nombre), lista);
        elemento->setData(Qt::UserRole, persona.idPersona);
    }

    QDialogButtonBox *botones = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QPushButton *agregar = botones->button(QDialogButtonBox::Ok);
    agregar->setText("Agregar");
    agregar->setEnabled(false);
    layout->addWidget(descripcion);
    layout->addWidget(filtro);
    layout->addWidget(lista);
    layout->addWidget(botones);

    connect(filtro, &QLineEdit::textChanged, &dialog, [lista](const QString& texto) {
        for (int i = 0; i < lista->count(); ++i) {
            QListWidgetItem *elemento = lista->item(i);
            elemento->setHidden(!elemento->text().contains(texto, Qt::CaseInsensitive));
        }

        if (lista->currentItem() != nullptr && lista->currentItem()->isHidden()) {
            lista->setCurrentItem(nullptr);
        }
    });
    connect(lista, &QListWidget::itemSelectionChanged, &dialog, [lista, agregar]() {
        agregar->setEnabled(lista->currentItem() != nullptr);
    });
    connect(lista, &QListWidget::itemDoubleClicked, &dialog, [&dialog](QListWidgetItem*) {
        dialog.accept();
    });
    connect(botones, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted || lista->currentItem() == nullptr) {
        return;
    }

    const int idPersona = lista->currentItem()->data(Qt::UserRole).toInt();
    const auto persona = std::find_if(disponibles.begin(), disponibles.end(),
        [idPersona](const DatosIntegrante& integrante) {
            return integrante.idPersona == idPersona;
        });

    if (persona != disponibles.end()) {
        integrantesGrupo.push_back(*persona);
        actualizarListaIntegrantes();
    }
}

void VentanaInterprete::quitarIntegrante() {
    if (listaIntegrantes->currentRow() < 0) {
        return;
    }

    const int indice = listaIntegrantes->currentRow();
    integrantesGrupo.erase(integrantesGrupo.begin() + indice);
    actualizarListaIntegrantes();
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

QString VentanaInterprete::obtenerFechaCreacion() const {
    return fechaCreacionActiva->isChecked() ? campoFechaCreacion->date().toString(Qt::ISODate) : QString();
}

QString VentanaInterprete::obtenerFechaSeparacion() const {
    return fechaSeparacionActiva->isChecked() ? campoFechaSeparacion->date().toString(Qt::ISODate) : QString();
}

std::vector<int> VentanaInterprete::obtenerIdsIntegrantes() const {
    std::vector<int> ids;
    ids.reserve(integrantesGrupo.size());

    for (const DatosIntegrante& integrante : integrantesGrupo) {
        ids.push_back(integrante.idPersona);
    }
    return ids;
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
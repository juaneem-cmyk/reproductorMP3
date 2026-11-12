#include "VentanaAlbumes.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QAbstractItemView>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
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

    // Configuración de la tabla y los botones
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    QLabel *descripcion = new QLabel(
        "Modifica el nombre, año o carpeta de los álbumes. Usa el control numérico de la columna Año. "
        "La cantidad de canciones es informativa. Los cambios se guardan únicamente en la base de datos; "
        "los MP3 no se modifican.", this);
    descripcion->setWordWrap(true);

    tablaAlbumes->setColumnCount(4);
    tablaAlbumes->setHorizontalHeaderLabels({"Álbum", "Año", "Carpeta", "Canciones"});
    tablaAlbumes->setRowCount(static_cast<int>(albumesOriginales.size()));
    tablaAlbumes->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaAlbumes->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaAlbumes->setAlternatingRowColors(true);
    tablaAlbumes->setShowGrid(false);
    tablaAlbumes->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::EditKeyPressed);
    tablaAlbumes->verticalHeader()->setVisible(false);
    tablaAlbumes->horizontalHeader()->setStretchLastSection(false);
    tablaAlbumes->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tablaAlbumes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
    tablaAlbumes->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    tablaAlbumes->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
    tablaAlbumes->setColumnWidth(1, 105);
    tablaAlbumes->setColumnWidth(3, 90);
    tablaAlbumes->setWordWrap(false);

    // Llenar la tabla con los datos de los álbumes
    for (int fila = 0; fila < static_cast<int>(albumesOriginales.size()); ++fila) {
        const DatosAlbum& album = albumesOriginales[static_cast<std::size_t>(fila)];
        auto *nombre = new QTableWidgetItem(QString::fromStdString(album.nombre));
        nombre->setData(Qt::UserRole, album.idAlbum);
        nombre->setToolTip(QString::fromStdString(album.nombre));
        tablaAlbumes->setItem(fila, 0, nombre);

        // El año se muestra como un control numérico editable
        auto *yearItem = new QTableWidgetItem();
        yearItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
        tablaAlbumes->setItem(fila, 1, yearItem);

        auto *year = new QSpinBox(tablaAlbumes);
        year->setRange(0, 9999);
        year->setSpecialValueText("Desconocido");
        year->setValue(album.year);
        year->setAlignment(Qt::AlignCenter);
        year->setToolTip("Año del álbum. Usa Desconocido si no se conoce.");
        year->setMinimumWidth(90);
        tablaAlbumes->setCellWidget(fila, 1, year);
        connect(year, qOverload<int>(&QSpinBox::valueChanged), this, [this, fila](int) {
            tablaAlbumes->selectRow(fila);
        });

        auto *ruta = new QTableWidgetItem(QString::fromStdString(album.ruta));
        ruta->setFlags(ruta->flags() & ~Qt::ItemIsEditable);
        ruta->setToolTip(QString::fromStdString(album.ruta));
        tablaAlbumes->setItem(fila, 2, ruta);

        auto *canciones = new QTableWidgetItem(QString::number(album.cantidadCanciones));
        canciones->setFlags(canciones->flags() & ~Qt::ItemIsEditable);
        canciones->setTextAlignment(Qt::AlignCenter);
        tablaAlbumes->setItem(fila, 3, canciones);
    }

    botonCambiarCarpeta->setEnabled(false);
    connect(tablaAlbumes, &QTableWidget::itemSelectionChanged, this, [this]() {
        botonCambiarCarpeta->setEnabled(tablaAlbumes->currentRow() >= 0);
    });
    connect(botonCambiarCarpeta, &QPushButton::clicked, this, &VentanaAlbumes::cambiarCarpeta);

    QDialogButtonBox *botones = new QDialogButtonBox(this);
    QPushButton *guardar = botones->addButton("Guardar cambios", QDialogButtonBox::AcceptRole);
    QPushButton *cancelar = botones->addButton("Cancelar", QDialogButtonBox::RejectRole);
    guardar->setDefault(true);

    layoutPrincipal->addWidget(descripcion);
    layoutPrincipal->addWidget(tablaAlbumes, 1);
    layoutPrincipal->addWidget(botonCambiarCarpeta, 0, Qt::AlignLeft);
    layoutPrincipal->addWidget(botones);

    connect(guardar, &QPushButton::clicked, this, &VentanaAlbumes::guardarCambios);
    connect(cancelar, &QPushButton::clicked, this, &QDialog::reject);

    setStyleSheet(R"(
        QDialog { background-color: #20251B; color: #E6E5D7; }
        QLabel { color: #E6E5D7; }
        QTableWidget {
            background-color: #292F23;
            alternate-background-color: #303B27;
            color: #E6E5D7;
            border: none;
            gridline-color: #503923;
            selection-background-color: #5E873B;
            selection-color: white;
        }
        QHeaderView::section {
            background-color: #39452E;
            color: #E6E5D7;
            border: none;
            border-bottom: 2px solid #805638;
            padding: 9px;
            font-weight: bold;
        }
        QTableWidget QLineEdit, QTableWidget QSpinBox {
            background-color: #20251B;
            color: #E6E5D7;
            border: 1px solid #86A94F;
            border-radius: 3px;
            padding: 3px;
        }
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

// Obtener los datos de los álbumes modificados en la tabla
std::vector<DatosAlbum> VentanaAlbumes::obtenerAlbumes() const {
    std::vector<DatosAlbum> resultado;
    resultado.reserve(static_cast<std::size_t>(tablaAlbumes->rowCount()));

    for (int fila = 0; fila < tablaAlbumes->rowCount(); ++fila) {
        DatosAlbum album = albumesOriginales[static_cast<std::size_t>(fila)];
        album.nombre = tablaAlbumes->item(fila, 0)->text().trimmed().toStdString();
        const auto *campoYear = qobject_cast<QSpinBox*>(tablaAlbumes->cellWidget(fila, 1));
        album.year = campoYear == nullptr ? albumesOriginales[static_cast<std::size_t>(fila)].year
        : campoYear->value();
        album.ruta = tablaAlbumes->item(fila, 2)->text().toStdString();
        resultado.push_back(std::move(album));
    }
    return resultado;
}

// Validar los datos de la tabla y cerrar la ventana si son correctos
void VentanaAlbumes::guardarCambios() {
    for (int fila = 0; fila < tablaAlbumes->rowCount(); ++fila) {
        const QString nombre = tablaAlbumes->item(fila, 0)->text().trimmed();

        if (nombre.isEmpty()) {
            QMessageBox::warning(this, "Nombre inválido",
                "El nombre del álbum no puede estar vacío.");
            tablaAlbumes->setCurrentCell(fila, 0);
            tablaAlbumes->editItem(tablaAlbumes->item(fila, 0));
            return;
        }
        
        if (tablaAlbumes->item(fila, 2)->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Carpeta inválida",
                "Selecciona una carpeta para cada álbum.");
            tablaAlbumes->setCurrentCell(fila, 2);
            return;
        }
        auto *campoYear = qobject_cast<QSpinBox*>(tablaAlbumes->cellWidget(fila, 1));

        if (campoYear == nullptr || campoYear->value() < 0 || campoYear->value() > 9999) {
            QMessageBox::warning(this, "Año inválido",
                "El año debe ser un número entre 0 y 9999. Usa 0 si es desconocido.");
            if (campoYear != nullptr) {
                campoYear->setFocus();
            }
            return;
        }
    }
    accept();
}

// Abrir una ventana para seleccionar la carpeta de un álbum y actualizar la tabla
void VentanaAlbumes::cambiarCarpeta() {
    const int fila = tablaAlbumes->currentRow();
    if (fila < 0 || tablaAlbumes->item(fila, 2) == nullptr) {
        return;
    }

    const QString carpetaActual = tablaAlbumes->item(fila, 2)->text();
    const QString carpeta = QFileDialog::getExistingDirectory(this,
        "Seleccionar carpeta del álbum", carpetaActual);
    if (carpeta.isEmpty()) {
        return;
    }

    tablaAlbumes->item(fila, 2)->setText(carpeta);
    tablaAlbumes->item(fila, 2)->setToolTip(carpeta);
}
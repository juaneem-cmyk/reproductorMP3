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

    // Estética de cofre
    setStyleSheet(R"(
        QDialog {
            background-color: #291D12;
            color: #F2E0C0;
        }
        QLabel {
            color: #F2E0C0;
            background: transparent;
        }
        QTableWidget {
            background-color: #332316;
            alternate-background-color: #422D1B;
            color: #F2E0C0;
            border: 2px solid #8C602C;
            gridline-color: #68451F;
            selection-background-color: #A87932;
            selection-color: #FFFFFF;
            outline: 0px;
        }
        QTableWidget::item {
            padding: 5px;
            border: 0px;
        }
        QTableWidget::item:selected {
            background-color: #A87932;
            color: #FFFFFF;
        }
        QHeaderView::section {
            background-color: #795024;
            color: #FFF0D0;
            border: 1px solid #A87932;
            border-bottom: 3px solid #C09245;
            padding: 9px;
            font-family: "Press Start 2P";
            font-size: 8px;
            font-weight: bold;
        }
        QTableWidget QLineEdit, QTableWidget QSpinBox {
            background-color: #2D2016;
            color: #F6E7D0;
            border: 2px solid #A87932;
            border-top-color: #D0A45B;
            border-left-color: #D0A45B;
            border-radius: 0px;
            padding: 3px;
        }
        QTableWidget QSpinBox:focus, QTableWidget QLineEdit:focus {
            border-color: #E0BC75;
        }
        QPushButton {
            background-color: #795024;
            color: #FFF0D8;
            border: 2px solid #C09245;
            border-right-color: #4B321A;
            border-bottom-color: #4B321A;
            border-radius: 0px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #A87932;
            border-color: #E0BC75;
        }
        QPushButton:pressed {
            background-color: #5A3A1A;
            border-top-color: #4B321A;
            border-left-color: #4B321A;
            border-right-color: #C09245;
            border-bottom-color: #C09245;
        }
        QScrollBar:vertical {
            background: #291D12;
            width: 14px;
            margin: 0px;
            border: 1px solid #68451F;
        }
        QScrollBar::handle:vertical {
            background: #A87932;
            min-height: 24px;
            border: 1px solid #D0A45B;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0px;
        }
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
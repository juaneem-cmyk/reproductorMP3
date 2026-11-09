#include "PanelCanciones.h"
#include "../modelos/Canciones.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QHeaderView>
#include <QSortFilterProxyModel>

PanelCanciones::PanelCanciones(QWidget *parent) : QWidget(parent) {
    setObjectName("panelDerecho");

    // Distribución interna
    QVBoxLayout *layoutPrincipal = new QVBoxLayout(this);
    layoutPrincipal->setContentsMargins(10, 10, 10, 10);
    layoutPrincipal->setSpacing(10);

    // Crear el modelo
    modeloCanciones = new Canciones(this);

    // Crear el filtro
    filtroCanciones = new QSortFilterProxyModel(this);
    filtroCanciones->setSourceModel(modeloCanciones);
    filtroCanciones->setFilterRole(Canciones::TextoBusquedaRole);
    filtroCanciones->setFilterKeyColumn(0);
    filtroCanciones->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // Crear la tabla
    tablaCanciones = new QTableView(this);
    tablaCanciones->setModel(filtroCanciones);

    // Configuración de la tabla
    tablaCanciones->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaCanciones->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaCanciones->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaCanciones->setAlternatingRowColors(true);
    tablaCanciones->setShowGrid(false);
    tablaCanciones->verticalHeader()->setVisible(false);

    // Ajustar columnas
    QHeaderView *encabezado = tablaCanciones->horizontalHeader();
    encabezado->setSectionResizeMode(0, QHeaderView::Stretch);
    encabezado->setSectionResizeMode(1, QHeaderView::Stretch);
    encabezado->setSectionResizeMode(2, QHeaderView::Stretch);
    encabezado->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    encabezado->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    encabezado->setDefaultAlignment(Qt::AlignCenter);
    tablaCanciones->verticalHeader()->setDefaultSectionSize(42);

    // Agregar tabla al panel
    layoutPrincipal->addWidget(tablaCanciones);

    // Estilos de la tabla
    tablaCanciones->setStyleSheet(R"(
        QTableView {
            background-color: #292F23;
            alternate-background-color: #303B27;
            color: #E6E5D7;
            border: none;
            selection-background-color: #5E873B;
            selection-color: #FFFFFF;
            font-size: 13px;
        }
        QHeaderView::section {
            background-color: #39452E;
            color: #E6E5D7;
            border: none;
            border-bottom: 2px solid #805638;
            padding: 10px;
            font-weight: bold;
        }
    )");
}

QTableView *PanelCanciones::obtenerTabla() const {
    return tablaCanciones;
}

QString PanelCanciones::obtenerRutaCancion(int fila) const {
    QModelIndex indiceProxy = filtroCanciones->index(fila, 0);

    if (!indiceProxy.isValid()) {
        return {};
    }
    QModelIndex indiceOriginal = filtroCanciones->mapToSource(indiceProxy);
    return modeloCanciones->rutaEnFila(indiceOriginal.row());
}

void PanelCanciones::establecerTextoBusqueda(const QString &texto) {
    filtroCanciones->setFilterFixedString(texto.trimmed());
}
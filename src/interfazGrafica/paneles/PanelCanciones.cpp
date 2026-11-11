#include "PanelCanciones.h"
#include "../modelos/Canciones.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QListView>
#include <QStackedWidget>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QSize>
#include <QFontMetrics>
#include <QEvent>

// Centra el conjunto de portadas sin modificar el tamaño ni la separación que ya tienen
class GaleriaCentrada : public QListView {
public:
    explicit GaleriaCentrada(QWidget *parent = nullptr) : QListView(parent) {
        // La ventana también cambia cuando aparece o desaparece la barra de desplazamiento
        viewport()->installEventFilter(this);
    }

protected:
    bool eventFilter(QObject *objeto, QEvent *evento) override {
        if (objeto == viewport() && evento->type() == QEvent::Resize) {
            ajustarMargenHorizontal();
        }
        return QListView::eventFilter(objeto, evento);
    }

private:
    void ajustarMargenHorizontal() {
        constexpr int ANCHO_CELDA = 190;
        constexpr int ESPACIADO = 10;
        constexpr int ANCHO_PASO = ANCHO_CELDA + ESPACIADO;

        // Recupera el ancho disponible antes de aplicar los márgenes propios
        const int anchoDisponible = viewport()->width() + 2 * margenHorizontal;
        if (anchoDisponible <= 0) {
            return;
        }

        // QListView también reserva el espaciado entre celdas
        const int columnas = qMax(1, (anchoDisponible + ESPACIADO) / ANCHO_PASO);
        const int anchoOcupado = columnas * ANCHO_CELDA + (columnas - 1) * ESPACIADO;
        const int nuevoMargen = qMax(0, (anchoDisponible - anchoOcupado) / 2);

        if (nuevoMargen != margenHorizontal) {
            margenHorizontal = nuevoMargen;
            setViewportMargins(margenHorizontal, 0, margenHorizontal, 0);
        }
    }

    int margenHorizontal = 0;
};

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

    // Crear la tabla de canciones
    tablaCanciones = new QTableView(this);
    tablaCanciones->setModel(filtroCanciones);

    // Crear la vista de galería
    galeriaCanciones = new GaleriaCentrada(this);
    galeriaCanciones->setModel(filtroCanciones);
    galeriaCanciones->setSelectionMode(QAbstractItemView::SingleSelection);
    galeriaCanciones->setSelectionBehavior(QAbstractItemView::SelectItems);
    galeriaCanciones->setEditTriggers(QAbstractItemView::NoEditTriggers);
    galeriaCanciones->setViewMode(QListView::IconMode);
    galeriaCanciones->setFlow(QListView::LeftToRight);
    galeriaCanciones->setWrapping(true);
    galeriaCanciones->setResizeMode(QListView::Adjust);
    galeriaCanciones->setMovement(QListView::Static);
    galeriaCanciones->setSpacing(10);
    galeriaCanciones->setGridSize(QSize(190, 226));
    galeriaCanciones->setIconSize(QSize(160, 160));
    galeriaCanciones->setUniformItemSizes(true);
    galeriaCanciones->setWordWrap(true);
    galeriaCanciones->setTextElideMode(Qt::ElideRight);

    // Ambas vistas comparten la selección para conservar edición y reproducción
    galeriaCanciones->setSelectionModel(tablaCanciones->selectionModel());

    // Alternar entre tabla y galería sin duplicar el modelo ni la búsqueda
    vistasCanciones = new QStackedWidget(this);
    vistasCanciones->addWidget(tablaCanciones);
    vistasCanciones->addWidget(galeriaCanciones);
    vistasCanciones->setCurrentWidget(tablaCanciones);

    // Configuración de la tabla
    tablaCanciones->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaCanciones->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaCanciones->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaCanciones->setAlternatingRowColors(true);
    tablaCanciones->setShowGrid(false);
    tablaCanciones->setIconSize(QSize(36, 36));
    tablaCanciones->verticalHeader()->setVisible(false);

    // Ajustar columnas
    QHeaderView *encabezado = tablaCanciones->horizontalHeader();
    encabezado->setSectionResizeMode(0, QHeaderView::Stretch);
    encabezado->setSectionResizeMode(1, QHeaderView::Interactive);
    encabezado->setSectionResizeMode(2, QHeaderView::Interactive);
    tablaCanciones->setColumnWidth(1, 135);
    tablaCanciones->setColumnWidth(2, 120);
    encabezado->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    // Ajustar la columna de duración para que tenga un ancho fijo suficiente para mostrar "00:00"
    QFont fuenteEncabezado = encabezado->font();
    fuenteEncabezado.setBold(true);
    QFontMetrics medidasDuracion(fuenteEncabezado);
    int anchoDuracion = medidasDuracion.horizontalAdvance("Duración");
    int anchoTiempo = medidasDuracion.horizontalAdvance("00:00");
    if (anchoTiempo > anchoDuracion) {
        anchoDuracion = anchoTiempo;
    }
    encabezado->setSectionResizeMode(4, QHeaderView::Fixed);
    tablaCanciones->setColumnWidth(4, anchoDuracion + 20);
    encabezado->setDefaultAlignment(Qt::AlignCenter);
    tablaCanciones->verticalHeader()->setDefaultSectionSize(48);

    // Agregar ambas vistas al panel
    layoutPrincipal->addWidget(vistasCanciones);

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

    // Estilo de la galería de portadas
    galeriaCanciones->setStyleSheet(R"(
        QListView {
            background-color: #292F23;
            color: #E6E5D7;
            border: none;
            font-size: 13px;
            outline: none;
        }
        QListView::item {
            padding: 5px;
            border-radius: 5px;
        }
        QListView::item:selected {
            background-color: #5E873B;
            color: #FFFFFF;
        }
        QListView::item:hover {
            background-color: #39452E;
        }
    )");
}

QTableView *PanelCanciones::obtenerTabla() const {
    return tablaCanciones;
}

QListView *PanelCanciones::obtenerGaleria() const {
    return galeriaCanciones;
}

void PanelCanciones::establecerVistaGaleria(bool activa) {
    vistasCanciones->setCurrentWidget(activa ? static_cast<QWidget *>(galeriaCanciones)
    : static_cast<QWidget *>(tablaCanciones));
}

void PanelCanciones::seleccionarFila(int fila) {
    QModelIndex indice = filtroCanciones->index(fila, 0);

    if (!indice.isValid()) {
        return;
    }

    // Seleccionar la fila completa también en la tabla oculta
    tablaCanciones->selectRow(fila);

    if (vistasCanciones->currentWidget() == galeriaCanciones) {
        galeriaCanciones->scrollTo(indice);
    } else {
        tablaCanciones->scrollTo(indice);
    }
}

QString PanelCanciones::obtenerRutaCancion(int fila) const {
    QModelIndex indiceProxy = filtroCanciones->index(fila, 0);

    if (!indiceProxy.isValid()) {
        return {};
    }
    QModelIndex indiceOriginal = filtroCanciones->mapToSource(indiceProxy);
    return modeloCanciones->rutaEnFila(indiceOriginal.row());
}

bool PanelCanciones::CancionSelect(CancionVista &cancion) const {
    QModelIndexList seleccion = tablaCanciones->selectionModel()->selectedIndexes();

    if (seleccion.isEmpty()) {
        return false;
    }

    // Convertir el índice filtrado al índice del modelo original
    QModelIndex indiceOriginal = filtroCanciones->mapToSource(seleccion.first());

    if (!indiceOriginal.isValid()) {
        return false;
    }

    return modeloCanciones->obtenerCancion(indiceOriginal.row(), cancion);
}

void PanelCanciones::establecerTextoBusqueda(const QString &texto) {
    filtroCanciones->setFilterFixedString(texto.trimmed());
}

void PanelCanciones::recargarCanciones() {
    modeloCanciones->recargar();
}
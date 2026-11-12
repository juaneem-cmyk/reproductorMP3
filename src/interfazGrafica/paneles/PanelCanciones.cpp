#include "PanelCanciones.h"
#include "../modelos/Canciones.h"
#include "../TemaPrincipal.h"
#include <QVBoxLayout>
#include <QTableView>
#include <QAbstractItemView>
#include <QListView>
#include <QStackedWidget>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QItemSelectionModel>
#include <QSize>
#include <QFont>
#include <QFontMetrics>
#include <QEvent>
#include <QApplication>
#include <QMouseEvent>

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
    tablaCanciones->setObjectName("tablaCanciones");
    tablaCanciones->setModel(filtroCanciones);

    // Crear la vista de galería
    galeriaCanciones = new GaleriaCentrada(this);
    galeriaCanciones->setObjectName("galeriaCanciones");
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
    encabezado->setFont(TemaPrincipal::fuenteEncabezado(9));
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
    tablaCanciones->setFont(QFont("DejaVu Sans", 9));
    galeriaCanciones->setFont(QFont("DejaVu Sans", 9));

    // Agregar ambas vistas al panel
    layoutPrincipal->addWidget(vistasCanciones);

    // Estilo de la tabla: encabezados retro y divisiones rectas inspiradas en bloques de Minecraft
    tablaCanciones->setStyleSheet(R"(
        QTableView {
            background-color: #263A27;
            alternate-background-color: #30482F;
            color: #F2EBD5;
            border: none;
            selection-background-color: #648F3D;
            selection-color: #FFFFFF;
            outline: none;
            font-size: 13px;
        }
        QTableView::item {
            padding: 4px 6px;
            border: none;
        }
        QTableView::item:selected {
            background-color: #648F3D;
            color: #FFFFFF;
            border: 1px solid #A9C978;
        }
        QTableView::item:hover {
            background-color: #426536;
        }
        QHeaderView::section {
            background-color: #4B7138;
            color: #F2EBD5;
            border-style: solid;
            border-width: 0px 1px 3px 0px;
            border-color: transparent #344C2E #A36D3C transparent;
            padding: 9px 7px;
            font-weight: bold;
        }
    )");

    // La galería conserva las portadas como protagonistas
    galeriaCanciones->setStyleSheet(R"(
        QListView {
            background-color: #263A27;
            color: #F2EBD5;
            border: none;
            outline: none;
            font-size: 13px;
        }
        QListView::item {
            padding: 5px;
            margin: 1px;
            border: 2px solid transparent;
        }
        QListView::item:selected {
            background-color: #426536;
            color: #FFFFFF;
            border: 2px solid #A9C978;
        }
        QListView::item:hover {
            background-color: #354F30;
            border: 2px solid #789D4C;
        }
    )");

    // Detectar clics fuera de las vistas para permitir limpiar la selección
    if (QApplication::instance()) {
        QApplication::instance()->installEventFilter(this);
    }
}

PanelCanciones::~PanelCanciones() {
    if (QApplication::instance()) {
        QApplication::instance()->removeEventFilter(this);
    }
}

bool PanelCanciones::eventFilter(QObject *objeto, QEvent *evento) {
    if (evento->type() != QEvent::MouseButtonPress) {
        return QWidget::eventFilter(objeto, evento);
    }

    QWidget *widget = qobject_cast<QWidget *>(objeto);
    if (!widget) {
        return QWidget::eventFilter(objeto, evento);
    }

    // Ignorar eventos que no ocurran dentro del área principal de trabajo
    QWidget *areaTrabajo = parentWidget();
    QWidget *ancestro = widget;
    while (ancestro && ancestro != areaTrabajo) {
        ancestro = ancestro->parentWidget();
    }
    if (!areaTrabajo || ancestro != areaTrabajo) {
        return QWidget::eventFilter(objeto, evento);
    }

    // Si se pulsa una zona vacía de la ventana se limpia la selección de la tabla y la galería
    auto limpiarSiEsEspacioVacio = [this, evento](QAbstractItemView *vista) {
        const auto *clic = static_cast<QMouseEvent *>(evento);
        const QPoint posicion = clic->position().toPoint();
        if (!vista->indexAt(posicion).isValid()) {
            tablaCanciones->selectionModel()->clear();
        }
    };

    // Mantener la selección al interactuar con cualquier parte de las vistas
    auto perteneceA = [widget](QWidget *raiz) {
        QWidget *actual = widget;
        while (actual) {
            if (actual == raiz) {
                return true;
            }
            actual = actual->parentWidget();
        }
        return false;
    };

    if (perteneceA(tablaCanciones)) {
        if (widget == tablaCanciones->viewport()) {
            limpiarSiEsEspacioVacio(tablaCanciones);
        }
        return QWidget::eventFilter(objeto, evento);
    }

    if (perteneceA(galeriaCanciones)) {
        if (widget == galeriaCanciones->viewport()) {
            limpiarSiEsEspacioVacio(galeriaCanciones);
        }
        return QWidget::eventFilter(objeto, evento);
    }

    // Cualquier clic en el resto del área de trabajo deselecciona la canción
    tablaCanciones->selectionModel()->clear();
    return QWidget::eventFilter(objeto, evento);
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
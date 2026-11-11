#ifndef PANEL_CANCIONES_H
#define PANEL_CANCIONES_H

#include <QWidget>
#include <QString>

class QTableView;
class QListView;
class QStackedWidget;
class Canciones;
class QSortFilterProxyModel;
struct CancionVista;

class PanelCanciones : public QWidget {
public:
    explicit PanelCanciones(QWidget *parent = nullptr);
    void establecerTextoBusqueda(const QString &texto);
    void recargarCanciones();
    void establecerVistaGaleria(bool activa);
    void seleccionarFila(int fila);
    bool CancionSelect(CancionVista &cancion) const;
    
    QTableView *obtenerTabla() const;
    QListView *obtenerGaleria() const;
    QString obtenerRutaCancion(int fila) const;

private:
    QTableView *tablaCanciones;
    QListView *galeriaCanciones;
    QStackedWidget *vistasCanciones;
    QSortFilterProxyModel *filtroCanciones;
    Canciones *modeloCanciones;
};
#endif
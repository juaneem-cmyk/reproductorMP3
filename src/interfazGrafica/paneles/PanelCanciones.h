#ifndef PANEL_CANCIONES_H
#define PANEL_CANCIONES_H

#include <QWidget>
#include <QString>

class QTableView;
class Canciones;
class QSortFilterProxyModel;

class PanelCanciones : public QWidget {
public:
    explicit PanelCanciones(QWidget *parent = nullptr);
    void establecerTextoBusqueda(const QString &texto);
    void recargarCanciones();
    
    QTableView *obtenerTabla() const;
    QString obtenerRutaCancion(int fila) const;

private:
    QTableView *tablaCanciones;
    QSortFilterProxyModel *filtroCanciones;
    Canciones *modeloCanciones;
};
#endif
#ifndef PANEL_CANCIONES_H
#define PANEL_CANCIONES_H

#include <QWidget>
#include <QString>

class QTableView;
class Canciones;

class PanelCanciones : public QWidget {
public:
    explicit PanelCanciones(QWidget *parent = nullptr);
    
    QTableView *obtenerTabla() const;
    QString obtenerRutaCancion(int fila) const;

private:
    QTableView *tablaCanciones;
    Canciones *modeloCanciones;
};
#endif
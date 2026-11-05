#ifndef PANEL_CANCIONES_H
#define PANEL_CANCIONES_H

#include <QWidget>

class QTableView;
class Canciones;

class PanelCanciones : public QWidget {
public:
    explicit PanelCanciones(QWidget *parent = nullptr);

private:
    QTableView *tablaCanciones;
    Canciones *modeloCanciones;
};
#endif
#ifndef PANEL_CANCIONES_H
#define PANEL_CANCIONES_H

#include <QWidget>

class QTableView;
class ModeloCanciones;

class PanelCanciones : public QWidget {
public:
    explicit PanelCanciones(QWidget *parent = nullptr);

private:
    QTableView *tablaCanciones;
    ModeloCanciones *modeloCanciones;
};
#endif
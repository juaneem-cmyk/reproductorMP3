#ifndef CANCIONES_H
#define CANCIONES_H

#include <QAbstractTableModel>
#include <QString>
#include <vector>

struct CancionVista {
    QString titulo;
    QString artista;
    QString album;
    int year;
    QString ruta;
    QString duracion;
};

class Canciones : public QAbstractTableModel {
public:
    explicit Canciones(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &indice, int rol = Qt::DisplayRole) const override;
    QVariant headerData(int seccion, Qt::Orientation orientacion, int rol = Qt::DisplayRole) const override;
    QString rutaEnFila(int fila) const;

private:
    std::vector<CancionVista> canciones;
    void cargarCanciones();
};
#endif
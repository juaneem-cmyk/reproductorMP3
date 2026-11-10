#ifndef CANCIONES_H
#define CANCIONES_H

#include <QAbstractTableModel>
#include <QString>
#include <vector>

struct CancionVista {
    int idRola = 0;
    QString titulo;
    QString artista;
    QString album;
    int year = 0;
    QString ruta;
    QString duracion;
    int idInterprete = 0;
    int idTipoInterprete = 2;
};

class Canciones : public QAbstractTableModel {
public:
    explicit Canciones(QObject *parent = nullptr);
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    bool obtenerCancion(int fila, CancionVista &cancion) const;
    enum Roles {
    TextoBusquedaRole = Qt::UserRole + 1
    };
    void recargar();

    QVariant data(const QModelIndex &indice, int rol = Qt::DisplayRole) const override;
    QVariant headerData(int seccion, Qt::Orientation orientacion, int rol = Qt::DisplayRole) const override;
    QString rutaEnFila(int fila) const;

private:    
    std::vector<CancionVista> canciones;

    void cargarCanciones();
};
#endif
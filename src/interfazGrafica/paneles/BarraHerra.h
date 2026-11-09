#ifndef BARRA_HERRAMIENTAS_H
#define BARRA_HERRAMIENTAS_H

#include <QWidget>

class QToolButton;
class QLineEdit;

class BarraHerramientas : public QWidget {
public:
    explicit BarraHerramientas(QWidget *parent = nullptr);

    QToolButton *obtenerBotonEditarCancion() const;
    QToolButton *obtenerBotonEditarInterprete() const;
    QLineEdit *obtenerBarraBusqueda() const;
    QToolButton *obtenerBotonLista() const;
    QToolButton *obtenerBotonGaleria() const;
    QToolButton *obtenerBotonImportarMusica() const;

private:
    QToolButton *botonEditarCancion;
    QToolButton *botonEditarInterprete;
    QLineEdit *barraBusqueda;
    QToolButton *botonLista;
    QToolButton *botonGaleria;
    QToolButton *botonImportarMusica;
};
#endif
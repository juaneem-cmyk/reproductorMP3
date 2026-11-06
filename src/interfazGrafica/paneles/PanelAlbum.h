#ifndef PANEL_ALBUM_H
#define PANEL_ALBUM_H

#include <QWidget>
#include <QImage>

class Portada;

class PanelAlbum : public QWidget {
public:
    explicit PanelAlbum(QWidget *parent = nullptr);
    void establecerPortada(const QImage &imagen);
    void limpiarPortada();

private:
    Portada *portada;
};
#endif
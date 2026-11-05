#ifndef PORTADA_H
#define PORTADA_H

#include <QFrame>

class QLabel;
class QResizeEvent;
class Portada : public QFrame {
public:
    explicit Portada(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *evento) override;

private:
    QLabel *portada;
};

#endif
#include <QApplication>
#include "VentanaPrincipal.h"

int main(int argc, char *argv[]) {
    QApplication aplicacion(argc, argv);
    VentanaPrincipal ventana;
    ventana.show();
    return aplicacion.exec();
}
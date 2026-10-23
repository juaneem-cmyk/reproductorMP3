#include "ControladorMinero.h"

int main(int numeroArgumentos, char* listaArgumentos[]) {
    ControladorMinero controlador;
    return controlador.ejecutar(
        numeroArgumentos,
        listaArgumentos
    );
}
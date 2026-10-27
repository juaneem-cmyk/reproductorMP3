#include "MineroABD.h"
#include "Biblioteca.h"
#include <iostream>
#include <string>

int main(int numeroArgumentos, char* listaArgumentos[]) {
    // Inicializar la base de datos cuando se solicite explícitamente
    if (numeroArgumentos == 2 && std::string(listaArgumentos[1]) == "--inicializar-bd") {
        Biblioteca preparador;
        bool baseDatosCreada = false;
        std::string mensajeError;

        if (!preparador.biblioteca(baseDatosCreada, mensajeError)) {
            std::cerr << "Error al preparar la biblioteca: " << mensajeError << '\n';
            return 1;
        }

        if (baseDatosCreada) {
            std::cout << "Base de datos inicializada correctamente.\n";
        } else {
            std::cout << "La base de datos ya existía.\n";
        }
        return 0;
    }
    // Ejecutar el minero sin encargarse de inicializar la base de datos
    MineroABD minero;
    return minero.ejecutar(numeroArgumentos, listaArgumentos);
}
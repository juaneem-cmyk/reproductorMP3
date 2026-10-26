#include "MineroABD.h"
#include "ControladorMinero.h"
#include "Biblioteca.h"
#include <iostream>
#include <string>

int MineroABD::ejecutar(int numeroArgumentos, char* listaArgumentos[]) const {
    const bool soloInicializar = numeroArgumentos == 2 && std::string(listaArgumentos[1]) == "--inicializar-bd";

    // Si los argumentos son incorrectos, delegar al controlador
    if (!soloInicializar && numeroArgumentos != 2) {
        ControladorMinero controlador;
        return controlador.ejecutar(numeroArgumentos, listaArgumentos);
    }
    // Preparar la biblioteca antes de utilizarla
    Biblioteca preparador;
    bool baseDatosCreada = false;
    std::string mensajeError;

    if (!preparador.biblioteca(baseDatosCreada, mensajeError)) {
        std::cerr << "Error al preparar la biblioteca: " << mensajeError << '\n';
        return 1;
    }

    // Permitir inicializar la base de datos sin procesar canciones
    if (soloInicializar) {
        if (baseDatosCreada) {
            std::cout << "Base de datos inicializada correctamente.\n";
        } else {
            std::cout << "La base de datos ya existía.\n";
        }
        return 0;
    }
    // Delegar el procesamiento al controlador del minero
    ControladorMinero controlador;
    return controlador.ejecutar(numeroArgumentos, listaArgumentos);
}
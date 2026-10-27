#include "MineroABD.h"
#include "ControladorMinero.h"
#include <iostream>
#include <string>
#include <cctype>
#include <filesystem>
#include <system_error>

int MineroABD::ejecutar(int numeroArgumentos, char* listaArgumentos[]) const {
    const bool soloInicializar = numeroArgumentos == 2 && std::string(listaArgumentos[1]) == "--inicializar-bd";

    // Si los argumentos son incorrectos, delegar al controlador
    if (numeroArgumentos != 2) {
        ControladorMinero controlador;
        return controlador.ejecutar(numeroArgumentos, listaArgumentos);
    }
    // Validar la ruta antes de preparar la biblioteca
    if (!soloInicializar) {
        namespace fs = std::filesystem;    
        const fs::path ruta(listaArgumentos[1]);
        std::error_code error;

        // Verificar si la ruta es un archivo regular
        if (fs::is_regular_file(ruta, error) && !error) {
            std::string extension = ruta.extension().string();

            // Comparar la extensión sin distinguir mayúsculas
            for (char& caracter : extension) {
                caracter = static_cast<char>(std::tolower(static_cast<unsigned char>(caracter)));
            }

            if (extension != ".mp3") {
                std::cerr << "Error: el archivo indicado no tiene " << "extensión MP3.\n";
                return 1;
            }
        } else {
            error.clear();

            if (!fs::is_directory(ruta, error) || error) {
                std::cerr << "Error: la ruta no es un archivo regular " << "ni un directorio válido o no se puede acceder.\n";
                return 1;
            }
        }
    }
    // Delegar el procesamiento al controlador del minero
    ControladorMinero controlador;
    return controlador.ejecutar(numeroArgumentos, listaArgumentos);
}
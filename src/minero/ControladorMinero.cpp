#include "ControladorMinero.h"
#include "ImportarMusica.h"
#include <iostream>
#include <filesystem>
#include <string>

int ControladorMinero::ejecutar(int numeroArgumentos, char* listaArgumentos[]) const {
    if (numeroArgumentos != 2) {
        std::cerr << "Uso:\n" << "  minero <archivo.mp3 | directorio>\n";
        return 1;
    }
    ImportarMusica importador;
    ResultadoMinado resultado;
    std::string mensajeError;

    if (!importador.importar(std::filesystem::path(listaArgumentos[1]), resultado, mensajeError)) {
        std::cerr << "Error al importar música: " << mensajeError << '\n';
        return 1;
    }

    std::cout << "Archivos encontrados: " << resultado.archivosEncontrados << '\n';
    std::cout << "Archivos procesados: " << resultado.archivosProcesados << '\n';
    std::cout << "Archivos omitidos: " << resultado.archivosOmitidos << '\n';
    std::cout << "Canciones nuevas registradas: " << resultado.cancionesInsertadas << '\n';
    std::cout << "Canciones que ya existían: " << resultado.cancionesExistentes << '\n';
    std::cout << "Errores de registro: " << resultado.erroresRegistro << '\n';
    return 0;
}
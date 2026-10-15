#include "LectorMetadatos.h"
#include "ExplorarDirectorios.h"
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int main(int numeroArgumentos, char* listaArgumentos[]) {
    if (numeroArgumentos != 2) {
        std::cerr << "Uso: minero <directorio-musical>\n";
        return 1;
    }
    std::filesystem::path directorio = listaArgumentos[1];
    ExplorarDirectorios explorar;
    LectorMetadatosMP3 lector;
    std::vector<std::filesystem::path> archivosMP3 = explorar.buscarMP3(directorio);

    if (archivosMP3.empty()) {
        std::cout << "No se encontraron archivos MP3 en el directorio.\n";
        return 0;
    }
    std::cout << "Se encontraron " << archivosMP3.size() << " archivos MP3.\n\n";

    for (const auto& ruta : archivosMP3) {
        MetadatosMusicales metadatos;
        std::string mensajeError;
        std::cout << "Archivo: " << ruta << '\n';

        if (!lector.leerArchivo(ruta.string(), metadatos, mensajeError)) {
            std::cerr << "Error: " << mensajeError << "\n\n";
            continue;
        }
        std::cout << "Título: " << metadatos.titulo << '\n';
        std::cout << "Artista: " << metadatos.artista << '\n';
        std::cout << "Álbum: " << metadatos.album << '\n';
        std::cout << "Año: " << metadatos.year << '\n';
        std::cout << "Número de pista: " << metadatos.numeroPista << '\n';
        std::cout << "Duración: " << metadatos.duracionSegundos << " segundos\n\n";
    }
    return 0;
}
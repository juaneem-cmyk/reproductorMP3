#include "LectorMetadatos.h"
#include "ExplorarDirectorios.h"
#include <filesystem>
#include <iostream>
#include <cctype>
#include <string>
#include <vector>

int main(int numeroArgumentos, char* listaArgumentos[]) {
    if (numeroArgumentos != 2) {
        std::cerr << "Uso: minero <archivo.mp3 | directorio>\n";
        return 1;
    }
    std::filesystem::path ruta = listaArgumentos[1];
    std::vector<std::filesystem::path> archivosMP3;
    std::error_code error;

    if (std::filesystem::is_regular_file(ruta, error) && !error) {
        std::string extension = ruta.extension().string();

        for (char& caracter : extension) {
            caracter = static_cast<char>(std::tolower(static_cast<unsigned char>(caracter)));
        }

        if (extension != ".mp3") {
            std::cerr << "Error: el archivo indicado no tiene extensión MP3.\n";
            return 1;
        }
        archivosMP3.push_back(ruta);
    } else {
        error.clear();

        if (!std::filesystem::is_directory(ruta, error) || error) {
            std::cerr << "Error: la ruta no es un archivo regular ni un directorio " << "válido o no se puede acceder.\n";
            return 1;
        }
        ExplorarDirectorios explorador;
        archivosMP3 = explorador.buscarMP3(ruta);
    }

    if (archivosMP3.empty()) {
        std::cout << "No se encontraron archivos MP3.\n";
        return 0;
    }
    std::cout << "Se encontraron " << archivosMP3.size() << " archivos con extensión MP3.\n\n";
    LectorMetadatosMP3 lector;
    unsigned int archivosProcesados = 0;
    unsigned int archivosOmitidos = 0;

    for (const auto& rutaArchivo : archivosMP3) {
        MetadatosMusicales metadatos;
        std::string mensajeError;
        std::cout << "Archivo: " << rutaArchivo << '\n';

        if (!lector.leerArchivo(rutaArchivo.string(), metadatos, mensajeError)) {
            archivosOmitidos++;
            std::cerr << "Omitido: " << mensajeError << "\n\n";
            continue;
        }
        std::cout << "Título: " << metadatos.titulo << '\n';
        std::cout << "Artista: " << metadatos.artista << '\n';
        std::cout << "Álbum: " << metadatos.album << '\n';
        std::cout << "Año: " << metadatos.year << '\n';
        std::cout << "Número de pista: " << metadatos.numeroPista << '\n';
        std::cout << "Duración: " << metadatos.duracionSegundos << " segundos\n\n";
        archivosProcesados++;
    }
    std::cout << "Archivos procesados correctamente: " << archivosProcesados << '\n';
    std::cout << "Archivos omitidos: " << archivosOmitidos << '\n';
    return 0;
}
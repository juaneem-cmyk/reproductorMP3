#include "LectorMetadatos.h"
#include <iostream>
#include <string>

int main(int numeroArgumentos, char* listaArgumentos[]) {
    if (numeroArgumentos != 2) {
        std::cerr << "Uso: minero <ruta-del-archivo.mp3>\n";
        return 1;
    }
    LectorMetadatosMP3 lector;
    MetadatosMusicales metadatos;
    std::string mensajeError;

    if (!lector.leerArchivo(listaArgumentos[1], metadatos, mensajeError)) {
        std::cerr << "Error: " << mensajeError << '\n';
        return 1;
    }
    std::cout << "Metadatos del archivo\n";
    std::cout << "Título: " << metadatos.titulo << '\n';
    std::cout << "Artista: " << metadatos.artista << '\n';
    std::cout << "Álbum: " << metadatos.album << '\n';
    std::cout << "Año: " << metadatos.year << '\n';
    std::cout << "Número de pista: " << metadatos.numeroPista << '\n';
    std::cout << "Duración: " << metadatos.duracionSegundos << " segundos\n";
    return 0;
}
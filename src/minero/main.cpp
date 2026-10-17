#include "LectorMetadatos.h"
#include "ExplorarDirectorios.h"
#include "InicializarBD.h"
#include <filesystem>
#include <iostream>
#include <cctype>
#include <string>
#include <vector>

/* Programa principal que permite al usuario indicar un archivo MP3 o un directorio.
    Si se indica un archivo, se leen sus metadatos y se muestran en pantalla.
    Si se indica un directorio, se buscan todos los archivos con extensión .mp3
    y se leen sus metadatos, mostrando los resultados en pantalla. */
int main(int numeroArgumentos, char* listaArgumentos[]) {
    if (numeroArgumentos == 2 && std::string(listaArgumentos[1]) == "--inicializar-bd") {
    InicializadorBaseDatos inicializador;
    bool baseDatosCreada = false;
    std::string mensajeError;
    const std::string rutaBaseDatos = "biblioteca_musical.db";
    const std::string rutaEsquema = "src/baseDatos/rolas.sql";

    // Intentar inicializar la base de datos SQLite, creando el archivo y ejecutando el esquema si es necesario
    if (!inicializador.inicializar(rutaBaseDatos, rutaEsquema, baseDatosCreada, mensajeError)) {
        std::cerr << "Error al inicializar SQLite: " << mensajeError << '\n';
        return 1;
    }

    // Informar al usuario si la base de datos fue creada o si ya existía
    if (baseDatosCreada) {
        std::cout << "Base de datos inicializada correctamente.\n";
    } else {
        std::cout << "La base de datos ya existía.\n";
    }
    return 0;
    }
    if (numeroArgumentos != 2) {
        std::cerr << "Uso:\n" << "  minero <archivo.mp3 | directorio>\n" << "  minero --inicializar-bd\n";        
        return 1;
    }
    std::filesystem::path ruta = listaArgumentos[1];
    std::vector<std::filesystem::path> archivosMP3;
    std::error_code error;

    // Se comprueba si la ruta es un archivo regular o un directorio válido
    if (std::filesystem::is_regular_file(ruta, error) && !error) {
        std::string extension = ruta.extension().string();

        // Convertir la extensión a minúsculas para la comparación
        for (char& caracter : extension) {
            caracter = static_cast<char>(std::tolower(static_cast<unsigned char>(caracter)));
        }

        // Si la ruta es un archivo regular, se verifica que tenga extensión .mp3
        if (extension != ".mp3") {
            std::cerr << "Error: el archivo indicado no tiene extensión MP3.\n";
            return 1;
        }
        archivosMP3.push_back(ruta);
    // Si es un archivo regular con extensión .mp3, se agrega a la lista de archivos a procesar
    } else {
        error.clear();

        // Si la ruta no es un archivo regular, se asume que es un directorio y se buscan archivos MP3 en él
        if (!std::filesystem::is_directory(ruta, error) || error) {
            std::cerr << "Error: la ruta no es un archivo regular ni un directorio " << "válido o no se puede acceder.\n";
            return 1;
        }
        ExplorarDirectorios explorador;
        archivosMP3 = explorador.buscarMP3(ruta);
    }

    // Si no se encontraron archivos MP3, se informa al usuario y se termina el programa
    if (archivosMP3.empty()) {
        std::cout << "No se encontraron archivos MP3.\n";
        return 0;
    }
    std::cout << "Se encontraron " << archivosMP3.size() << " archivos con extensión MP3.\n\n";
    LectorMetadatosMP3 lector;
    unsigned int archivosProcesados = 0;
    unsigned int archivosOmitidos = 0;

    // Se procesan los archivos MP3 encontrados, leyendo sus metadatos y mostrando la información en pantalla
    for (const auto& rutaArchivo : archivosMP3) {
        MetadatosMusicales metadatos;
        std::string mensajeError;
        std::cout << "Archivo: " << rutaArchivo << '\n';

        // Se intenta leer los metadatos del archivo MP3. Si falla, se omite y se muestra un mensaje de error
        if (!lector.leerArchivo(rutaArchivo.string(), metadatos, mensajeError)) {
            archivosOmitidos++;
            std::cerr << "Omitido: " << mensajeError << "\n\n";
            continue;
        }
        // Si se leen correctamente los metadatos, se muestran en pantalla
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
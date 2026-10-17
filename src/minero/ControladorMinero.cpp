#include "ControladorMinero.h"
#include "ExplorarDirectorios.h"
#include "InicializarBD.h"
#include "LectorMetadatos.h"
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int ControladorMinero::ejecutar(int numeroArgumentos, char* listaArgumentos[]) const {
    // Comprobar si se ha solicitado la inicialización de la base de datos
    if (numeroArgumentos == 2 && std::string(listaArgumentos[1]) == "--inicializar-bd") {
        return inicializarBaseDatos();
    }

    // Comprobar si se ha proporcionado un archivo o directorio como argumento
    if (numeroArgumentos != 2) {
        std::cerr << "Uso:\n" << "  minero <archivo.mp3 | directorio>\n" << "  minero --inicializar-bd\n";
        return 1;
    }
    return procesarRuta(listaArgumentos[1]);
}

int ControladorMinero::inicializarBaseDatos() const {
    const std::string rutaBaseDatos = "biblioteca_musical.db";
    const std::string rutaEsquema = "src/baseDatos/rolas.sql";
    InicializadorBaseDatos inicializador;
    bool baseDatosCreada = false;
    std::string mensajeError;

    // Intentar inicializar la base de datos
    if (!inicializador.inicializar(rutaBaseDatos, rutaEsquema, baseDatosCreada, mensajeError)) {
        std::cerr << "Error al inicializar SQLite: " << mensajeError << '\n';
        return 1;
    }

    // Informar al usuario sobre el resultado de la inicialización
    if (baseDatosCreada) {
        std::cout << "Base de datos inicializada correctamente.\n";
    } else {
        std::cout << "La base de datos ya existía.\n";
    }
    return 0;
}

int ControladorMinero::procesarRuta(const std::string& argumento) const {
    namespace fs = std::filesystem;
    const fs::path ruta(argumento);
    std::vector<fs::path> archivosMP3;
    std::error_code error;

    // Comprobar si la ruta es un archivo regular o un directorio
    if (fs::is_regular_file(ruta, error) && !error) {
        std::string extension = ruta.extension().string();

        // Convertir la extensión a minúsculas para la comparación
        for (char& caracter : extension) {
            caracter = static_cast<char>(std::tolower(static_cast<unsigned char>(caracter)));
        }

        // Comprobar si el archivo tiene la extensión .mp3
        if (extension != ".mp3") {
            std::cerr << "Error: el archivo indicado no tiene " << "extensión MP3.\n";
            return 1;
        }
        archivosMP3.push_back(ruta);
    } else {
        error.clear();

        // Comprobar si la ruta es un directorio válido
        if (!fs::is_directory(ruta, error) || error) {
            std::cerr << "Error: la ruta no es un archivo regular " << "ni un directorio válido o no se puede acceder.\n";
            return 1;
        }
        ExplorarDirectorios explorador;
        archivosMP3 = explorador.buscarMP3(ruta);
    }

    // Comprobar si se encontraron archivos MP3 en la ruta proporcionada
    if (archivosMP3.empty()) {
        std::cout << "No se encontraron archivos MP3.\n";
        return 0;
    }
    std::cout << "Se encontraron " << archivosMP3.size() << " archivos con extensión MP3.\n\n";
    LectorMetadatosMP3 lector;
    unsigned int archivosProcesados = 0;
    unsigned int archivosOmitidos = 0;

    // Procesar cada archivo MP3 encontrado y leer sus metadatos
    for (const auto& rutaArchivo : archivosMP3) {
        MetadatosMusicales metadatos;
        std::string mensajeError;
        std::cout << "Archivo: " << rutaArchivo << '\n';

        // Intentar leer los metadatos del archivo MP3
        if (!lector.leerArchivo(rutaArchivo.string(), metadatos, mensajeError)) {
            ++archivosOmitidos;
            std::cerr << "Omitido: " << mensajeError << "\n\n";
            continue;
        }
        // Mostrar los metadatos obtenidos del archivo MP3
        std::cout << "Título: " << metadatos.titulo << '\n';
        std::cout << "Artista: " << metadatos.artista << '\n';
        std::cout << "Álbum: " << metadatos.album << '\n';
        std::cout << "Año: " << metadatos.year << '\n';
        std::cout << "Número de pista: " << metadatos.numeroPista << '\n';
        std::cout << "Duración: " << metadatos.duracionSegundos << " segundos\n\n";
        ++archivosProcesados;
    }
    std::cout << "Archivos procesados correctamente: " << archivosProcesados << '\n';
    std::cout << "Archivos omitidos: " << archivosOmitidos << '\n';
    return 0;
}
#include "ControladorMinero.h"
#include "ExplorarDirectorios.h"
#include "LectorMetadatos.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include "repositorio/RepositorioMusical.h"
#include <cctype>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

int ControladorMinero::ejecutar(int numeroArgumentos, char* listaArgumentos[]) const {
    // Comprobar si se proporcionó un archivo o directorio
    if (numeroArgumentos != 2) {
        std::cerr << "Uso:\n" << "  minero <archivo.mp3 | directorio>\n";
        return 1;
    }
    return procesarRuta(listaArgumentos[1]);
}

// Procesa la ruta proporcionada y busca archivos MP3
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
    // Abrir la conexión con la base de datos de la biblioteca
    ConexionSQLite conexion;
    std::string mensajeErrorBD;

    if (!conexion.abrir(Rutas::rutaBaseDatos().string(), mensajeErrorBD)) {
        std::cerr << "Error al abrir SQLite: " << mensajeErrorBD << '\n';
        return 1;
    }

    // Activar las claves foráneas para esta conexión
    if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", mensajeErrorBD)) {
        std::cerr << "Error al activar las claves foráneas: " << mensajeErrorBD << '\n';
        return 1;
    }
    // Crear el repositorio utilizando la conexión existente
    RepositorioMusical repositorio(conexion);
    std::cout << "Se encontraron " << archivosMP3.size() << " archivos con extensión MP3.\n\n";
    LectorMetadatosMP3 lector;
    unsigned int archivosProcesados = 0;
    unsigned int archivosOmitidos = 0;
    unsigned int cancionesInsertadas = 0;
    unsigned int cancionesExistentes = 0;
    unsigned int erroresRegistro = 0;

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
        std::cout << "Género: " << metadatos.genero << '\n';
        std::cout << "Número de pista: " << metadatos.numeroPista << '\n';
        std::cout << "Duración: " << metadatos.duracionSegundos << " segundos\n\n";
        // Mostrar los metadatos adicionales
        std::cout << "Artista del álbum: " << metadatos.artistaAlbum << '\n';
        std::cout << "Texto de pista: " << metadatos.textoPista << '\n';
        std::cout << "Total de pistas: " << metadatos.totalPistas << '\n';
        std::cout << "Texto de disco: " << metadatos.textoDisco << '\n';
        std::cout << "Número de disco: " << metadatos.numeroDisco << '\n';
        std::cout << "Total de discos: " << metadatos.totalDiscos << '\n';
        std::cout << "Compositor: " << metadatos.compositor << '\n';
        std::cout << "Artista original: " << metadatos.artistaOriginal << '\n';
        std::cout << "Comentarios encontrados: " << metadatos.comentarios.size() << '\n';
        // Mostrar los comentarios encontrados en los metadatos
        for (const auto& comentario : metadatos.comentarios) {
            std::cout << " Comentario: " << comentario.texto << '\n';
        }
        // Mostrar información descriptiva de las imágenes
        std::cout << "Portada frontal: " << metadatos.portadaFrontal.descripcion << 
        " (" << metadatos.portadaFrontal.datos.size() << " bytes)\n";
        std::cout << "Imagen del artista: " << metadatos.imagenArtista.descripcion
        << " (" << metadatos.imagenArtista.datos.size() << " bytes)\n";

        // Registrar la canción en SQLite si todavía no existe
        bool insertada = false;
        std::string mensajeErrorRegistro;

        if (!repositorio.registrarCancion(rutaArchivo.string(), metadatos, insertada, mensajeErrorRegistro)) {
            ++erroresRegistro;               
            std::cerr << "Error al registrar la canción: " << mensajeErrorRegistro << "\n\n";
        } else if (insertada) {
            ++cancionesInsertadas;
            std::cout << "Canción registrada en SQLite.\n\n";
        } else {
            ++cancionesExistentes;
            std::cout << "La canción ya estaba registrada.\n\n";
        }
        ++archivosProcesados;
    }
    std::cout << "Archivos procesados correctamente: " << archivosProcesados << '\n';
    std::cout << "Archivos omitidos: " << archivosOmitidos << '\n';
    std::cout << "Canciones nuevas registradas: " << cancionesInsertadas << '\n';
    std::cout << "Canciones que ya existían: " << cancionesExistentes << '\n';
    std::cout << "Errores de registro en SQLite: " << erroresRegistro << '\n';
    return 0;
}
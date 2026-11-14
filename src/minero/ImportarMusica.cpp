#include "ImportarMusica.h"
#include "ExplorarDirectorios.h"
#include "LectorMetadatos.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include "repositorio/RepositorioMusical.h"
#include <algorithm>
#include <cctype>
#include <filesystem>
#include <system_error>
#include <vector>

bool ImportarMusica::importar(const std::filesystem::path& ruta, ResultadoMinado& resultado,
    std::string& mensajeError) const {
    namespace fs = std::filesystem;
    resultado = ResultadoMinado{};
    mensajeError.clear();
    std::vector<fs::path> archivosMP3;
    std::error_code error;

    // Comprobar si la ruta corresponde a un archivo regular
    if (fs::is_regular_file(ruta, error) && !error) {
        std::string extension = ruta.extension().string();

        std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char caracter) {
                return static_cast<char>(std::tolower(caracter));
            }
        );

        if (extension != ".mp3") {
            mensajeError = "El archivo indicado no tiene extensión MP3.";
            return false;
        }
        archivosMP3.push_back(ruta);
    } else {
        error.clear();

        // Comprobar si la ruta corresponde a un directorio
        if (!fs::is_directory(ruta, error) || error) {
            mensajeError = "La ruta no es un archivo regular ni un directorio válido.";
            return false;
        }
        ExplorarDirectorios explorador;
        archivosMP3 = explorador.buscarMP3(ruta);
    }
    resultado.archivosEncontrados = archivosMP3.size();

    // No hay trabajo que realizar si no se encontraron archivos
    if (archivosMP3.empty()) {
        return true;
    }

    // Comprobar que la base de datos ya exista
    const fs::path rutaBaseDatos = Rutas::rutaBaseDatos();

    if (rutaBaseDatos.empty()) {
        mensajeError = "No se pudo determinar la ubicación de la biblioteca.";
        return false;
    }

    if (!fs::exists(rutaBaseDatos, error)) {
        mensajeError = error ? "Error al comprobar la base de datos: " + error.message()
            : "La base de datos no existe. Inicialízala primero.";
        return false;
    }

    if (!fs::is_regular_file(rutaBaseDatos, error) || error) {
        mensajeError = "La ruta de la base de datos no es un archivo válido.";
        return false;
    }

    // Abrir SQLite
    ConexionSQLite conexion;

    if (!conexion.abrir(rutaBaseDatos.string(), mensajeError)) {
        return false;
    }

    // Activar las claves foráneas
    if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", mensajeError)) {
        return false;
    }
    RepositorioMusical repositorio(conexion);
    LectorMetadatosMP3 lector;

    // Procesar los archivos encontrados
    for (const auto& rutaArchivo : archivosMP3) {
        std::error_code errorRuta;
        const fs::path rutaNormalizada = fs::absolute(rutaArchivo, errorRuta).lexically_normal();

        if (errorRuta) {
            ++resultado.archivosOmitidos;
            continue;
        }

        // Comprobar si la canción ya está registrada
        bool yaRegistrada = false;
        std::string errorConsulta;

        if (!repositorio.existeCancionPorRuta(rutaNormalizada.string(), yaRegistrada, errorConsulta)) {
            ++resultado.erroresRegistro;
            continue;
        }

        if (yaRegistrada) {
            ++resultado.archivosProcesados;
            ++resultado.cancionesExistentes;
            continue;
        }

        // Leer los metadatos del MP3
        MetadatosMusicales metadatos;
        std::string errorLectura;

        if (!lector.leerArchivo(rutaArchivo.string(), metadatos, errorLectura)) {
            ++resultado.archivosOmitidos;
            continue;
        }

        // Registrar la canción en SQLite
        bool insertada = false;
        std::string errorRegistro;

        if (!repositorio.registrarCancion(rutaNormalizada.string(), metadatos,
            insertada, errorRegistro)) {
            ++resultado.erroresRegistro;
        } else if (insertada) {
            ++resultado.cancionesInsertadas;
        } else {
            ++resultado.cancionesExistentes;
        }
        ++resultado.archivosProcesados;
    }
    return true;
}
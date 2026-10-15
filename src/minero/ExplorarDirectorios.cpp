#include "ExplorarDirectorios.h"
#include <algorithm>
#include <cctype>
#include <system_error>

/* Busca archivos MP3 en un directorio y sus subdirectorios.
   Devuelve un vector con las rutas de los archivos encontrados. 
   Si no se puede acceder al directorio o no se encuentran archivos, devuelve un vector vacío. */ 
std::vector<std::filesystem::path>
ExplorarDirectorios::buscarMP3(const std::filesystem::path& directorio) const {
    std::vector<std::filesystem::path> archivosMP3;
    std::error_code error;

    // Se comprueba si la ruta es un directorio válido antes de iniciar la búsqueda
    if (!std::filesystem::is_directory(directorio, error) || error) {
        return archivosMP3;
    }
    std::filesystem::recursive_directory_iterator iterador(directorio, std::filesystem::directory_options::skip_permission_denied, error);
    const std::filesystem::recursive_directory_iterator fin;

    // Si no se pudo iniciar el recorrido, se devuelve la lista vacía
    if (error) {
        return archivosMP3;
    }

    // Se recorre el directorio y sus subdirectorios buscando archivos con extensión .mp3
    while (iterador != fin) {
        std::error_code errorArchivo;

        // Se verifica si el elemento actual es un archivo regular y se obtiene su extensión
        if (iterador->is_regular_file(errorArchivo) && !errorArchivo) {
            std::string extension = iterador->path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char caracter) {
                return static_cast<char>(std::tolower(caracter));
                });

            // Si la extensión es .mp3, se agrega la ruta del archivo a la lista de resultados
            if (extension == ".mp3") {
                archivosMP3.push_back(iterador->path());
            }
        }
        iterador.increment(error);
        // Si ocurre un error al intentar avanzar el iterador, se limpia el error y se continúa con el siguiente elemento
        if (error) {
            error.clear();
        }
    }
    return archivosMP3;
}
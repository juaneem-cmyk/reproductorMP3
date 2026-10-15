#include "ExplorarDirectorios.h"
#include <algorithm>
#include <cctype>
#include <system_error>

std::vector<std::filesystem::path>
ExplorarDirectorios::buscarMP3(const std::filesystem::path& directorio) const {
    std::vector<std::filesystem::path> archivosMP3;
    std::error_code error;

    if (!std::filesystem::is_directory(directorio, error) || error) {
        return archivosMP3;
    }
    std::filesystem::recursive_directory_iterator iterador(directorio, std::filesystem::directory_options::skip_permission_denied, error);
    const std::filesystem::recursive_directory_iterator fin;

    // Si no se pudo iniciar el recorrido, se devuelve la lista vacía
    if (error) {
        return archivosMP3;
    }

    while (iterador != fin) {
        std::error_code errorArchivo;

        if (iterador->is_regular_file(errorArchivo) && !errorArchivo) {
            std::string extension = iterador->path().extension().string();
            std::transform(extension.begin(), extension.end(), extension.begin(), [](unsigned char caracter) {
                return static_cast<char>(std::tolower(caracter));
                });

            if (extension == ".mp3") {
                archivosMP3.push_back(iterador->path());
            }
        }
        iterador.increment(error);
        if (error) {
            error.clear();
        }
    }
    return archivosMP3;
}
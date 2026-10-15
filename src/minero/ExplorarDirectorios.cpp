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
    std::filesystem::recursive_directory_iterator fin;

    while (iterador != fin) {
        if (error) {
            error.clear();
            iterador.increment(error);
            continue;
        }
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
    }
    return archivosMP3;
}

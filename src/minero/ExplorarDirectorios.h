
#ifndef EXPLORAR_DIRECTORIOS_H
#define EXPLORAR_DIRECTORIOS_H

#include <filesystem>
#include <vector>
#include <string>

class ExplorarDirectorios {
public:
    // Busca archivos MP3 en un directorio y sus subdirectorios.
    std::vector<std::filesystem::path> buscarMP3(const std::filesystem::path& directorio) const;
};

#endif

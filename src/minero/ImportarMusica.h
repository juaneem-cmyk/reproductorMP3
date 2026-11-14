#ifndef IMPORTAR_MUSICA_H
#define IMPORTAR_MUSICA_H

#include <filesystem>
#include <cstddef>
#include <string>

struct ResultadoMinado {
    std::size_t archivosEncontrados = 0;
    std::size_t archivosProcesados = 0;
    std::size_t archivosOmitidos = 0;
    std::size_t cancionesInsertadas = 0;
    std::size_t cancionesExistentes = 0;
    std::size_t erroresRegistro = 0;
};

class ImportarMusica {
public:
    bool importar(const std::filesystem::path& ruta, ResultadoMinado& resultado,
        std::string& mensajeError) const;
};
#endif
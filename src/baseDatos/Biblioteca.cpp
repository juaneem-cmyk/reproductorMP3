#include "Biblioteca.h"
#include "InicializarBD.h"
#include "Rutas.h"

// Preparar el directorio y asegurar que exista la base de datos
bool Biblioteca::preparar(bool& baseDatosCreada, std::string& mensajeError) const {
    baseDatosCreada = false;

    // Preparar el directorio donde se guardan los datos
    if (!Rutas::prepararDirectorioDatos(mensajeError)) {
        return false;
    }
    // Utilizar el inicializador existente para crear la base de datos
    InicializadorBaseDatos inicializador;
    return inicializador.inicializar(Rutas::rutaBaseDatos().string(), Rutas::rutaEsquemaSQL().string(), baseDatosCreada, mensajeError);
}
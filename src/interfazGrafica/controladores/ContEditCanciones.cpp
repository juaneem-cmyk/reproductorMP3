#include "ContEditCanciones.h"
#include "EdicionCanciones.h"
#include "../paneles/PanelCanciones.h"
#include "../modelos/Canciones.h"
#include "../ventanas/VentanaEdicion.h"
#include "../servicios/GestorImagenes.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include "EdicionCanciones.h"
#include <QMessageBox>
#include <QToolButton>
#include <QWidget>

ControladorEdicionCanciones::ControladorEdicionCanciones(QToolButton *botonEditarCancion, PanelCanciones *panelCanciones, 
    QWidget *ventanaPadre) : QObject(ventanaPadre), panelCanciones(panelCanciones), ventanaPadre(ventanaPadre) {
    connect(botonEditarCancion, &QToolButton::clicked, this, [this]() {
        editarCancion();
    });
}

void ControladorEdicionCanciones::editarCancion() {
    CancionVista cancion;

    if (!panelCanciones->CancionSelect(cancion)) {
        return;
    }

    // Abrir la ventana con los datos actuales
    VentanaEdit ventana(cancion, ventanaPadre);

    if (ventana.exec() != QDialog::Accepted) {
        return;
    }

    // Abrir la conexión con SQLite
    ConexionSQLite conexion;
    std::string mensajeError;

    if (!conexion.abrir(Rutas::rutaBaseDatos().string(), mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos", QString::fromStdString(mensajeError));
        return;
    }

    // Activar las claves foráneas
    if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos", QString::fromStdString(mensajeError));
        return;
    }

    // Actualizar los datos de la canción
    EdicionCanciones editor(conexion);
    bool actualizado = editor.actualizarCancion(cancion.idRola,
        ventana.obtenerTitulo().toStdString(), ventana.obtenerArtista().toStdString(),
        ventana.obtenerAlbum().toStdString(), ventana.obtenerYear(), mensajeError
    );

    if (!actualizado) {
        QMessageBox::critical(ventanaPadre, "Error al editar canción", QString::fromStdString(mensajeError));
        return;
    }

    // Guardar o restaurar la portada personalizada, si el usuario lo solicitó
    QString errorImagen;
    bool imagenActualizada = true;

    if (!ventana.obtenerRutaImagenSeleccionada().isEmpty()) {
        imagenActualizada = GestorImagenes::guardarCancion(cancion.idRola, ventana.obtenerRutaImagenSeleccionada(), errorImagen);
    } else if (ventana.debeEliminarImagenPersonalizada()) {
        imagenActualizada = GestorImagenes::eliminarCancion(cancion.idRola, errorImagen);
    }

    // Actualizar la tabla
    panelCanciones->recargarCanciones();

    if (!imagenActualizada) {
        QMessageBox::warning(ventanaPadre, "Canción actualizada",
            "Los datos de la canción se guardaron, pero no se pudo actualizar la portada:\n" + errorImagen);
        return;
    }

    QMessageBox::information(ventanaPadre, "Canción actualizada", 
    "Los cambios se guardaron correctamente.");
}
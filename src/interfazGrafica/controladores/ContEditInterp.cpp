#include "ContEditInterp.h"
#include "EdicionInterp.h"
#include "../paneles/PanelCanciones.h"
#include "../modelos/Canciones.h"
#include "../ventanas/VentanaInterp.h"
#include "../servicios/GestorImagenes.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include <QMessageBox>
#include <QToolButton>
#include <QWidget>

ControladorEditInterp::ControladorEditInterp(QToolButton *botonEditarInterprete, PanelCanciones *panelCanciones,
    QWidget *ventanaPadre) : QObject(ventanaPadre), panelCanciones(panelCanciones), ventanaPadre(ventanaPadre) {
    connect(botonEditarInterprete, &QToolButton::clicked, this, [this]() {
        editarInterprete();
    });
}

void ControladorEditInterp::editarInterprete() {
    CancionVista cancion;

    if (!panelCanciones->CancionSelect(cancion)) {
        return;
    }

    if (cancion.idInterprete <= 0) {
        QMessageBox::warning(ventanaPadre, "Sin intérprete",
            "La canción seleccionada no tiene un intérprete asociado.");
        return;
    }

    // Abrir la ventana con los datos actuales y la imagen del intérprete
    VentanaInterprete ventana(cancion, ventanaPadre);

    if (ventana.exec() != QDialog::Accepted) {
        return;
    }

    // Abrir la conexión con SQLite
    ConexionSQLite conexion;
    std::string mensajeError;

    if (!conexion.abrir(Rutas::rutaBaseDatos().string(), mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos",
            QString::fromStdString(mensajeError));
        return;
    }

    // Activar las claves foráneas
    if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos",
            QString::fromStdString(mensajeError));
        return;
    }

    // Actualizar el intérprete seleccionado
    EdicionInterpretes editor(conexion);
    bool actualizado = editor.actualizarInterprete(
        cancion.idInterprete,
        ventana.obtenerNombre().toStdString(),
        ventana.obtenerTipo(),
        mensajeError
    );

    if (!actualizado) {
        QMessageBox::critical(ventanaPadre, "Error al editar intérprete",
            QString::fromStdString(mensajeError));
        return;
    }

    // Guardar o restaurar la imagen personalizada, si el usuario lo solicitó
    QString errorImagen;
    bool imagenActualizada = true;

    if (!ventana.obtenerRutaImagenSeleccionada().isEmpty()) {
        imagenActualizada = GestorImagenes::guardarInterprete(cancion.idInterprete, 
        ventana.obtenerRutaImagenSeleccionada(), errorImagen);
    } else if (ventana.debeEliminarImagenPersonalizada()) {
        imagenActualizada = GestorImagenes::eliminarInterprete(cancion.idInterprete, errorImagen);
    }

    // Actualizar la tabla
    panelCanciones->recargarCanciones();

    if (!imagenActualizada) {
        QMessageBox::warning(ventanaPadre, "Intérprete actualizado",
            "Los datos del intérprete se guardaron, pero no se pudo actualizar su imagen:\n" + errorImagen);
        return;
    }

    QMessageBox::information(ventanaPadre, "Intérprete actualizado",
        "Los cambios se guardaron correctamente.");
}
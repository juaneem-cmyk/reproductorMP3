#include "ContEditInterp.h"
#include "EdicionInterp.h"
#include "../paneles/PanelCanciones.h"
#include "../modelos/Canciones.h"
#include "../ventanas/VentanaInterp.h"
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

    // Abrir la ventana con los datos actuales
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

    // Actualizar la tabla
    panelCanciones->recargarCanciones();
    QMessageBox::information(ventanaPadre, "Intérprete actualizado",
        "Los cambios se guardaron correctamente.");
}
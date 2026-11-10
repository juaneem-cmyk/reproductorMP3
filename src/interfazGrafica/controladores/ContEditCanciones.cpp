#include "ContEditCanciones.h"
#include "EdicionCanciones.h"
#include "../paneles/PanelCanciones.h"
#include "../modelos/Canciones.h"
#include "../ventanas/VentanaEdicion.h"
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

    // Actualizar la tabla
    panelCanciones->recargarCanciones();
    QMessageBox::information(ventanaPadre, "Canción actualizada",
        "Los cambios se guardaron correctamente.");
}
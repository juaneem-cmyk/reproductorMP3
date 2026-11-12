#include "ContEditAlbumes.h"
#include "EdicionAlbumes.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include "../paneles/PanelCanciones.h"
#include "../ventanas/VentanaAlbumes.h"
#include <QDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QWidget>
#include <vector>

ControladorEditAlbumes::ControladorEditAlbumes(QToolButton *botonAdministrarAlbumes,
    PanelCanciones *panelCanciones, QWidget *ventanaPadre)
    : QObject(ventanaPadre), panelCanciones(panelCanciones), ventanaPadre(ventanaPadre) {
    connect(botonAdministrarAlbumes, &QToolButton::clicked, this, [this]() {
        editarAlbumes();
    });
}

// Función para editar álbumes asociados a canciones
void ControladorEditAlbumes::editarAlbumes() {
    ConexionSQLite conexion;
    std::string mensajeError;

    if (!conexion.abrir(Rutas::rutaBaseDatos().string(), mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos", QString::fromStdString(mensajeError));
        return;
    }

    if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos", QString::fromStdString(mensajeError));
        return;
    }

    // Obtener los álbumes existentes asociados a canciones
    EdicionAlbumes editor(conexion);
    std::vector<DatosAlbum> albumes;
    if (!editor.obtenerAlbumes(albumes, mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al consultar álbumes", QString::fromStdString(mensajeError));
        return;
    }

    if (albumes.empty()) {
        QMessageBox::information(ventanaPadre, "Sin álbumes",
            "Todavía no hay álbumes asociados a canciones para administrar.");
        return;
    }

    // Abrir la ventana de edición de álbumes
    VentanaAlbumes ventana(albumes, ventanaPadre);
    if (ventana.exec() != QDialog::Accepted) {
        return;
    }

    if (!editor.actualizarAlbumes(ventana.obtenerAlbumes(), mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al actualizar álbumes", QString::fromStdString(mensajeError));
        return;
    }
    panelCanciones->recargarCanciones();
    QMessageBox::information(ventanaPadre, "Álbumes actualizados",
        "Los cambios se guardaron correctamente en la base de datos.");
}
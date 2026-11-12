#include "ContEditInterp.h"
#include "EdicionInterp.h"
#include "CrearInterp.h"
#include "../paneles/PanelCanciones.h"
#include "../modelos/Canciones.h"
#include "../ventanas/VentanaInterp.h"
#include "../servicios/GestorImagenes.h"
#include "ConexionSQLite.h"
#include "Rutas.h"
#include <QDialog>
#include <QMessageBox>
#include <QToolButton>
#include <QWidget>
#include <vector>

ControladorEditInterp::ControladorEditInterp(QToolButton *botonEditarInterprete,
    PanelCanciones *panelCanciones, QWidget *ventanaPadre)
    : QObject(ventanaPadre), panelCanciones(panelCanciones), ventanaPadre(ventanaPadre) {
    connect(botonEditarInterprete, &QToolButton::clicked, this, [this]() {
        editarInterprete();
    });
}

// Función para editar el intérprete de la canción seleccionada en el panel de canciones
void ControladorEditInterp::editarInterprete() {
    CancionVista cancion;

    if (!panelCanciones->CancionSelect(cancion)) {
        agregarInterprete();
        return;
    }

    if (cancion.idInterprete <= 0) {
        QMessageBox::warning(ventanaPadre, "Sin intérprete",
            "La canción seleccionada no tiene un intérprete asociado.");
        return;
    }

    // Abrir la conexión antes de mostrar el editor para cargar los datos existentes
    ConexionSQLite conexion;
    std::string mensajeError;

    if (!conexion.abrir(Rutas::rutaBaseDatos().string(), mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos",
            QString::fromStdString(mensajeError));
        return;
    }

    if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos",
            QString::fromStdString(mensajeError));
        return;
    }
    EdicionInterpretes editor(conexion);
    DatosPersona datosPersona;
    DatosGrupo datosGrupo;
    std::vector<DatosIntegrante> integrantes;
    std::vector<DatosIntegrante> personasDisponibles;

    // Obtener los datos del intérprete y del grupo, así como los integrantes y las personas disponibles
    if (!editor.obtenerPersona(cancion.idInterprete, datosPersona, mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al consultar intérprete",
            QString::fromStdString(mensajeError));
        return;
    }

    // Reparar registros heredados: un grupo puede existir sin integrantes ni fechas
    if (cancion.idTipoInterprete == 1
        && !editor.asegurarRegistroGrupo(cancion.idInterprete, mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al consultar grupo",
            QString::fromStdString(mensajeError));
        return;
    }

    if (!editor.obtenerGrupo(cancion.idInterprete, datosGrupo, mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al consultar grupo",
            QString::fromStdString(mensajeError));
        return;
    }

    // Obtener los integrantes del grupo y las personas disponibles para agregar
    if (!editor.obtenerIntegrantes(cancion.idInterprete, integrantes, mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al consultar integrantes",
            QString::fromStdString(mensajeError));
        return;
    }

    if (!editor.obtenerPersonasDisponibles(personasDisponibles, mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al consultar personas",
            QString::fromStdString(mensajeError));
        return;
    }
    VentanaInterprete ventana(cancion, datosPersona, datosGrupo, integrantes, personasDisponibles, ventanaPadre);

    if (ventana.exec() != QDialog::Accepted) {
        return;
    }
    DatosPersona nuevosDatosPersona;
    nuevosDatosPersona.nombreReal = ventana.obtenerNombreReal().toStdString();
    nuevosDatosPersona.fechaNacimiento = ventana.obtenerFechaNacimiento().toStdString();
    nuevosDatosPersona.fechaFallecimiento = ventana.obtenerFechaFallecimiento().toStdString();

    DatosGrupo nuevosDatosGrupo;
    nuevosDatosGrupo.fechaCreacion = ventana.obtenerFechaCreacion().toStdString();
    nuevosDatosGrupo.fechaSeparacion = ventana.obtenerFechaSeparacion().toStdString();

    int idInterpreteActualizado = 0;
    const bool actualizado = editor.actualizarInterprete(
        cancion.idInterprete,
        cancion.idRola,
        ventana.obtenerNombre().toStdString(),
        ventana.obtenerTipo(),
        nuevosDatosPersona,
        nuevosDatosGrupo,
        ventana.obtenerIdsIntegrantes(),
        idInterpreteActualizado,
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
    if (ventana.obtenerTipo() != 2) {
        if (!ventana.obtenerRutaImagenSeleccionada().isEmpty()) {
            imagenActualizada = GestorImagenes::guardarInterprete(idInterpreteActualizado, 
            ventana.obtenerRutaImagenSeleccionada(), errorImagen);
        } else if (ventana.debeEliminarImagenPersonalizada()) {
            imagenActualizada = GestorImagenes::eliminarInterprete(idInterpreteActualizado, errorImagen);
        }
    }
    panelCanciones->recargarCanciones();

    if (!imagenActualizada) {
        QMessageBox::warning(ventanaPadre, "Intérprete actualizado",
            "Los datos del intérprete se guardaron, pero no se pudo actualizar su imagen:\n"
            + errorImagen);
        return;
    }

    QMessageBox::information(ventanaPadre, "Intérprete actualizado",
        "Los cambios se guardaron correctamente."
    );
}

// Permite registrar una persona o grupo aunque todavía no interprete canciones
void ControladorEditInterp::agregarInterprete() {
    ConexionSQLite conexion;
    std::string mensajeError;

    if (!conexion.abrir(Rutas::rutaBaseDatos().string(), mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos",
            QString::fromStdString(mensajeError));
        return;
    }

    if (!conexion.ejecutarSQL("PRAGMA foreign_keys = ON;", mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error de base de datos",
            QString::fromStdString(mensajeError));
        return;
    }

    EdicionInterpretes editor(conexion);
    std::vector<DatosIntegrante> personasDisponibles;
    if (!editor.obtenerPersonasDisponibles(personasDisponibles, mensajeError)) {
        QMessageBox::critical(ventanaPadre, "Error al consultar personas",
            QString::fromStdString(mensajeError));
        return;
    }

    VentanaInterprete ventana(personasDisponibles, ventanaPadre);
    if (ventana.exec() != QDialog::Accepted) {
        return;
    }

    DatosPersona datosPersona;
    datosPersona.nombreReal = ventana.obtenerNombreReal().toStdString();
    datosPersona.fechaNacimiento = ventana.obtenerFechaNacimiento().toStdString();
    datosPersona.fechaFallecimiento = ventana.obtenerFechaFallecimiento().toStdString();

    DatosGrupo datosGrupo;
    datosGrupo.fechaCreacion = ventana.obtenerFechaCreacion().toStdString();
    datosGrupo.fechaSeparacion = ventana.obtenerFechaSeparacion().toStdString();

    const std::vector<int> integrantes = ventana.obtenerTipo() == 1
    ? ventana.obtenerIdsIntegrantes() : std::vector<int>();

    int idInterprete = 0;
    CrearInterpretes creador(conexion);
    if (!creador.crearInterprete(ventana.obtenerNombre().toStdString(), ventana.obtenerTipo(),
        datosPersona, datosGrupo, integrantes, idInterprete, mensajeError)) {
        QMessageBox::warning(ventanaPadre, "No se pudo agregar el intérprete",
            QString::fromStdString(mensajeError));
        return;
    }

    // La ficha se creó en SQLite y la imagen personalizada se guarda aparte
    QString errorImagen;
    if (!ventana.obtenerRutaImagenSeleccionada().isEmpty()
        && !GestorImagenes::guardarInterprete(idInterprete,
            ventana.obtenerRutaImagenSeleccionada(), errorImagen)) {
        QMessageBox::warning(ventanaPadre, "Intérprete registrado",
            "El intérprete se registró correctamente, pero no se pudo guardar su imagen:\n"
            + errorImagen);
        return;
    }

    QMessageBox::information(ventanaPadre, "Intérprete registrado",
        ventana.obtenerTipo() == 0
            ? "La persona se registró correctamente. Ya puede agregarse como integrante de un grupo o asignarse a una canción."
            : "El grupo se registró correctamente. Puedes agregar integrantes cuando estén disponibles o asignarlo a una canción.");
}
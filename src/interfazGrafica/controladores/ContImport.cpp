#include "ContImport.h"
#include "../paneles/PanelCanciones.h"
#include "ImportarMusica.h"
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMessageBox>
#include <QToolButton>
#include <QDir>
#include <QtConcurrent/QtConcurrentRun>
#include <filesystem>
#include <exception>

struct ResultadoImportacionUI {
    bool correcto = false;
    ResultadoMinado resultado;
    std::string mensajeError;
};

ControladorImport::ControladorImport(QToolButton *botonImportar,
    PanelCanciones *panelCanciones, QWidget *ventanaPadre)
    : QObject(ventanaPadre), panelCanciones(panelCanciones), ventanaPadre(ventanaPadre) {
    connect(botonImportar, &QToolButton::clicked, this, [this, botonImportar]() {
        importarMusica(botonImportar);
    });
}

void ControladorImport::importarMusica(QToolButton *botonImportar) {
    
    // Seleccionar el directorio que contiene la música
    QString directorio = QFileDialog::getExistingDirectory(ventanaPadre,
        "Seleccionar carpeta de música", QDir::homePath());

    if (directorio.isEmpty()) {
        return;
    }

    // Convertir la ruta de Qt a filesystem
    const QByteArray rutaUTF8 = directorio.toUtf8();
    const std::filesystem::path ruta = std::filesystem::u8path(rutaUTF8.constData());

    // Evitar que se inicie otra importación simultáneamente
    botonImportar->setEnabled(false);
    botonImportar->setToolTip("Importando música...");

    // Observar el trabajo en segundo plano
    auto *observador = new QFutureWatcher<ResultadoImportacionUI>(ventanaPadre);

    connect(observador, &QFutureWatcher<ResultadoImportacionUI>::finished,
        this, [this, observador, botonImportar]() {
            ResultadoImportacionUI salida = observador->result();
            observador->deleteLater();
            botonImportar->setEnabled(true);
            botonImportar->setToolTip("Importar música");

            // Mostrar un error si no se pudo completar la importación
            if (!salida.correcto) {
                QMessageBox::critical(ventanaPadre, "Error de importación",
                    QString::fromStdString(salida.mensajeError));
                return;
            }

            // Actualizar la biblioteca visible
            panelCanciones->recargarCanciones();
            const ResultadoMinado& r = salida.resultado;
            if (r.archivosEncontrados == 0) {
                QMessageBox::information(ventanaPadre, "Importación finalizada",
                    "No se encontraron archivos MP3 en el directorio seleccionado."
                );
                return;
            }

            // Construir el resumen
            QString resumen = QString(
                "Archivos encontrados: %1\n"
                "Archivos procesados: %2\n"
                "Canciones nuevas: %3\n"
                "Canciones existentes: %4\n"
                "Archivos omitidos: %5\n"
                "Errores de registro: %6"
            )
            .arg(static_cast<qulonglong>(r.archivosEncontrados))
            .arg(static_cast<qulonglong>(r.archivosProcesados))
            .arg(static_cast<qulonglong>(r.cancionesInsertadas))
            .arg(static_cast<qulonglong>(r.cancionesExistentes))
            .arg(static_cast<qulonglong>(r.archivosOmitidos))
            .arg(static_cast<qulonglong>(r.erroresRegistro));

            // Mostrar el resultado
            if (r.erroresRegistro > 0 || r.archivosOmitidos > 0) {
                QMessageBox::warning(ventanaPadre,
                    "Importación finalizada con observaciones", resumen);
            } else {
                QMessageBox::information(ventanaPadre,
                    "Importación completada", resumen);
            }
        }
    );

    // Ejecutar el importador fuera del hilo de la interfaz
    observador->setFuture(QtConcurrent::run([ruta]() {
        ResultadoImportacionUI salida;
        ImportarMusica importador;

        try {
            salida.correcto = importador.importar(ruta, salida.resultado, salida.mensajeError);
        } catch (const std::exception& error) {
            salida.correcto = false;
            salida.mensajeError = error.what();
        }
        return salida;
    }));
}
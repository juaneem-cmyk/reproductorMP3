#include "ControladoRep.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

ControladorReproduccion::ControladorReproduccion(QObject *parent) : QObject(parent) {
    reproductor = new QMediaPlayer(this);
    salidaAudio = new QAudioOutput(this);

    // Conectar el reproductor con la salida de audio
    reproductor->setAudioOutput(salidaAudio);
    salidaAudio->setVolume(0.5f);
}

// Carga un archivo de audio en el reproductor
void ControladorReproduccion::cargarCancion(const QString &ruta) {
    QUrl url = QUrl::fromLocalFile(ruta);
    reproductor->setSource(url);
}

// Controles de reproducción
void ControladorReproduccion::reproducir() {
    reproductor->play();
}

// Pausa la reproducción del audio
void ControladorReproduccion::pausar() {
    reproductor->pause();
}

// Detiene la reproducción del audio
void ControladorReproduccion::detener() {
    reproductor->stop();
}

// Control de volumen
void ControladorReproduccion::establecerVolumen(float volumen) {
    if (volumen < 0.0f)
        volumen = 0.0f;

    if (volumen > 1.0f)
        volumen = 1.0f;

    salidaAudio->setVolume(volumen);
}

// Información de reproducción
qint64 ControladorReproduccion::obtenerPosicion() const {
    return reproductor->position();
}

// Obtiene la duración total del audio en milisegundos
qint64 ControladorReproduccion::obtenerDuracion() const {
    return reproductor->duration();
}

// Verifica si el reproductor está actualmente reproduciendo audio
bool ControladorReproduccion::estaReproduciendo() const {
    return reproductor->playbackState() == QMediaPlayer::PlayingState;
}

// Establece la posición de reproducción en milisegundos
QMediaPlayer *ControladorReproduccion::obtenerReproductor() const {
    return reproductor;
}

void ControladorReproduccion::establecerPosicion(qint64 posicion) {
    reproductor->setPosition(posicion);
}
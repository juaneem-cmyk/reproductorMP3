#ifndef CONTROLADOR_REPRODUCCION_H
#define CONTROLADOR_REPRODUCCION_H

#include <QObject>
#include <QString>

class QMediaPlayer;
class QAudioOutput;

class ControladorReproduccion : public QObject {
public:
    explicit ControladorReproduccion(QObject *parent = nullptr);

    // Carga un archivo de audio
    void cargarCancion(const QString &ruta);

    // Controles de reproducción
    void reproducir();
    void pausar();
    void detener();

    // Control de volumen
    void establecerVolumen(float volumen);

    // Información de reproducción
    qint64 obtenerPosicion() const;
    qint64 obtenerDuracion() const;
    bool estaReproduciendo() const;

private:
    QMediaPlayer *reproductor;
    QAudioOutput *salidaAudio;
};
#endif
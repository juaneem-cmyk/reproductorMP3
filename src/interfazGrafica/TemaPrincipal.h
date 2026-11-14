#ifndef TEMA_PRINCIPAL_H
#define TEMA_PRINCIPAL_H

#include <QFont>
#include <QFontDatabase>
#include <QString>
#include <QStringList>

/* Recursos tipográficos compartidos exclusivamente por la ventana principal.
 Si el usuario instala Press Start 2P, se utilizará para los encabezados,
 en caso contrario se emplea una fuente monoespaciada disponible en Linux. */
namespace TemaPrincipal {

inline QString familiaPixelada() {
    const QStringList familias = QFontDatabase::families();

    for (const QString &familia : familias) {
        if (familia.compare(QStringLiteral("Press Start 2P"), Qt::CaseInsensitive) == 0) {
            return familia;
        }
    }
    return QStringLiteral("DejaVu Sans Mono");
}

inline QFont fuenteEncabezado(int puntos = 9) {
    QFont fuente(familiaPixelada());
    fuente.setPointSize(puntos);
    fuente.setBold(true);
    return fuente;
}

}
#endif
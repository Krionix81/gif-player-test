#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "splash_widget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "gif-player-test_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    SplashWidget w(QStringLiteral(":/movies/resources/ddc-movie.gif"),
                   QStringLiteral(":/images/resources/ddc_bk.png"));
    w.show();
    return a.exec();
}

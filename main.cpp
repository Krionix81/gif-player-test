#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include "video_splash.h"

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

    SplashWidget splash(QStringLiteral("qrc:/movies/resources/movies/ddc-splash.mp4"),
                        QStringLiteral(":/images/resources/images/ddc_bk.png"));
    splash.show();

    return a.exec();
}

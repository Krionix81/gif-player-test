#include <QApplication>
#include <QDialog>
#include <QLocale>
#include <QSplashScreen>
#include <QTimer>
#include <QTranslator>
#include <QVBoxLayout>
#include "video_splash.h"

int main(int argc, char *argv[])
{
    qputenv("QT_ENABLE_HIGHDPI_SCALING", "0");

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

    enum class AppType { MTR = 0, EMU, DDC, TMS, WDP, UTL, SC, GLA };
    auto ctrlDlg = std::make_unique<QDialog>(nullptr);
    auto lt = new QVBoxLayout(ctrlDlg.get());

    auto splash = std::make_unique<SplashWidget>(QStringLiteral(
                                                     "qrc:/movies/resources/movies/ddc-splash.mp4"),
                                                 QStringLiteral(
                                                     ":/images/resources/images/ddc_bk.png"));
    const auto duration(splash->duration());
    // qDebug() << splash->duration() << "ms";
    QTimer::singleShot(duration, &a, [&splash]() { splash->close(); });

    // Dynamic text test
    for (int i(1); i < 10; ++i) {
        QTimer::singleShot(i * duration / 10, &a, [&splash, i, duration]() {
            splash->setText(QObject::tr("Loading module #%1...").arg(i));
        });
    }

    splash->show();
    ctrlDlg->show();

    return a.exec();
}

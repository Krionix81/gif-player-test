#include <QApplication>
#include <QDialog>
#include <QLocale>
#include <QPushButton>
#include <QTimer>
#include <QTranslator>
#include <QVBoxLayout>
#include "splash_widget.h"

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

    auto ctrlDlg = std::make_unique<QDialog>(nullptr);
    auto lt = new QVBoxLayout(ctrlDlg.get());

    const std::array<QString, 8> appNames{"ddc", "emu", "gla", "mtr", "sc", "tms", "utl", "wdp"};
    for (auto i = 0; i < appNames.size(); ++i) {
        auto btn = new QPushButton(appNames[i], ctrlDlg.get());
        btn->setMinimumWidth(200);
        lt->addWidget(btn);
        QObject::connect(btn, &QPushButton::clicked, &a, [i, &appNames, &a](bool checked) {
            auto splash = new SplashWidget(
                QStringLiteral("qrc:/movies/resources/movies/%1-mov.mp4").arg(appNames[i]),
                QStringLiteral(":/images/resources/images/%1-bk-layer.png").arg(appNames[i]));

            const auto duration(/*splash->duration()*/ 3333);
            // qDebug() << splash->duration() << "ms";
            QTimer::singleShot(duration, &a, [splash]() {
                splash->close();
                splash->deleteLater();
            });

            // Dynamic text test
            for (int i(1); i < 10; ++i) {
                QTimer::singleShot(i * duration / 10, &a, [splash, i]() {
                    splash->setText(QObject::tr("Loading module #%1...").arg(i));
                });
            }
            splash->show();
        });
    }

    ctrlDlg->show();

    return a.exec();
}

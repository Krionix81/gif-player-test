#pragma once
#include <QVersionNumber>
#include <QWidget>
/**
 * \brief The SplashWidget class
 * Welcome form for displaying a welcome message or loading progress when launching an app
 * Allows to play media samples 
 */

class SplashWidget : public QWidget
{
    Q_PROPERTY(QVersionNumber version READ version CONSTANT)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)

    Q_OBJECT

public:
    /**
     * \brief SplashWidget constructs widget with custom background and media
     * \details The easiest way to use:
     * \code
     * auto splash = std::make_unique<SplashWidget>(mediaPath, bkPath);
     * const auto duration(splash->duration());
     * QTimer::singleShot(duration, &a, [splash]() {
     *      splash->close();
     *      splash->deleteLater();
     * });
     * splash->show();
     * \endcode
     * \param[in]  mediaPath media file path (for resources you should use qrc prefix!)
     * \param[in]  bkPath background image file path 
     */
    explicit SplashWidget(const QString &mediaPath, const QString &bkPath);
    ~SplashWidget();

    QVersionNumber version() const { return _version; }

    void setText(const QString &);
    QString text() const;

    /**
     * \brief duration
     * \return media duration if file is valid and format is supported, 0 otherwise
     */
    qint64 duration() const;

signals:
    void textChanged(const QString &);
    void durationChanged(qint64);

private:
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;

    class ContentWidget : public QWidget
    {
    public:
        ContentWidget(const QString &, const std::pair<qreal, qreal> &, QWidget *);

        QPixmap _bkPix;
        class QLabel *_textLbl;
        QScopedPointer<class QMediaPlayer> _player;
        int _fntId{-1};
    };
    QScopedPointer<class ContentWidget> _contentWdg{nullptr};
    QVersionNumber _version{1, 0, 0};
    QPoint _mousePos; // Auxiliary mouse position for moving frameless and titleless widget
};

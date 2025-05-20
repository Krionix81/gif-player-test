#pragma once
#include <QVersionNumber>
#include <QWidget>
/**
 * @brief The SplashWidget class
 */

class SplashWidget : public QWidget
{
    Q_PROPERTY(QVersionNumber version READ version CONSTANT)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)

    Q_OBJECT

public:
    explicit SplashWidget(const QString &, const QString &);
    ~SplashWidget();

    QVersionNumber version() const { return _version; }

    void setText(const QString &);
    QString text() const;

    qint64 duration() const;

signals:
    void textChanged(const QString &);
    void durationChanged(qint64);

private:
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;

    // Data
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

#pragma once
#include <QVersionNumber>
#include <QWidget>

class SplashWidget : public QWidget
{
    Q_PROPERTY(QVersionNumber version READ version CONSTANT)
    Q_PROPERTY(bool text READ text WRITE setText /*NOTIFY textChanged*/)
    Q_PROPERTY(bool duration READ duration /*NOTIFY durationChanged*/)

public:
    explicit SplashWidget(const QString &, const QString &);
    ~SplashWidget();

    QVersionNumber version() const { return _version; }

    void setText(const QString &);
    QString text() const;

    qint64 duration() const;

private:
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;

    // Data
    class ContentWidget : public QWidget
    {
    public:
        ContentWidget(const QString &, const std::pair<qreal, qreal> &);

        QPixmap _bkPix;
        class QLabel *_textLbl;
        QScopedPointer<class QMediaPlayer> _player;
        int _fntId{-1};
    };
    QScopedPointer<class ContentWidget> _contentWdg{nullptr};
    QVersionNumber _version{1, 0, 0};
    QPoint _mousePos; // Auxiliary mouse position for moving frameless and titleless widget
};

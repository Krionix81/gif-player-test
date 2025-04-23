#pragma once
#include <QWidget>

class SplashWidget : public QWidget
{
    Q_PROPERTY(bool text READ text WRITE setText)

public:
    explicit SplashWidget(const QString &, const QString &, QWidget *parent = nullptr);
    ~SplashWidget();

    void setText() {};
    QString text() const;

private:
    void mousePressEvent(QMouseEvent *event) final;
    void mouseMoveEvent(QMouseEvent *event) final;

    // Data
    class ContentWidget : public QWidget
    {
    public:
        ContentWidget(const QString &, int, QWidget *parent = nullptr);

        QPixmap _bkPix;
        class QLabel *_textLbl;
        QScopedPointer<class QMediaPlayer> _player;
    };
    QScopedPointer<class ContentWidget> _contentWdg;
    QPoint _mousePos; // Auxiliary mouse position for moving frameless and titleless widget
};

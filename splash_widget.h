#pragma once
#include <QWidget>

class SplashWidget : public QWidget
{
    Q_OBJECT

public:
    SplashWidget(const QString &, const QString &, QWidget *parent = nullptr);
    ~SplashWidget();

private:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    class QMovie *m_movie;
    class QLabel *m_textField;

    QPoint m_mousePos;
};

#pragma once
#include <QMovie>
#include <QWidget>

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    class QMovie *m_movie;
};
#endif // WIDGET_H

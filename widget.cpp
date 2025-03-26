#include "widget.h"
#include <QBoxLayout>
#include <QLabel>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    auto grLt = new QGridLayout(this);
    grLt->setContentsMargins(0, 0, 0, 0);
    grLt->setSpacing(0);

    int row(0), col(0);

    // Movie container label
    auto lbl = new QLabel(this);
    grLt->addWidget(lbl, row++, col, 1, -1);
    lbl->setMovie(m_movie = new QMovie(this));
}

Widget::~Widget() {}

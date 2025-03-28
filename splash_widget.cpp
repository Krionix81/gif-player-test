#include "splash_widget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMovie>
//=================================================================================================

SplashWidget::SplashWidget(const QString &moviePath,
                           const QString &bkPath,
                           QWidget *parent /* = nullptr*/)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    setStyleSheet("border: 3px solid red;");

    auto lt = new QVBoxLayout(this);
    lt->setContentsMargins(0, 0, 0, 0);
    lt->setSpacing(0);

    // Background layer widget
    auto bkWdg = new QWidget(this);
    bkWdg->setAutoFillBackground(true);
    lt->addWidget(bkWdg);

    auto pal(bkWdg->palette());
    auto bkPix = QPixmap(bkPath);
    pal.setBrush(QPalette::Window, bkPix.scaled({640, 550}, Qt::KeepAspectRatio));
    bkWdg->setPalette(pal);

    auto grLt = new QGridLayout(bkWdg);
    grLt->setContentsMargins(0, 0, 0, 0);
    grLt->setSpacing(0);
    bkWdg->setLayout(grLt);

    int row(0), col(0);

    // Movie container label
    auto lbl = new QLabel(bkWdg);
    grLt->addWidget(lbl, row++, col, 1, -1);
    lbl->setAttribute(Qt::WA_TranslucentBackground);
    lbl->setMovie(m_movie = new QMovie(moviePath, "gif", bkWdg));
    if (!m_movie->isValid())
        qWarning() << m_movie->lastErrorString();
    else
        m_movie->start();

    lbl = new QLabel(bkWdg);
    lbl->setAlignment(Qt::AlignCenter);
    grLt->addWidget(lbl, row, col++);

    grLt->addWidget(m_textField
                    = new QLabel(tr("jedjejd\ndldldldldjejdjejdej\niieidiedieidie\njejejeje"),
                                 bkWdg),
                    row,
                    col);
}

SplashWidget::~SplashWidget() {}
//=================================================================================================

void SplashWidget::mousePressEvent(QMouseEvent *event)
{
    m_mousePos = event->pos();
}
//=================================================================================================

void SplashWidget::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPosition().toPoint() - m_mousePos);
}
//=================================================================================================

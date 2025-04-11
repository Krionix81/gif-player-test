#include "video_splash.h"
#include <QBoxLayout>
#include <QLabel>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QVideoWidget>

namespace {
constexpr int MOVIE_TOP_OFFSET(28);
}
//=================================================================================================

SplashWidget::ContentWidget::ContentWidget(const QString &moviePath,
                                           int movieTopOffset,
                                           QWidget *parent /* = nullptr*/)
    : QWidget(parent)
    , _player(new QMediaPlayer(this))
{
    setAutoFillBackground(true);

    auto lt = new QGridLayout(this);
    lt->setContentsMargins(0, 0, 0, 0);
    lt->setSpacing(0);

    int row(0), col(0);

    // Just spacer to offset movie down
    auto spacer = new QWidget(this);
    spacer->setAttribute(Qt::WA_TranslucentBackground);
    spacer->setSizePolicy(spacer->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    spacer->setFixedHeight(movieTopOffset);
    lt->addWidget(spacer, row++, col, 1, -1);

    // Movie player
    auto videoWdg = new QVideoWidget(this);
    lt->addWidget(videoWdg, row++, col, 1, -1);

    _player->setVideoOutput(videoWdg);
    connect(_player.data(),
            &QMediaPlayer::errorOccurred,
            this,
            [=](QMediaPlayer::Error error, const QString &errorString) { qDebug() << errorString; });

    _player->setSource(QUrl(moviePath));
    const auto md = _player->metaData().value(QMediaMetaData::Resolution);
    Q_ASSERT_X(QMediaPlayer::NoError == _player->error() && !md.isNull(),
               "Invalid video source!",
               Q_FUNC_INFO);
    const auto videoRes(md.toSize());
    videoWdg->setFixedSize(videoRes);
    _player->setLoops(QMediaPlayer::Infinite);
    _player->play();

    auto logoLbl = new QLabel(this);
    logoLbl->setFixedHeight(182);
    logoLbl->setAttribute(Qt::WA_TranslucentBackground);
    lt->addWidget(logoLbl, row, col++);

    _textLbl = new QLabel("jfggggggggggggggggggggjfjgjfj\nrfrjfrjfjrjfjrjfrj\njrjrjrjjrjrjrjjjrjrj",
                          this);
    //textLbl->setAttribute(Qt::WA_TranslucentBackground);
    lt->addWidget(_textLbl, row, col);
    _textLbl->setStyleSheet("QLabel{border: 3px solid red;}");

    lt->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, col);
}
//=================================================================================================

SplashWidget::SplashWidget(const QString &moviePath,
                           const QString &bkPath,
                           QWidget *parent /* = nullptr*/)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
    , _contentWdg(new ContentWidget(moviePath, MOVIE_TOP_OFFSET, this))
{
    auto lt = new QVBoxLayout(this);
    lt->setContentsMargins(0, 0, 0, 0);
    lt->setSpacing(0);

    // Get backgroung image dimensions
    const auto bkPix = QPixmap(bkPath);
    Q_ASSERT_X(!bkPix.isNull(), "Invalid background image!", Q_FUNC_INFO);

    _contentWdg->setFixedSize(bkPix.size());
    auto pal(_contentWdg->palette());
    pal.setBrush(QPalette::Window, QBrush{bkPix});
    _contentWdg->setPalette(pal);

    lt->addWidget(_contentWdg.data());

    setAutoFillBackground(false);
    setMask(bkPix.createHeuristicMask());
}

SplashWidget::~SplashWidget() {}
//=================================================================================================

QString SplashWidget::text() const
{
    return _contentWdg->_textLbl->text();
}
//=================================================================================================

void SplashWidget::mousePressEvent(QMouseEvent *event)
{
    _mousePos = event->pos();
}
//=================================================================================================

void SplashWidget::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPosition().toPoint() - _mousePos);
}
//=================================================================================================

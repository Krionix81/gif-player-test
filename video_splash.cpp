#include "video_splash.h"
#include <QBoxLayout>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLabel>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QPainter>
#include <QStyleOption>
#include <QVideoWidget>

namespace {
constexpr int SCR_WIDTH_PART(4);
constexpr int MOVIE_TOP_OFFSET(28);
} // namespace
//=================================================================================================

SplashWidget::ContentWidget::ContentWidget(const QString &moviePath,
                                           qreal sizeRatio,
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
    spacer->setFixedHeight(MOVIE_TOP_OFFSET * sizeRatio);
    lt->addWidget(spacer, row++, col, 1, -1);

    // Movie player
    auto videoWdg = new QVideoWidget(this);
    videoWdg->setAspectRatioMode(Qt::IgnoreAspectRatio /*KeepAspectRatio*/);
    lt->addWidget(videoWdg, row++, col, 1, -1);

    _player->setVideoOutput(videoWdg);
    connect(_player.data(),
            &QMediaPlayer::mediaStatusChanged,
            this,
            [this, videoWdg, moviePath, sizeRatio](QMediaPlayer::MediaStatus status) {
                if (QMediaPlayer::LoadedMedia == status) {
                    const auto md = _player->metaData().value(QMediaMetaData::Resolution);
                    Q_ASSERT_X(QMediaPlayer::NoError == _player->error() && !md.isNull(),
                               "Invalid video source!",
                               Q_FUNC_INFO);
                    videoWdg->setFixedHeight(int(sizeRatio * md.toSize().height()));
                    _player->play();
                }
            });
    connect(_player.data(),
            &QMediaPlayer::errorOccurred,
            this,
            [=](QMediaPlayer::Error error, const QString &errorString) { qDebug() << errorString; });

    _player->setLoops(QMediaPlayer::Infinite);
    _player->setSource(QUrl(moviePath));

    auto logoLbl = new QLabel(this);
    logoLbl->setAttribute(Qt::WA_TranslucentBackground);
    lt->addWidget(logoLbl, row, col++);

    _textLbl = new QLabel("*******************************GERS Group********************", this);
    _textLbl->setAttribute(Qt::WA_TranslucentBackground);
    lt->addWidget(_textLbl, row, col);

    // Way to setup font
    if (_fntId = QFontDatabase::addApplicationFont(
            QStringLiteral(":/font/resources/font/inter-medium.ttf"));
        0 <= _fntId)
        _textLbl->setFont(QFont{QStringLiteral("Inter Medium"), 10, QFont::Normal});
    else
        qWarning("Failed to load splash font!");

    lt->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding), row, col);
}
//=================================================================================================

SplashWidget::SplashWidget(const QString &moviePath,
                           const QString &bkPath,
                           QWidget *parent /* = nullptr*/)
    : QWidget(parent, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setAttribute(Qt::WA_NoSystemBackground);

    // Get backgroung image
    const auto tmpPix = QPixmap(bkPath);
    Q_ASSERT_X(!tmpPix.isNull(), "Invalid background image!", Q_FUNC_INFO);

    // Detect complex screen scale factor(dpr + resolution)
    const auto screen = QGuiApplication::primaryScreen();
    qreal scaleFactor(1.);
    if (nullptr != screen) {
        scaleFactor = 1. / screen->devicePixelRatio();
        const auto resolution(screen->size());
        Q_ASSERT_X(resolution.width() >= tmpPix.size().width()
                       && resolution.height() >= tmpPix.size().height(),
                   "Display resolution fuckup!",
                   Q_FUNC_INFO);
        if (tmpPix.width() >= resolution.width() / SCR_WIDTH_PART)
            scaleFactor *= resolution.width() / (qreal(tmpPix.width()) * SCR_WIDTH_PART);
    }

    _contentWdg.reset(new ContentWidget(moviePath, scaleFactor, this));

    // Setup backgroung dimensions
    _contentWdg->_bkPix = tmpPix.scaled(tmpPix.size() * scaleFactor,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);

    // Resize container widget according background image size
    _contentWdg->setFixedSize(_contentWdg->_bkPix.size());

    // Fill background
    auto pal(_contentWdg->palette());
    pal.setBrush(QPalette::Window, QBrush{_contentWdg->_bkPix});
    _contentWdg->setPalette(pal);

    // Make bottom-level widget transparent
    setMask(_contentWdg->_bkPix.createHeuristicMask());

    auto lt = new QVBoxLayout(this);
    lt->setContentsMargins(0, 0, 0, 0);
    lt->setSpacing(0);
    lt->addWidget(_contentWdg.data());
}
//=================================================================================================

SplashWidget::~SplashWidget()
{
    if (0 <= _contentWdg->_fntId)
        QFontDatabase::removeApplicationFont(_contentWdg->_fntId);
}
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

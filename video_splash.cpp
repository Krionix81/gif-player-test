#include "video_splash.h"
#include <QBoxLayout>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLabel>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QPainter>
//#include <QStyleOption>
#include <QDate>
#include <QVideoWidget>

namespace {
constexpr int SCR_WIDTH_PART{4};
constexpr int MOVIE_TOP_OFFSET{28};
constexpr int VIDEO_HEIGHT{660};
} // namespace
//=================================================================================================

SplashWidget::ContentWidget::ContentWidget(const QString &mediaPath,
                                           const std::pair<qreal, qreal> &scaleFactors)
    : _player(new QMediaPlayer(this))
{
    setAutoFillBackground(true);

    const auto complexSF(scaleFactors.first * scaleFactors.second);

    auto lt = new QGridLayout(this);
    lt->setContentsMargins(0, 0, 0, 0);
    lt->setSpacing(0);

    int row(0), col(0);

    // Just spacer to offset media down
    auto spacer = new QWidget(this);
    spacer->setAttribute(Qt::WA_TranslucentBackground);
    spacer->setSizePolicy(spacer->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    spacer->setFixedHeight(MOVIE_TOP_OFFSET * complexSF);
    lt->addWidget(spacer, row++, col, 1, -1);

    // Media player
    auto videoWdg = new QVideoWidget(this);
    videoWdg->setAspectRatioMode(Qt::KeepAspectRatio);
    videoWdg->setFixedHeight(static_cast<int>(complexSF * VIDEO_HEIGHT));
    lt->addWidget(videoWdg, row++, col, 1, -1);

    _player->setVideoOutput(videoWdg);
    connect(_player.data(),
            &QMediaPlayer::mediaStatusChanged,
            this,
            [this /*, videoWdg, sizeRatio*/](QMediaPlayer::MediaStatus status) {
                if (QMediaPlayer::LoadedMedia == status) {
                    /* QMediaPlayer doesn't always return the correct media resolution from metadata
                    const auto md = _player->metaData().value(QMediaMetaData::Resolution);
                    Q_ASSERT_X(QMediaPlayer::NoError == _player->error() && !md.isNull(),
                               "Invalid video source!",
                               Q_FUNC_INFO);
                    videoWdg->setFixedHeight(int(sizeRatio * md.toSize().height()));
                    */
                    _player->play();
                }
            });
    connect(_player.data(),
            &QMediaPlayer::errorOccurred,
            this,
            [=](QMediaPlayer::Error error, const QString &errorString) {
                qCritical() << errorString;
            });

    _player->setLoops(QMediaPlayer::Infinite);
    _player->setSource(QUrl(mediaPath));

    // Company logo
    auto logoLbl = new QLabel(this);
    logoLbl->setAttribute(Qt::WA_TranslucentBackground);
    logoLbl->setAlignment(Qt::AlignCenter);
    logoLbl->setFrameStyle(QFrame::Box | QFrame::Plain);
    lt->addWidget(logoLbl, row, col++, -1, 1);

    auto logoPx{QPixmap(QStringLiteral(":/images/resources/images/gers_logo.svg"))};
    Q_ASSERT_X(!logoPx.isNull(), "Invalid logo image!", Q_FUNC_INFO);
    logoPx = logoPx.scaledToHeight(static_cast<int>(complexSF * logoPx.height()),
                                   Qt::SmoothTransformation);
    logoLbl->setPixmap(logoPx);
    logoLbl->setFixedWidth(static_cast<int>(0.4 * logoPx.width() + logoPx.width()));

    // Static text information part aka copyright
    auto staticTextLbl = new QLabel(tr("© «GERS Group» Company 2010-%2<br />"
                                       "All rights reserved. Any unauthorized use<br />"
                                       "of this software is prohibited by law.")
                                        .arg(QDate::currentDate().year()),
                                    this);
    staticTextLbl->setAttribute(Qt::WA_TranslucentBackground);
    staticTextLbl->setAlignment(Qt::AlignTop);
    staticTextLbl->setFrameStyle(QFrame::Box | QFrame::Plain);
    staticTextLbl->setIndent(12);
    lt->addWidget(staticTextLbl, row++, col);

    // Dynamic text to show loading information progress
    _textLbl = new QLabel(tr("Loading..."), this);
    _textLbl->setAttribute(Qt::WA_TranslucentBackground);
    _textLbl->setAlignment(Qt::AlignBottom);
    _textLbl->setFrameStyle(QFrame::Box | QFrame::Plain);
    _textLbl->setIndent(4);
    lt->addWidget(_textLbl, row, col);

    // Font setup
    if (_fntId = QFontDatabase::addApplicationFont(
            QStringLiteral(":/font/resources/font/inter-medium.ttf"));
        0 <= _fntId) {
        auto fnt{QFont(QStringLiteral("Inter Medium"))};
        fnt.setPixelSize(static_cast<int>(std::floor(scaleFactors.first * 12)));
        staticTextLbl->setFont(fnt);
        fnt.setPixelSize(static_cast<int>(std::floor(scaleFactors.first * 10)));
        _textLbl->setFont(fnt);
    } else {
        qWarning("Failed to load splash font!");
    }
}
//=================================================================================================

SplashWidget::SplashWidget(const QString &moviePath, const QString &bkPath)
    : QWidget(nullptr, Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint)
{
    setAttribute(Qt::WA_NativeWindow);
    setAttribute(Qt::WA_DontCreateNativeAncestors);
    setAttribute(Qt::WA_NoSystemBackground);

    // Get backgroung image
    const auto tmpPix{QPixmap(bkPath)};
    Q_ASSERT_X(!tmpPix.isNull(), "Invalid background image!", Q_FUNC_INFO);

    // Detect complex screen scale factor(dpr + resolution)
    const auto screen = (nullptr != QGuiApplication::screenAt(QCursor::pos()))
                            ? QGuiApplication::screenAt(QCursor::pos())
                            : QGuiApplication::primaryScreen();
    std::pair<qreal, qreal> scaleFactors{1., 1.};
    if (nullptr != screen) {
        scaleFactors.first = 1. / screen->devicePixelRatio();
        const auto resolution(screen->size() * screen->devicePixelRatio());
        Q_ASSERT_X(resolution.width() >= tmpPix.size().width()
                       && resolution.height() >= tmpPix.size().height(),
                   "Display resolution fuckup!",
                   Q_FUNC_INFO);
        if (tmpPix.width() >= resolution.width() / SCR_WIDTH_PART)
            scaleFactors.second = resolution.width() / (qreal(tmpPix.width()) * SCR_WIDTH_PART);
    }

    _contentWdg.reset(new ContentWidget(moviePath, scaleFactors));

    // Setup backgroung dimensions
    _contentWdg->_bkPix = tmpPix.scaled(tmpPix.size() * scaleFactors.first * scaleFactors.second,
                                        Qt::KeepAspectRatio,
                                        Qt::SmoothTransformation);

    // Resize container widget according background image size
    const auto bkSize(_contentWdg->_bkPix.size());
    _contentWdg->setFixedSize(bkSize);

    // Fill background
    auto pal(_contentWdg->palette());
    pal.setBrush(QPalette::Window, QBrush{_contentWdg->_bkPix});
    _contentWdg->setPalette(pal);

    // The way to make bottom-level widget transparent(i.e. for complex shape background)
    setMask(_contentWdg->_bkPix.mask());

    auto lt = new QVBoxLayout(this);
    lt->setContentsMargins(0, 0, 0, 0);
    lt->setSpacing(0);
    lt->addWidget(_contentWdg.data());

    // Сenter the window on screen taking into account the scaling done
    move(screen->availableGeometry().center() - QPoint{bkSize.width() / 2, bkSize.height() / 2});
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

void SplashWidget::setText(const QString &text)
{
    return _contentWdg->_textLbl->setText(text);
}
//=================================================================================================

qint64 SplashWidget::duration() const
{
    return _contentWdg->_player->duration();
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

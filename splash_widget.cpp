#include "splash_widget.h"
#include <QBoxLayout>
#include <QDate>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QLabel>
#include <QMediaMetaData>
#include <QMediaPlayer>
#include <QMouseEvent>
#include <QPainter>
#include <QVideoWidget>

namespace {
constexpr int SCR_WIDTH_PART{3};
constexpr int MOVIE_TOP_OFFSET{28};
constexpr int VIDEO_HEIGHT{660};
constexpr int FNT_SZ{18};
} // namespace

SplashWidget::ContentWidget::ContentWidget(const QString &mediaPath,
                                           const std::pair<qreal, qreal> &scaleFactors,
                                           QWidget *parent)
    : _player(new QMediaPlayer(this))
    , QWidget(parent)
{
    setAutoFillBackground(true);

    const auto complexSF(scaleFactors.first * scaleFactors.second);

    auto mainLt = new QGridLayout(this);
    mainLt->setContentsMargins(0, 0, 0, 0);
    mainLt->setSpacing(0);

    int row(0), col(0);

    // Just spacer to offset media down
    auto spacer = new QWidget(this);
    spacer->setAttribute(Qt::WA_TranslucentBackground);
    spacer->setSizePolicy(spacer->sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
    spacer->setFixedHeight(MOVIE_TOP_OFFSET * complexSF);
    mainLt->addWidget(spacer, row++, col, 1, -1);

    // Media player
    auto videoWdg = new QVideoWidget(this);
    videoWdg->setAspectRatioMode(Qt::IgnoreAspectRatio /*KeepAspectRatio*/);
    videoWdg->setFixedHeight(static_cast<int>(complexSF * VIDEO_HEIGHT));
    mainLt->addWidget(videoWdg, row++, col, 1, -1);

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
    auto logoPx{QPixmap(QStringLiteral(":/resources/gers_logo.svg"))};
    Q_ASSERT_X(!logoPx.isNull(), "Invalid logo image!", Q_FUNC_INFO);
    logoPx = logoPx.scaledToHeight(static_cast<int>(complexSF * logoPx.height()),
                                   Qt::SmoothTransformation);
    logoLbl->setPixmap(logoPx);
    logoLbl->setFixedWidth(static_cast<int>(logoPx.width()));

    // Intermediate layout used to set image margins according design
    auto lt = new QVBoxLayout();
    lt->setSpacing(0);
    lt->setContentsMargins(QMargins{68, 11, 40, 35} * complexSF);
    mainLt->addLayout(lt, row, col++, -1, 1);
    lt->addWidget(logoLbl);

    // Static text information part aka copyright
    auto staticTextLbl = new QLabel(tr("© «GERS Group» Company 2010-%2<br />"
                                       "All rights reserved. Any unauthorized use<br />"
                                       "of this software is prohibited by law.")
                                        .arg(QDate::currentDate().year()),
                                    this);
    staticTextLbl->setAttribute(Qt::WA_TranslucentBackground);
    staticTextLbl->setAlignment(Qt::AlignTop);

    lt = new QVBoxLayout();
    lt->setSpacing(0);
    lt->setContentsMargins(QMargins{0, 11, 0, 0} * complexSF);
    lt->addWidget(staticTextLbl);
    mainLt->addLayout(lt, row++, col, 1, -1);

    // Dynamic text to show loading information progress
    _textLbl = new QLabel(tr("Loading..."), this);
    _textLbl->setAttribute(Qt::WA_TranslucentBackground);
    _textLbl->setAlignment(Qt::AlignBottom);

    lt = new QVBoxLayout();
    lt->setSpacing(0);
    lt->setContentsMargins(QMargins{0, 0, 0, 35} * complexSF);
    mainLt->addLayout(lt, row, col, 1, -1);
    lt->addWidget(_textLbl);

    // Font setup
    if (_fntId = QFontDatabase::addApplicationFont(
            QStringLiteral(":/font/resources/font/inter-medium.ttf"));
        0 <= _fntId) {
        auto fnt{QFont(QStringLiteral("Inter Medium"))};
        fnt.setPixelSize(static_cast<int>(std::floor(scaleFactors.first * FNT_SZ)));
        staticTextLbl->setFont(fnt);
        fnt.setPixelSize(static_cast<int>(std::floor(scaleFactors.first * FNT_SZ)));
        _textLbl->setFont(fnt);
    } else {
        qWarning("Failed to load splash font!");
    }
}

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

    _contentWdg.reset(new ContentWidget(moviePath, scaleFactors, this));

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

    auto mainLt = new QVBoxLayout(this);
    mainLt->setContentsMargins(0, 0, 0, 0);
    mainLt->setSpacing(0);
    mainLt->addWidget(_contentWdg.data());

    // Сenter the window on screen taking into account the scaling done
    move(screen->availableGeometry().center() - QPoint{bkSize.width() / 2, bkSize.height() / 2});
}

SplashWidget::~SplashWidget()
{
    if (0 <= _contentWdg->_fntId)
        QFontDatabase::removeApplicationFont(_contentWdg->_fntId);
}

QString SplashWidget::text() const
{
    return _contentWdg->_textLbl->text();
}

void SplashWidget::setText(const QString &text)
{
    _contentWdg->_textLbl->setText(text);
    emit textChanged(text);
    QCoreApplication::processEvents(QEventLoop::ExcludeUserInputEvents);
}

qint64 SplashWidget::duration() const
{
    return _contentWdg->_player->duration();
}

void SplashWidget::mousePressEvent(QMouseEvent *event)
{
    _mousePos = event->pos();
}

void SplashWidget::mouseMoveEvent(QMouseEvent *event)
{
    move(event->globalPosition().toPoint() - _mousePos);
}

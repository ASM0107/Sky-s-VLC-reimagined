#include "QtMediaBackend.h"

QtMediaBackend::QtMediaBackend(QObject *parent) : IMediaBackend(parent) {
    m_player = new QMediaPlayer(this);
    m_audioOutput = new QAudioOutput(this);
    m_player->setAudioOutput(m_audioOutput);

    connect(m_player, &QMediaPlayer::positionChanged, this, &IMediaBackend::positionChanged);
    connect(m_player, &QMediaPlayer::durationChanged, this, &IMediaBackend::durationChanged);
    connect(m_player, &QMediaPlayer::playbackStateChanged, this, &QtMediaBackend::onPlayerStateChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &QtMediaBackend::onMediaStatusChanged);
    connect(m_player, &QMediaPlayer::errorOccurred, this, &QtMediaBackend::onErrorOccurred);
}

QtMediaBackend::~QtMediaBackend() = default;

void QtMediaBackend::setSource(const QUrl& url) {
    m_currentUrl = url;
    m_player->setSource(url);
}

void QtMediaBackend::play() { m_player->play(); }
void QtMediaBackend::pause() { m_player->pause(); }
void QtMediaBackend::stop() { m_player->stop(); }
void QtMediaBackend::seek(qint64 position) { m_player->setPosition(position); }
void QtMediaBackend::setVolume(float volume) { m_audioOutput->setVolume(volume); }
void QtMediaBackend::setMuted(bool muted) { m_audioOutput->setMuted(muted); }
void QtMediaBackend::setPlaybackRate(qreal rate) { m_player->setPlaybackRate(rate); }

void QtMediaBackend::setVideoSink(QVideoSink* sink) {
    m_player->setVideoSink(sink);
}

QString QtMediaBackend::currentMedia() const {
    return m_currentUrl.toString();
}

qint64 QtMediaBackend::position() const { return m_player->position(); }
qint64 QtMediaBackend::duration() const { return m_player->duration(); }

IMediaBackend::PlaybackState QtMediaBackend::state() const {
    switch (m_player->playbackState()) {
        case QMediaPlayer::PlayingState: return PlaybackState::Playing;
        case QMediaPlayer::PausedState: return PlaybackState::Paused;
        default: return PlaybackState::Stopped;
    }
}

void QtMediaBackend::onPlayerStateChanged(QMediaPlayer::PlaybackState state) {
    emit stateChanged(this->state());
}

void QtMediaBackend::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    if (status == QMediaPlayer::EndOfMedia) {
        emit mediaEnded();
    }
}

void QtMediaBackend::onErrorOccurred(QMediaPlayer::Error error, const QString &errorString) {
    emit errorOccurred(QString("QtMediaBackend Error: %1").arg(errorString));
}

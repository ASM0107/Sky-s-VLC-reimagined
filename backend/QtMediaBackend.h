#ifndef QTMEDIABACKEND_H
#define QTMEDIABACKEND_H

#include "IMediaBackend.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QVideoSink>

class QtMediaBackend : public IMediaBackend {
    Q_OBJECT
public:
    explicit QtMediaBackend(QObject *parent = nullptr);
    ~QtMediaBackend() override;

    void setSource(const QUrl& url) override;
    void play() override;
    void pause() override;
    void stop() override;
    void seek(qint64 position) override;
    void setVolume(float volume) override;
    void setMuted(bool muted) override;
    void setPlaybackRate(qreal rate) override;
    void setVideoSink(QVideoSink* sink) override;

    QString currentMedia() const override;
    qint64 position() const override;
    qint64 duration() const override;
    PlaybackState state() const override;

private slots:
    void onPlayerStateChanged(QMediaPlayer::PlaybackState state);
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onErrorOccurred(QMediaPlayer::Error error, const QString &errorString);

private:
    QMediaPlayer* m_player;
    QAudioOutput* m_audioOutput;
    QUrl m_currentUrl;
};

#endif // QTMEDIABACKEND_H

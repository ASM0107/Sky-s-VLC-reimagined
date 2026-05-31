#ifndef FFMPEGBACKEND_H
#define FFMPEGBACKEND_H

#include "IMediaBackend.h"

// Stub for native FFmpeg Backend integration (libavformat, libavcodec, libavutil)
// Note: Implementing a full custom FFmpeg player loop in Qt requires extensive code
// (threading, A/V sync, hardware decoding). This acts as a placeholder architecture.

class FFmpegBackend : public IMediaBackend {
    Q_OBJECT
public:
    explicit FFmpegBackend(QObject *parent = nullptr) : IMediaBackend(parent) {}
    
    void setSource(const QUrl& url) override { m_url = url; }
    void play() override { /* Initialize decoder thread and start */ emit stateChanged(PlaybackState::Playing); }
    void pause() override { emit stateChanged(PlaybackState::Paused); }
    void stop() override { emit stateChanged(PlaybackState::Stopped); }
    void seek(qint64 /*position*/) override { /* av_seek_frame */ }
    void setVolume(float /*volume*/) override { /* Scale PCM samples */ }
    void setMuted(bool /*muted*/) override {}
    void setPlaybackRate(qreal /*rate*/) override {}
    void setVideoSink(QVideoSink* /*sink*/) override { /* Convert AVFrame to QVideoFrame and push to sink */ }

    QString currentMedia() const override { return m_url.toString(); }
    qint64 position() const override { return 0; }
    qint64 duration() const override { return 0; }
    PlaybackState state() const override { return PlaybackState::Stopped; }

private:
    QUrl m_url;
};

#endif // FFMPEGBACKEND_H

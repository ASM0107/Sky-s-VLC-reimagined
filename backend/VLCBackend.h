#ifndef VLCBACKEND_H
#define VLCBACKEND_H

#include "IMediaBackend.h"

// Stub for native libVLC Backend integration.
// Requires linking against libvlc and libvlccore.

class VLCBackend : public IMediaBackend {
    Q_OBJECT
public:
    explicit VLCBackend(QObject *parent = nullptr) : IMediaBackend(parent) {}
    
    void setSource(const QUrl& url) override { m_url = url; }
    void play() override { emit stateChanged(PlaybackState::Playing); }
    void pause() override { emit stateChanged(PlaybackState::Paused); }
    void stop() override { emit stateChanged(PlaybackState::Stopped); }
    void seek(qint64 /*position*/) override {}
    void setVolume(float /*volume*/) override {}
    void setMuted(bool /*muted*/) override {}
    void setPlaybackRate(qreal /*rate*/) override {}
    void setVideoSink(QVideoSink* /*sink*/) override { /* Use libvlc_video_set_callbacks to push pixels */ }

    QString currentMedia() const override { return m_url.toString(); }
    qint64 position() const override { return 0; }
    qint64 duration() const override { return 0; }
    PlaybackState state() const override { return PlaybackState::Stopped; }

private:
    QUrl m_url;
};

#endif // VLCBACKEND_H

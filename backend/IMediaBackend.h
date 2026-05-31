#ifndef IMEDIABACKEND_H
#define IMEDIABACKEND_H

#include <QObject>
#include <QString>
#include <QUrl>

class QVideoSink;

class IMediaBackend : public QObject {
    Q_OBJECT
public:
    explicit IMediaBackend(QObject *parent = nullptr) : QObject(parent) {}
    virtual ~IMediaBackend() = default;

    virtual void setSource(const QUrl& url) = 0;
    virtual void play() = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
    virtual void seek(qint64 position) = 0;
    virtual void setVolume(float volume) = 0;
    virtual void setMuted(bool muted) = 0;
    virtual void setPlaybackRate(qreal rate) = 0;
    
    virtual void setVideoSink(QVideoSink* sink) = 0;

    virtual QString currentMedia() const = 0;
    virtual qint64 position() const = 0;
    virtual qint64 duration() const = 0;
    
    enum class PlaybackState { Playing, Paused, Stopped };
    virtual PlaybackState state() const = 0;

signals:
    void positionChanged(qint64 position);
    void durationChanged(qint64 duration);
    void stateChanged(IMediaBackend::PlaybackState state);
    void mediaEnded();
    void errorOccurred(const QString& error);
};

#endif // IMEDIABACKEND_H

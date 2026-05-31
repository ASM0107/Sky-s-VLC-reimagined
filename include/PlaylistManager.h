#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QStringList>
#include <QList>
#include <QUrl>

struct PlaylistItem {
    QString title;
    QUrl url;
};

class PlaylistManager : public QObject {
    Q_OBJECT
public:
    explicit PlaylistManager(QObject *parent = nullptr);

    void addMedia(const QUrl& url);
    void addMediaList(const QList<QUrl>& urls);
    void removeMedia(int index);
    void clear();

    QList<PlaylistItem> getItems() const;
    int currentIndex() const;
    void setCurrentIndex(int index);

    bool saveToJson(const QString& filePath) const;
    bool loadFromJson(const QString& filePath);

signals:
    void playlistChanged();
    void currentIndexChanged(int index);

private:
    QList<PlaylistItem> m_items;
    int m_currentIndex;
};

#endif // PLAYLISTMANAGER_H

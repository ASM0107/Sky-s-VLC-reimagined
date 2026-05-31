#ifndef SUBTITLEENGINE_H
#define SUBTITLEENGINE_H

#include <QObject>
#include <QString>
#include <QList>
#include <QUrl>

struct SubtitleBlock {
    qint64 startTimeMs;
    qint64 endTimeMs;
    QString text;
};

class SubtitleEngine : public QObject {
    Q_OBJECT
public:
    explicit SubtitleEngine(QObject *parent = nullptr);

    bool loadSrt(const QString& filePath);
    void clear();
    
    QString getSubtitleAt(qint64 timeMs) const;

signals:
    void subtitlesLoaded();

private:
    qint64 parseTime(const QString& timeStr) const;
    QList<SubtitleBlock> m_blocks;
};

#endif // SUBTITLEENGINE_H

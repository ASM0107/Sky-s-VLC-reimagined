#ifndef METADATAENGINE_H
#define METADATAENGINE_H

#include <QObject>
#include <QString>
#include <QVariantMap>
#include <QUrl>

class MetadataEngine : public QObject {
    Q_OBJECT
public:
    explicit MetadataEngine(QObject *parent = nullptr);
    
    // Starts asynchronous extraction using QtConcurrent
    void extractMetadataAsync(const QUrl& url);

signals:
    void metadataReady(const QUrl& url, const QVariantMap& metadata);
    void thumbnailReady(const QUrl& url, const QString& thumbnailPath);

private:
    QVariantMap extractSynchronous(const QUrl& url);
};

#endif // METADATAENGINE_H

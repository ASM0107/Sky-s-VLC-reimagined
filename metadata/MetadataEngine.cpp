#include "MetadataEngine.h"
#include <QtConcurrent>
#include <QFutureWatcher>
#include <QFileInfo>
#include <QVariantMap>

MetadataEngine::MetadataEngine(QObject *parent) : QObject(parent) {}

void MetadataEngine::extractMetadataAsync(const QUrl& url) {
    auto watcher = new QFutureWatcher<QVariantMap>(this);
    connect(watcher, &QFutureWatcher<QVariantMap>::finished, this, [this, watcher, url]() {
        QVariantMap metadata = watcher->result();
        emit metadataReady(url, metadata);
        watcher->deleteLater();
    });

    QFuture<QVariantMap> future = QtConcurrent::run([this, url]() {
        return extractSynchronous(url);
    });
    
    watcher->setFuture(future);
}

QVariantMap MetadataEngine::extractSynchronous(const QUrl& url) {
    QVariantMap map;
    // In a real FFmpeg integration, libavformat would be used here to probe the file
    // For this demonstration, we'll extract basic properties from QFileInfo
    if (url.isLocalFile()) {
        QFileInfo info(url.toLocalFile());
        map["title"] = info.completeBaseName();
        map["size"] = info.size();
        map["codec"] = "H.264 / AAC (Mock)";
        map["resolution"] = "1920x1080 (Mock)";
        map["fps"] = "60 (Mock)";
    } else {
        map["title"] = url.toString();
        map["codec"] = "Network Stream (Mock)";
    }
    
    // Simulate complex extraction delay without freezing UI
    QThread::msleep(300);
    
    return map;
}

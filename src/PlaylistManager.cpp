#include "PlaylistManager.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>
#include <QFileInfo>

PlaylistManager::PlaylistManager(QObject *parent) : QObject(parent), m_currentIndex(-1) {}

void PlaylistManager::addMedia(const QUrl& url) {
    PlaylistItem item;
    item.url = url;
    if (url.isLocalFile()) {
        item.title = QFileInfo(url.toLocalFile()).fileName();
    } else {
        item.title = url.toString();
    }
    m_items.append(item);
    
    if (m_currentIndex == -1) {
        m_currentIndex = 0;
        emit currentIndexChanged(m_currentIndex);
    }
    emit playlistChanged();
}

void PlaylistManager::addMediaList(const QList<QUrl>& urls) {
    for (const QUrl& url : urls) {
        addMedia(url);
    }
}

void PlaylistManager::removeMedia(int index) {
    if (index >= 0 && index < m_items.size()) {
        m_items.removeAt(index);
        if (m_currentIndex == index) {
            m_currentIndex = m_items.isEmpty() ? -1 : qMin(index, m_items.size() - 1);
            emit currentIndexChanged(m_currentIndex);
        } else if (m_currentIndex > index) {
            m_currentIndex--;
            emit currentIndexChanged(m_currentIndex);
        }
        emit playlistChanged();
    }
}

void PlaylistManager::clear() {
    m_items.clear();
    m_currentIndex = -1;
    emit currentIndexChanged(m_currentIndex);
    emit playlistChanged();
}

QList<PlaylistItem> PlaylistManager::getItems() const {
    return m_items;
}

int PlaylistManager::currentIndex() const {
    return m_currentIndex;
}

void PlaylistManager::setCurrentIndex(int index) {
    if (index >= 0 && index < m_items.size() && m_currentIndex != index) {
        m_currentIndex = index;
        emit currentIndexChanged(m_currentIndex);
    }
}

bool PlaylistManager::saveToJson(const QString& filePath) const {
    QJsonArray array;
    for (const auto& item : m_items) {
        QJsonObject obj;
        obj["title"] = item.title;
        obj["url"] = item.url.toString();
        array.append(obj);
    }
    
    QJsonDocument doc(array);
    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        return true;
    }
    return false;
}

bool PlaylistManager::loadFromJson(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return false;
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError || !doc.isArray()) return false;
    
    clear();
    QJsonArray array = doc.array();
    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        PlaylistItem item;
        item.title = obj["title"].toString();
        item.url = QUrl(obj["url"].toString());
        m_items.append(item);
    }
    
    if (!m_items.isEmpty()) {
        m_currentIndex = 0;
        emit currentIndexChanged(m_currentIndex);
    }
    emit playlistChanged();
    return true;
}

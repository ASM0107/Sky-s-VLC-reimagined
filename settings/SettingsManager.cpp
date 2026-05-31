#include "SettingsManager.h"
#include <QFileInfo>

SettingsManager::SettingsManager(QObject *parent) 
    : QObject(parent), m_settings("SkyDev", "VLCReimagined") {}

SettingsManager& SettingsManager::instance() {
    static SettingsManager instance;
    return instance;
}

void SettingsManager::setVolume(float volume) {
    m_settings.setValue("playback/volume", volume);
}

float SettingsManager::getVolume() const {
    return m_settings.value("playback/volume", 0.5f).toFloat();
}

void SettingsManager::setLastPosition(const QString& filePath, qint64 position) {
    if(filePath.isEmpty()) return;
    QFileInfo info(filePath);
    m_settings.setValue("positions/" + info.fileName(), position);
}

qint64 SettingsManager::getLastPosition(const QString& filePath) const {
    if(filePath.isEmpty()) return 0;
    QFileInfo info(filePath);
    return m_settings.value("positions/" + info.fileName(), 0).toLongLong();
}

void SettingsManager::addRecentFile(const QString& filePath) {
    QStringList recents = getRecentFiles();
    recents.removeAll(filePath); // Remove if exists
    recents.prepend(filePath); // Add to top
    // Keep only last 10
    if (recents.size() > 10) {
        recents.pop_back();
    }
    m_settings.setValue("history/recentFiles", recents);
}

QStringList SettingsManager::getRecentFiles() const {
    return m_settings.value("history/recentFiles", QStringList()).toStringList();
}

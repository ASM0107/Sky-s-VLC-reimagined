#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <QStringList>

class SettingsManager : public QObject {
    Q_OBJECT
public:
    explicit SettingsManager(QObject *parent = nullptr);
    static SettingsManager& instance();

    // Volume
    void setVolume(float volume);
    float getVolume() const;

    // Last playback position
    void setLastPosition(const QString& filePath, qint64 position);
    qint64 getLastPosition(const QString& filePath) const;

    // Recent files
    void addRecentFile(const QString& filePath);
    QStringList getRecentFiles() const;

private:
    QSettings m_settings;
};

#endif // SETTINGSMANAGER_H

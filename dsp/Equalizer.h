#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <QObject>
#include <QList>
#include <QString>

class Equalizer : public QObject {
    Q_OBJECT
public:
    explicit Equalizer(QObject *parent = nullptr);

    enum class Preset {
        Flat,
        BassBoost,
        Rock,
        Classical,
        Vocal,
        Custom
    };

    void setPreset(Preset preset);
    void setBandGain(int bandIndex, float gainDb); // 0-9 for 10-band EQ
    void setPreampGain(float gainDb);
    void setEnabled(bool enabled);

    float getBandGain(int bandIndex) const;
    float getPreampGain() const;
    bool isEnabled() const;

signals:
    void bandGainChanged(int bandIndex, float gainDb);
    void preampGainChanged(float gainDb);
    void presetApplied(Preset preset);
    void stateChanged(bool enabled);

private:
    QList<float> m_bands;
    float m_preampGain;
    bool m_enabled;
};

#endif // EQUALIZER_H

#include "Equalizer.h"

Equalizer::Equalizer(QObject *parent) : QObject(parent), m_preampGain(0.0f), m_enabled(false) {
    for (int i = 0; i < 10; ++i) {
        m_bands.append(0.0f);
    }
}

void Equalizer::setPreset(Preset preset) {
    switch(preset) {
        case Preset::Flat:
            for(int i=0; i<10; ++i) m_bands[i] = 0.0f;
            break;
        case Preset::BassBoost:
            m_bands[0] = 6.0f; m_bands[1] = 5.0f; m_bands[2] = 4.0f;
            for(int i=3; i<10; ++i) m_bands[i] = 0.0f;
            break;
        case Preset::Rock:
            m_bands[0]=4.0f; m_bands[1]=3.0f; m_bands[2]=0.0f; m_bands[3]=-2.0f;
            m_bands[4]=-3.0f; m_bands[5]=0.0f; m_bands[6]=2.0f; m_bands[7]=3.0f;
            m_bands[8]=4.0f; m_bands[9]=5.0f;
            break;
        case Preset::Classical:
            m_bands[0]=5.0f; m_bands[1]=4.0f; m_bands[2]=3.0f; m_bands[3]=2.0f;
            m_bands[4]=-1.0f; m_bands[5]=-1.0f; m_bands[6]=0.0f; m_bands[7]=1.0f;
            m_bands[8]=3.0f; m_bands[9]=4.0f;
            break;
        case Preset::Vocal:
            m_bands[0]=-2.0f; m_bands[1]=-1.0f; m_bands[2]=0.0f; m_bands[3]=2.0f;
            m_bands[4]=4.0f; m_bands[5]=4.0f; m_bands[6]=2.0f; m_bands[7]=0.0f;
            m_bands[8]=-1.0f; m_bands[9]=-2.0f;
            break;
        default:
            break;
    }
    
    for(int i=0; i<10; ++i) {
        emit bandGainChanged(i, m_bands[i]);
    }
    emit presetApplied(preset);
}

void Equalizer::setBandGain(int bandIndex, float gainDb) {
    if (bandIndex >= 0 && bandIndex < 10) {
        m_bands[bandIndex] = gainDb;
        emit bandGainChanged(bandIndex, gainDb);
    }
}

void Equalizer::setPreampGain(float gainDb) {
    m_preampGain = gainDb;
    emit preampGainChanged(gainDb);
}

void Equalizer::setEnabled(bool enabled) {
    m_enabled = enabled;
    emit stateChanged(enabled);
}

float Equalizer::getBandGain(int bandIndex) const {
    if (bandIndex >= 0 && bandIndex < 10) return m_bands[bandIndex];
    return 0.0f;
}

float Equalizer::getPreampGain() const {
    return m_preampGain;
}

bool Equalizer::isEnabled() const {
    return m_enabled;
}

#include "EqualizerDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

EqualizerDialog::EqualizerDialog(Equalizer* eq, QWidget *parent) 
    : QDialog(parent), m_eq(eq), m_updatingUi(false) {
    
    setWindowTitle("Audio Equalizer");
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    QHBoxLayout* topLayout = new QHBoxLayout();
    m_enableCheck = new QCheckBox("Enable EQ");
    m_presetCombo = new QComboBox();
    m_presetCombo->addItems({"Flat", "Bass Boost", "Rock", "Classical", "Vocal", "Custom"});
    
    topLayout->addWidget(m_enableCheck);
    topLayout->addStretch();
    topLayout->addWidget(new QLabel("Preset:"));
    topLayout->addWidget(m_presetCombo);
    mainLayout->addLayout(topLayout);
    
    QHBoxLayout* slidersLayout = new QHBoxLayout();
    
    QStringList freqs = {"32", "64", "125", "250", "500", "1K", "2K", "4K", "8K", "16K"};
    
    for (int i = 0; i < 10; ++i) {
        QVBoxLayout* bandLayout = new QVBoxLayout();
        QSlider* slider = new QSlider(Qt::Vertical);
        slider->setRange(-120, 120); // -12dB to +12dB scaled by 10
        slider->setValue(0);
        
        connect(slider, &QSlider::valueChanged, this, [this, i](int val) {
            if (!m_updatingUi) {
                m_presetCombo->setCurrentIndex(static_cast<int>(Equalizer::Preset::Custom));
                m_eq->setBandGain(i, val / 10.0f);
            }
        });
        
        m_sliders.append(slider);
        bandLayout->addWidget(slider, 0, Qt::AlignHCenter);
        bandLayout->addWidget(new QLabel(freqs[i]), 0, Qt::AlignHCenter);
        slidersLayout->addLayout(bandLayout);
    }
    
    mainLayout->addLayout(slidersLayout);
    
    connect(m_enableCheck, &QCheckBox::toggled, m_eq, &Equalizer::setEnabled);
    connect(m_presetCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &EqualizerDialog::onPresetSelected);
    
    connect(m_eq, &Equalizer::presetApplied, this, &EqualizerDialog::updateUiFromEq);
    connect(m_eq, &Equalizer::bandGainChanged, this, [this](int band, float gain){
        if(!m_updatingUi) {
            m_updatingUi = true;
            m_sliders[band]->setValue(static_cast<int>(gain * 10));
            m_updatingUi = false;
        }
    });
    
    updateUiFromEq();
}

void EqualizerDialog::onPresetSelected(int index) {
    if (m_updatingUi) return;
    if (index == static_cast<int>(Equalizer::Preset::Custom)) return;
    m_eq->setPreset(static_cast<Equalizer::Preset>(index));
}

void EqualizerDialog::updateUiFromEq() {
    m_updatingUi = true;
    for (int i = 0; i < 10; ++i) {
        m_sliders[i]->setValue(static_cast<int>(m_eq->getBandGain(i) * 10));
    }
    m_enableCheck->setChecked(m_eq->isEnabled());
    m_updatingUi = false;
}

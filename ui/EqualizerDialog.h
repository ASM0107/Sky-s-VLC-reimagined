#ifndef EQUALIZERDIALOG_H
#define EQUALIZERDIALOG_H

#include <QDialog>
#include <QSlider>
#include <QComboBox>
#include <QCheckBox>
#include "../dsp/Equalizer.h"

class EqualizerDialog : public QDialog {
    Q_OBJECT
public:
    explicit EqualizerDialog(Equalizer* eq, QWidget *parent = nullptr);

private slots:
    void onPresetSelected(int index);
    void updateUiFromEq();

private:
    Equalizer* m_eq;
    QList<QSlider*> m_sliders;
    QComboBox* m_presetCombo;
    QCheckBox* m_enableCheck;
    bool m_updatingUi;
};

#endif // EQUALIZERDIALOG_H

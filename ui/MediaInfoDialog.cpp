#include "MediaInfoDialog.h"
#include <QVBoxLayout>
#include <QPushButton>

MediaInfoDialog::MediaInfoDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle("Media Information");
    setMinimumWidth(300);

    QFormLayout* form = new QFormLayout();
    
    m_titleLabel = new QLabel("-");
    m_codecLabel = new QLabel("-");
    m_resLabel = new QLabel("-");
    m_fpsLabel = new QLabel("-");
    
    form->addRow("Title:", m_titleLabel);
    form->addRow("Codec:", m_codecLabel);
    form->addRow("Resolution:", m_resLabel);
    form->addRow("FPS:", m_fpsLabel);
    
    QVBoxLayout* main = new QVBoxLayout(this);
    main->addLayout(form);
    
    QPushButton* closeBtn = new QPushButton("Close");
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    main->addWidget(closeBtn);
}

void MediaInfoDialog::setMetadata(const QVariantMap& metadata) {
    m_titleLabel->setText(metadata.value("title", "-").toString());
    m_codecLabel->setText(metadata.value("codec", "-").toString());
    m_resLabel->setText(metadata.value("resolution", "-").toString());
    m_fpsLabel->setText(metadata.value("fps", "-").toString());
}

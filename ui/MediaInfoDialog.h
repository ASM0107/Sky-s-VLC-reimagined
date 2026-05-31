#ifndef MEDIAINFODIALOG_H
#define MEDIAINFODIALOG_H

#include <QDialog>
#include <QVariantMap>
#include <QLabel>
#include <QFormLayout>

class MediaInfoDialog : public QDialog {
    Q_OBJECT
public:
    explicit MediaInfoDialog(QWidget *parent = nullptr);
    void setMetadata(const QVariantMap& metadata);

private:
    QLabel* m_titleLabel;
    QLabel* m_codecLabel;
    QLabel* m_resLabel;
    QLabel* m_fpsLabel;
};

#endif // MEDIAINFODIALOG_H

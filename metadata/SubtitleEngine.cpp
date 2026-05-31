#include "SubtitleEngine.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>

SubtitleEngine::SubtitleEngine(QObject *parent) : QObject(parent) {}

bool SubtitleEngine::loadSrt(const QString& filePath) {
    clear();
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);
    
    QRegularExpression timeRegex("(\\d{2}):(\\d{2}):(\\d{2}),(\\d{3})\\s*-->\\s*(\\d{2}):(\\d{2}):(\\d{2}),(\\d{3})");

    SubtitleBlock currentBlock;
    int state = 0; // 0=index, 1=time, 2=text

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        
        if (line.isEmpty()) {
            if (state == 2 && !currentBlock.text.isEmpty()) {
                currentBlock.text.replace("<br>", "\n"); // Some basic HTML strip/newline handle
                currentBlock.text.remove(QRegularExpression("<[^>]*>")); // strip tags
                m_blocks.append(currentBlock);
            }
            state = 0;
            currentBlock.text.clear();
            continue;
        }

        if (state == 0) {
            bool ok;
            line.toInt(&ok);
            if (ok) state = 1;
        } else if (state == 1) {
            QRegularExpressionMatch match = timeRegex.match(line);
            if (match.hasMatch()) {
                currentBlock.startTimeMs = 
                    match.captured(1).toInt() * 3600000 +
                    match.captured(2).toInt() * 60000 +
                    match.captured(3).toInt() * 1000 +
                    match.captured(4).toInt();
                    
                currentBlock.endTimeMs = 
                    match.captured(5).toInt() * 3600000 +
                    match.captured(6).toInt() * 60000 +
                    match.captured(7).toInt() * 1000 +
                    match.captured(8).toInt();
                state = 2;
            }
        } else if (state == 2) {
            if (!currentBlock.text.isEmpty())
                currentBlock.text += "\n";
            currentBlock.text += line;
        }
    }
    
    if (state == 2 && !currentBlock.text.isEmpty()) {
        currentBlock.text.remove(QRegularExpression("<[^>]*>"));
        m_blocks.append(currentBlock);
    }
    
    emit subtitlesLoaded();
    return true;
}

void SubtitleEngine::clear() {
    m_blocks.clear();
}

QString SubtitleEngine::getSubtitleAt(qint64 timeMs) const {
    // Binary search could be used here, but linear is fine for normal SRT sizes (~1-2k items)
    for (const auto& block : m_blocks) {
        if (timeMs >= block.startTimeMs && timeMs <= block.endTimeMs) {
            return block.text;
        }
    }
    return QString();
}

qint64 SubtitleEngine::parseTime(const QString& timeStr) const {
    return 0; // Handled inline in regex
}

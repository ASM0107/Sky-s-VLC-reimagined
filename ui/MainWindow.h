#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QSplitter>
#include <QMenu>
#include <QAction>
#include <QPropertyAnimation>

#include "../backend/IMediaBackend.h"
#include "../rendering/OpenGLRenderer.h"
#include "../metadata/MetadataEngine.h"
#include "../metadata/SubtitleEngine.h"
#include "../dsp/Equalizer.h"
#include "../include/PlaylistManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

private slots:
    void openFile();
    void openUrl();
    void openRecentFile();
    void togglePlayPause();
    void stopPlayback();
    void toggleMute();
    void toggleFullscreen();
    void toggleMiniPlayer();
    void showMediaInfo();
    void showEqualizer();
    void loadExternalSubtitle();
    
    void updatePosition(qint64 position);
    void updateDuration(qint64 duration);
    void seekToPosition(int position);
    void changeVolume(int volume);
    void changePlaybackRate(int index);
    
    void setDisplayMode(QAction* action);
    void setAspectRatio(QAction* action);
    
    void updateState(IMediaBackend::PlaybackState state);
    void handleError(const QString& errorMessage);
    
    void updatePlaylistUi();
    void playNext();
    void playPrevious();
    
    void addFilesToPlaylist();
    void removeSelectedPlaylistItems();
    void savePlaylist();
    void loadPlaylist();

private:
    void setupUi();
    void setupConnections();
    void setupMenus();
    void createVideoSubmenus(QMenu* parentMenu);
    void applyTheme();
    void populateRecentFiles();
    QString formatTime(qint64 ms) const;
    void applyLastPosition(const QUrl& url);
    void saveCurrentPosition();
    void animateControls(bool show);

    IMediaBackend* m_player;
    OpenGLRenderer* m_videoWidget;
    PlaylistManager* m_playlistManager;
    MetadataEngine* m_metadataEngine;
    SubtitleEngine* m_subtitleEngine;
    Equalizer* m_equalizer;

    // UI Elements
    QWidget* m_controlsWidget;
    QSplitter* m_mainSplitter;
    QListWidget* m_playlistUi;
    
    QSlider* m_seekSlider;
    QSlider* m_volumeSlider;
    
    QPushButton* m_playPauseBtn;
    QPushButton* m_stopBtn;
    QPushButton* m_nextBtn;
    QPushButton* m_prevBtn;
    QPushButton* m_muteBtn;
    QPushButton* m_fullscreenBtn;
    QPushButton* m_miniPlayerBtn;
    
    QLabel* m_timeLabel;
    QComboBox* m_speedCombo;
    
    QMenu* m_recentFilesMenu;
    QMenu* m_subtitleMenu;
    QPropertyAnimation* m_controlsAnimation;
    
    bool m_wasPlayingBeforeSeek = false;
    bool m_isMiniPlayer = false;
    QUrl m_currentUrl;
    QRect m_normalGeometry;
};

#endif // MAINWINDOW_H

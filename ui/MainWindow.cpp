#include "MainWindow.h"
#include "../settings/SettingsManager.h"
#include "../backend/QtMediaBackend.h"
#include "MediaInfoDialog.h"
#include "EqualizerDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QKeyEvent>
#include <QTime>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>
#include <QMenu>
#include <QMenuBar>
#include <QContextMenuEvent>
#include <QActionGroup>
#include <QStyle>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    
    m_player = new QtMediaBackend(this);
    m_playlistManager = new PlaylistManager(this);
    m_metadataEngine = new MetadataEngine(this);
    m_subtitleEngine = new SubtitleEngine(this);
    m_equalizer = new Equalizer(this);
    
    setAcceptDrops(true);
    setWindowTitle("Sky's VLC Reimagined Pro");
    resize(1024, 768); // Better default size
    
    setupUi();
    setupMenus();
    setupConnections();
    applyTheme();
    
    // Restore settings
    float vol = SettingsManager::instance().getVolume();
    m_volumeSlider->setValue(static_cast<int>(vol * 100));
    m_player->setVolume(vol);
}

MainWindow::~MainWindow() {
    saveCurrentPosition();
}

void MainWindow::setupUi() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    
    m_mainSplitter = new QSplitter(Qt::Horizontal, this);
    
    // Video section
    QWidget* videoContainer = new QWidget(this);
    QVBoxLayout* videoLayout = new QVBoxLayout(videoContainer);
    videoLayout->setContentsMargins(0, 0, 0, 0);
    
    m_videoWidget = new OpenGLRenderer(this);
    m_videoWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_videoWidget->installEventFilter(this);
    m_player->setVideoSink(m_videoWidget->videoSink());
    videoLayout->addWidget(m_videoWidget);
    
    // Playlist section
    QWidget* playlistContainer = new QWidget(this);
    QVBoxLayout* playlistLayout = new QVBoxLayout(playlistContainer);
    playlistLayout->setContentsMargins(5, 5, 5, 5);
    
    m_playlistUi = new QListWidget(this);
    QHBoxLayout* playlistBtns = new QHBoxLayout();
    QPushButton* addBtn = new QPushButton("+", this);
    QPushButton* remBtn = new QPushButton("-", this);
    QPushButton* saveBtn = new QPushButton("Save", this);
    QPushButton* loadBtn = new QPushButton("Load", this);
    playlistBtns->addWidget(addBtn);
    playlistBtns->addWidget(remBtn);
    playlistBtns->addWidget(saveBtn);
    playlistBtns->addWidget(loadBtn);
    
    playlistLayout->addWidget(new QLabel("Playlist", this));
    playlistLayout->addWidget(m_playlistUi);
    playlistLayout->addLayout(playlistBtns);
    
    m_mainSplitter->addWidget(videoContainer);
    m_mainSplitter->addWidget(playlistContainer);
    m_mainSplitter->setStretchFactor(0, 8); // Video container priority
    m_mainSplitter->setStretchFactor(1, 2); // Playlist priority
    
    mainLayout->addWidget(m_mainSplitter, 8); // Video area 80%
    
    // Controls section
    m_controlsWidget = new QWidget(this);
    m_controlsWidget->setObjectName("controlsBar");
    QVBoxLayout* controlsLayout = new QVBoxLayout(m_controlsWidget);
    controlsLayout->setContentsMargins(5, 2, 5, 2); // Tighter margins
    controlsLayout->setSpacing(2); // Tighter spacing
    
    // Seek bar
    QHBoxLayout* seekLayout = new QHBoxLayout();
    m_timeLabel = new QLabel("00:00 / 00:00", this);
    m_seekSlider = new QSlider(Qt::Horizontal, this);
    m_seekSlider->setRange(0, 0);
    seekLayout->addWidget(m_seekSlider);
    seekLayout->addWidget(m_timeLabel);
    
    // Buttons
    QHBoxLayout* btnsLayout = new QHBoxLayout();
    m_playPauseBtn = new QPushButton("▶", this);
    m_stopBtn = new QPushButton("⏹", this);
    m_prevBtn = new QPushButton("⏮", this);
    m_nextBtn = new QPushButton("⏭", this);
    m_muteBtn = new QPushButton("🔊", this);
    m_fullscreenBtn = new QPushButton("⛶", this);
    m_miniPlayerBtn = new QPushButton("Mini", this);
    
    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    m_volumeSlider->setRange(0, 100);
    m_volumeSlider->setMaximumWidth(100);
    
    m_speedCombo = new QComboBox(this);
    m_speedCombo->addItems({"0.5x", "1.0x", "1.25x", "1.5x", "2.0x"});
    m_speedCombo->setCurrentIndex(1);
    
    btnsLayout->addWidget(m_prevBtn);
    btnsLayout->addWidget(m_playPauseBtn);
    btnsLayout->addWidget(m_stopBtn);
    btnsLayout->addWidget(m_nextBtn);
    btnsLayout->addSpacing(20);
    btnsLayout->addWidget(m_muteBtn);
    btnsLayout->addWidget(m_volumeSlider);
    btnsLayout->addStretch();
    btnsLayout->addWidget(new QLabel("Speed:", this));
    btnsLayout->addWidget(m_speedCombo);
    btnsLayout->addWidget(m_miniPlayerBtn);
    btnsLayout->addWidget(m_fullscreenBtn);
    
    controlsLayout->addLayout(seekLayout);
    controlsLayout->addLayout(btnsLayout);
    mainLayout->addWidget(m_controlsWidget, 2); // Controls area 20%
    
    setCentralWidget(centralWidget);
    
    // Prevent UI elements from stealing keyboard focus so shortcuts work reliably
    m_seekSlider->setFocusPolicy(Qt::NoFocus);
    m_volumeSlider->setFocusPolicy(Qt::NoFocus);
    m_speedCombo->setFocusPolicy(Qt::NoFocus);
    m_playPauseBtn->setFocusPolicy(Qt::NoFocus);
    m_stopBtn->setFocusPolicy(Qt::NoFocus);
    m_prevBtn->setFocusPolicy(Qt::NoFocus);
    m_nextBtn->setFocusPolicy(Qt::NoFocus);
    m_muteBtn->setFocusPolicy(Qt::NoFocus);
    m_fullscreenBtn->setFocusPolicy(Qt::NoFocus);
    m_miniPlayerBtn->setFocusPolicy(Qt::NoFocus);
    m_playlistUi->setFocusPolicy(Qt::NoFocus);
    
    m_controlsAnimation = new QPropertyAnimation(m_controlsWidget, "maximumHeight", this);
    m_controlsAnimation->setDuration(200);
    
    // Connect playlist buttons
    connect(addBtn, &QPushButton::clicked, this, &MainWindow::addFilesToPlaylist);
    connect(remBtn, &QPushButton::clicked, this, &MainWindow::removeSelectedPlaylistItems);
    connect(saveBtn, &QPushButton::clicked, this, &MainWindow::savePlaylist);
    connect(loadBtn, &QPushButton::clicked, this, &MainWindow::loadPlaylist);
}

void MainWindow::setupConnections() {
    connect(m_playPauseBtn, &QPushButton::clicked, this, &MainWindow::togglePlayPause);
    connect(m_stopBtn, &QPushButton::clicked, this, &MainWindow::stopPlayback);
    connect(m_muteBtn, &QPushButton::clicked, this, &MainWindow::toggleMute);
    connect(m_fullscreenBtn, &QPushButton::clicked, this, &MainWindow::toggleFullscreen);
    connect(m_miniPlayerBtn, &QPushButton::clicked, this, &MainWindow::toggleMiniPlayer);
    connect(m_nextBtn, &QPushButton::clicked, this, &MainWindow::playNext);
    connect(m_prevBtn, &QPushButton::clicked, this, &MainWindow::playPrevious);
    
    connect(m_seekSlider, &QSlider::sliderPressed, this, [this]() {
        m_wasPlayingBeforeSeek = (m_player->state() == IMediaBackend::PlaybackState::Playing);
        m_player->pause();
    });
    connect(m_seekSlider, &QSlider::sliderMoved, this, &MainWindow::seekToPosition);
    connect(m_seekSlider, &QSlider::sliderReleased, this, [this]() {
        if (m_wasPlayingBeforeSeek) m_player->play();
    });
    
    connect(m_volumeSlider, &QSlider::valueChanged, this, &MainWindow::changeVolume);
    connect(m_speedCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::changePlaybackRate);
    
    connect(m_player, &IMediaBackend::positionChanged, this, &MainWindow::updatePosition);
    connect(m_player, &IMediaBackend::durationChanged, this, &MainWindow::updateDuration);
    connect(m_player, &IMediaBackend::stateChanged, this, &MainWindow::updateState);
    connect(m_player, &IMediaBackend::errorOccurred, this, &MainWindow::handleError);
    connect(m_player, &IMediaBackend::mediaEnded, this, &MainWindow::playNext);
    
    connect(m_videoWidget, &OpenGLRenderer::doubleClicked, this, &MainWindow::toggleFullscreen);
    
    connect(m_playlistManager, &PlaylistManager::playlistChanged, this, &MainWindow::updatePlaylistUi);
    connect(m_playlistManager, &PlaylistManager::currentIndexChanged, this, [this](int index) {
        if (index >= 0 && index < m_playlistManager->getItems().size()) {
            m_playlistUi->setCurrentRow(index);
            m_currentUrl = m_playlistManager->getItems()[index].url;
            SettingsManager::instance().addRecentFile(m_currentUrl.toString());
            populateRecentFiles();
            m_subtitleEngine->clear();
            m_videoWidget->setCurrentSubtitle("");
            m_player->setSource(m_currentUrl);
            m_player->play();
            applyLastPosition(m_currentUrl);
        } else {
            m_player->stop();
        }
    });
    
    connect(m_playlistUi, &QListWidget::doubleClicked, this, [this](const QModelIndex& index) {
        m_playlistManager->setCurrentIndex(index.row());
    });
}

void MainWindow::setupMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction("Open File...", this, &MainWindow::openFile, QKeySequence::Open);
    fileMenu->addAction("Open URL...", this, &MainWindow::openUrl);
    m_recentFilesMenu = fileMenu->addMenu("Recent Files");
    populateRecentFiles();
    fileMenu->addSeparator();
    fileMenu->addAction("Exit", this, &QWidget::close, QKeySequence::Quit);
    
    QMenu* playbackMenu = menuBar()->addMenu("&Playback");
    playbackMenu->addAction("Play/Pause", this, &MainWindow::togglePlayPause, Qt::Key_Space);
    playbackMenu->addAction("Stop", this, &MainWindow::stopPlayback);
    playbackMenu->addSeparator();
    playbackMenu->addAction("Next", this, &MainWindow::playNext, Qt::Key_MediaNext);
    playbackMenu->addAction("Previous", this, &MainWindow::playPrevious, Qt::Key_MediaPrevious);
    
    QMenu* toolsMenu = menuBar()->addMenu("&Tools");
    toolsMenu->addAction("Media Information...", this, &MainWindow::showMediaInfo, Qt::CTRL | Qt::Key_I);
    toolsMenu->addAction("Audio Equalizer...", this, &MainWindow::showEqualizer, Qt::CTRL | Qt::Key_E);
    m_subtitleMenu = menuBar()->addMenu("&Subtitles");
    QAction* loadSubAction = m_subtitleMenu->addAction("Load External Subtitle...");
    connect(loadSubAction, &QAction::triggered, this, &MainWindow::loadExternalSubtitle);
    m_subtitleMenu->addSeparator();
    QAction* disableSubAction = m_subtitleMenu->addAction("Disable Subtitles");
    disableSubAction->setCheckable(true);
    disableSubAction->setChecked(true);
    connect(disableSubAction, &QAction::triggered, this, [this](bool checked) {
        if (checked) {
            m_subtitleEngine->clear();
            m_videoWidget->setCurrentSubtitle("");
        }
    });
    
    QMenu* videoMenu = menuBar()->addMenu("&Video");
    videoMenu->addAction("Fullscreen", this, &MainWindow::toggleFullscreen, Qt::Key_F);
    videoMenu->addAction("Mini Player", this, &MainWindow::toggleMiniPlayer, Qt::CTRL | Qt::Key_M);
    videoMenu->addSeparator();
    createVideoSubmenus(videoMenu);
}

void MainWindow::createVideoSubmenus(QMenu* parentMenu) {
    QMenu* displayMenu = parentMenu->addMenu("Display Mode");
    QActionGroup* displayGroup = new QActionGroup(this);
    QStringList displayModes = {"Fit", "Fill", "Stretch", "100%"};
    for (const QString& mode : displayModes) {
        QAction* action = displayMenu->addAction(mode);
        action->setCheckable(true);
        action->setActionGroup(displayGroup);
        if (mode == "Fit") action->setChecked(true);
    }
    connect(displayMenu, &QMenu::triggered, this, &MainWindow::setDisplayMode);

    QMenu* aspectMenu = parentMenu->addMenu("Aspect Ratio");
    QActionGroup* aspectGroup = new QActionGroup(this);
    QStringList aspectModes = {"Auto", "16:9", "4:3", "21:9"};
    for (const QString& mode : aspectModes) {
        QAction* action = aspectMenu->addAction(mode);
        action->setCheckable(true);
        action->setActionGroup(aspectGroup);
        if (mode == "Auto") action->setChecked(true);
    }
    connect(aspectMenu, &QMenu::triggered, this, &MainWindow::setAspectRatio);
}

void MainWindow::setDisplayMode(QAction* action) {
    QString text = action->text();
    if (text == "Fit") m_videoWidget->setDisplayMode(OpenGLRenderer::DisplayMode::Fit);
    else if (text == "Fill") m_videoWidget->setDisplayMode(OpenGLRenderer::DisplayMode::Fill);
    else if (text == "Stretch") m_videoWidget->setDisplayMode(OpenGLRenderer::DisplayMode::Stretch);
    else if (text == "100%") m_videoWidget->setDisplayMode(OpenGLRenderer::DisplayMode::Native100);
}

void MainWindow::setAspectRatio(QAction* action) {
    QString text = action->text();
    if (text == "Auto") m_videoWidget->setAspectRatio(OpenGLRenderer::AspectRatio::Auto);
    else if (text == "16:9") m_videoWidget->setAspectRatio(OpenGLRenderer::AspectRatio::AR_16_9);
    else if (text == "4:3") m_videoWidget->setAspectRatio(OpenGLRenderer::AspectRatio::AR_4_3);
    else if (text == "21:9") m_videoWidget->setAspectRatio(OpenGLRenderer::AspectRatio::AR_21_9);
}

void MainWindow::applyTheme() {
    QString qss = "QMainWindow { background-color: #121212; color: #ffffff; }"
                  "#controlsBar { background-color: #1e1e1e; border-top: 1px solid #333; }"
                  "QPushButton { background-color: #2c2c2c; color: white; border: none; padding: 6px 15px; border-radius: 4px; }"
                  "QPushButton:hover { background-color: #ff8c00; color: #121212; }"
                  "QPushButton:pressed { background-color: #e67e00; }"
                  "QListWidget { background-color: #1e1e1e; color: white; border: 1px solid #333; }"
                  "QListWidget::item:selected { background-color: #ff8c00; color: #121212; }"
                  "QSlider::groove:horizontal { border: 1px solid #333; height: 6px; background: #444; border-radius: 3px; }"
                  "QSlider::sub-page:horizontal { background: #ff8c00; border-radius: 3px; }"
                  "QSlider::handle:horizontal { background: #ffffff; border: 1px solid #ff8c00; width: 14px; margin: -4px 0; border-radius: 7px; }"
                  "QMenuBar { background-color: #121212; color: white; }"
                  "QMenuBar::item:selected { background-color: #2c2c2c; color: #ff8c00; }"
                  "QMenu { background-color: #1e1e1e; color: white; border: 1px solid #333; }"
                  "QMenu::item:selected { background-color: #ff8c00; color: #121212; }";
                  
    setStyleSheet(qss);
    QFile styleFile("styles/dark_theme.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        setStyleSheet(styleFile.readAll());
        styleFile.close();
    }
}

void MainWindow::populateRecentFiles() {
    m_recentFilesMenu->clear();
    QStringList recents = SettingsManager::instance().getRecentFiles();
    for (const QString& file : recents) {
        QAction* action = m_recentFilesMenu->addAction(QFileInfo(file).fileName());
        action->setData(file);
        connect(action, &QAction::triggered, this, &MainWindow::openRecentFile);
    }
    if (recents.isEmpty()) {
        m_recentFilesMenu->addAction("No recent files")->setEnabled(false);
    }
}

void MainWindow::openRecentFile() {
    if (QAction* action = qobject_cast<QAction*>(sender())) {
        m_playlistManager->addMedia(QUrl(action->data().toString()));
        m_playlistManager->setCurrentIndex(m_playlistManager->getItems().size() - 1);
    }
}

void MainWindow::openFile() {
    QStringList files = QFileDialog::getOpenFileNames(this, "Open Media", QString(), "Media (*.mp4 *.mkv *.mp3 *.wav *.avi *.flac);;All (*)");
    QList<QUrl> urls;
    for (const QString& f : files) urls.append(QUrl::fromLocalFile(f));
    if (!urls.isEmpty()) {
        m_playlistManager->addMediaList(urls);
        if (m_playlistManager->currentIndex() == -1) m_playlistManager->setCurrentIndex(0);
    }
}

void MainWindow::openUrl() {
    bool ok;
    QString text = QInputDialog::getText(this, "Open Network Stream", "URL (e.g. m3u8, rtsp):", QLineEdit::Normal, "", &ok);
    if (ok && !text.isEmpty()) {
        m_playlistManager->addMedia(QUrl(text));
        if (m_playlistManager->currentIndex() == -1) m_playlistManager->setCurrentIndex(0);
    }
}

void MainWindow::addFilesToPlaylist() { openFile(); }

void MainWindow::removeSelectedPlaylistItems() {
    for (auto* item : m_playlistUi->selectedItems()) {
        m_playlistManager->removeMedia(m_playlistUi->row(item));
    }
}

void MainWindow::savePlaylist() {
    QString f = QFileDialog::getSaveFileName(this, "Save", "playlists/playlist.json", "JSON (*.json)");
    if (!f.isEmpty()) m_playlistManager->saveToJson(f);
}

void MainWindow::loadPlaylist() {
    QString f = QFileDialog::getOpenFileName(this, "Load", "playlists", "JSON (*.json)");
    if (!f.isEmpty()) m_playlistManager->loadFromJson(f);
}

void MainWindow::togglePlayPause() {
    if (m_player->state() == IMediaBackend::PlaybackState::Playing) m_player->pause();
    else m_player->play();
}

void MainWindow::stopPlayback() { m_player->stop(); }

void MainWindow::toggleMute() {
    bool isMuted = m_muteBtn->text() == "🔇";
    m_player->setMuted(!isMuted);
    m_muteBtn->setText(!isMuted ? "🔇" : "🔊");
}



void MainWindow::toggleFullscreen() {
    if (isFullScreen()) {
        showNormal();
        menuBar()->show();
        if(!m_isMiniPlayer) m_mainSplitter->widget(1)->show();
        animateControls(true);
    } else {
        showFullScreen();
        menuBar()->hide();
        m_mainSplitter->widget(1)->hide();
        animateControls(false);
    }
}

void MainWindow::toggleMiniPlayer() {
    m_isMiniPlayer = !m_isMiniPlayer;
    QRect currentGeo = geometry();
    QPoint center = currentGeo.center();

    if (m_isMiniPlayer) {
        m_normalGeometry = currentGeo; // Save state
        m_mainSplitter->widget(1)->hide();
        menuBar()->hide();
        
        QRect newGeo(0, 0, 400, 300);
        newGeo.moveCenter(center);
        setGeometry(newGeo);
        
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
        show();
    } else {
        m_mainSplitter->widget(1)->show();
        menuBar()->show();
        
        QRect newGeo = m_normalGeometry.isValid() ? m_normalGeometry : QRect(0, 0, 1024, 768);
        newGeo.moveCenter(center);
        setGeometry(newGeo);
        
        setWindowFlags(windowFlags() & ~Qt::WindowStaysOnTopHint);
        show();
    }
}

void MainWindow::showMediaInfo() {
    if(m_currentUrl.isEmpty()) return;
    MediaInfoDialog* dlg = new MediaInfoDialog(this);
    connect(m_metadataEngine, &MetadataEngine::metadataReady, dlg, [dlg, this](const QUrl& url, const QVariantMap& meta) {
        if (url == m_currentUrl) dlg->setMetadata(meta);
    });
    m_metadataEngine->extractMetadataAsync(m_currentUrl);
    dlg->show();
}

void MainWindow::loadExternalSubtitle() {
    QString f = QFileDialog::getOpenFileName(this, "Load Subtitle", "", "Subtitles (*.srt *.ass *.ssa);;All (*)");
    if (!f.isEmpty()) {
        if (m_subtitleEngine->loadSrt(f)) {
            // Uncheck disable option
            for (auto* action : m_subtitleMenu->actions()) {
                if (action->text() == "Disable Subtitles") {
                    action->setChecked(false);
                }
            }
        }
    }
}

void MainWindow::showEqualizer() {
    EqualizerDialog* dlg = new EqualizerDialog(m_equalizer, this);
    dlg->show();
}

void MainWindow::playNext() {
    int nextIdx = m_playlistManager->currentIndex() + 1;
    if (nextIdx < m_playlistManager->getItems().size()) m_playlistManager->setCurrentIndex(nextIdx);
    else m_player->stop();
}

void MainWindow::playPrevious() {
    int prevIdx = m_playlistManager->currentIndex() - 1;
    if (prevIdx >= 0) m_playlistManager->setCurrentIndex(prevIdx);
}

void MainWindow::updatePosition(qint64 pos) {
    if (!m_seekSlider->isSliderDown()) m_seekSlider->setValue(pos);
    m_timeLabel->setText(formatTime(pos) + " / " + formatTime(m_player->duration()));
    
    // Update subtitle overlay
    m_videoWidget->setCurrentSubtitle(m_subtitleEngine->getSubtitleAt(pos));
}

void MainWindow::updateDuration(qint64 dur) {
    m_seekSlider->setRange(0, dur);
    m_timeLabel->setText(formatTime(m_player->position()) + " / " + formatTime(dur));
}

void MainWindow::seekToPosition(int pos) { m_player->seek(pos); }

void MainWindow::changeVolume(int vol) {
    float v = vol / 100.0f;
    m_player->setVolume(v);
    SettingsManager::instance().setVolume(v);
}

void MainWindow::changePlaybackRate(int idx) {
    qreal r[] = {0.5, 1.0, 1.25, 1.5, 2.0};
    m_player->setPlaybackRate(r[idx]);
}

void MainWindow::updateState(IMediaBackend::PlaybackState state) {
    m_playPauseBtn->setText(state == IMediaBackend::PlaybackState::Playing ? "⏸" : "▶");
}

void MainWindow::handleError(const QString& msg) {
    QMessageBox::critical(this, "Error", msg);
}

void MainWindow::updatePlaylistUi() {
    m_playlistUi->clear();
    for (const auto& item : m_playlistManager->getItems()) m_playlistUi->addItem(item.title);
    if (m_playlistManager->currentIndex() != -1) m_playlistUi->setCurrentRow(m_playlistManager->currentIndex());
}

QString MainWindow::formatTime(qint64 ms) const {
    int s = (ms / 1000) % 60, m = (ms / 60000) % 60, h = (ms / 3600000);
    if (h > 0) return QString("%1:%2:%3").arg(h, 2, 10, QChar('0')).arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
    return QString("%1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0'));
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    Qt::KeyboardModifiers mods = event->modifiers();

    // Volume control
    if ((key == Qt::Key_Up && (mods & Qt::ControlModifier)) || key == Qt::Key_Plus) {
        m_volumeSlider->setValue(qMin(100, m_volumeSlider->value() + 5));
        return;
    }
    if ((key == Qt::Key_Down && (mods & Qt::ControlModifier)) || key == Qt::Key_Minus) {
        m_volumeSlider->setValue(qMax(0, m_volumeSlider->value() - 5));
        return;
    }

    // Seeking control
    if (key == Qt::Key_Left || key == Qt::Key_Right) {
        qint64 jump = 10000; // default 10s
        if ((mods & Qt::ControlModifier) && (mods & Qt::AltModifier)) jump = 300000; // 5m
        else if (mods & Qt::ControlModifier) jump = 60000; // 1m
        else if (mods & Qt::AltModifier) jump = 10000; // 10s
        else if (mods & Qt::ShiftModifier) jump = 3000; // 3s
        else jump = 10000; // un-modified default 10s

        if (key == Qt::Key_Left) m_player->seek(qMax(0LL, m_player->position() - jump));
        else m_player->seek(qMin(m_player->duration(), m_player->position() + jump));
        return;
    }

    // Playback and UI control
    if (key == Qt::Key_Space) togglePlayPause();
    else if (key == Qt::Key_S) stopPlayback();
    else if (key == Qt::Key_N) playNext();
    else if (key == Qt::Key_P) playPrevious();
    else if (key == Qt::Key_M) toggleMute();
    else if (key == Qt::Key_F || key == Qt::Key_F11) toggleFullscreen();
    else if (key == Qt::Key_Escape && isFullScreen()) toggleFullscreen();
    else if (key == Qt::Key_A) m_videoWidget->cycleAspectRatio();
    else if (key == Qt::Key_C) m_videoWidget->cycleDisplayMode();
    
    // Playback Speed control
    else if (key == Qt::Key_BracketLeft) {
        m_speedCombo->setCurrentIndex(qMax(0, m_speedCombo->currentIndex() - 1));
    } else if (key == Qt::Key_BracketRight) {
        m_speedCombo->setCurrentIndex(qMin(m_speedCombo->count() - 1, m_speedCombo->currentIndex() + 1));
    } else if (key == Qt::Key_Equal) {
        m_speedCombo->setCurrentIndex(1); // 1.0x
    }
    
    QMainWindow::keyPressEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event) {
    // Only show context menu over the video widget or when we're in fullscreen
    if (m_videoWidget->underMouse() || isFullScreen() || m_isMiniPlayer) {
        QMenu contextMenu(this);
        
        contextMenu.addAction("Play/Pause", this, &MainWindow::togglePlayPause);
        contextMenu.addAction("Stop", this, &MainWindow::stopPlayback);
        contextMenu.addSeparator();
        
        QMenu* videoSub = contextMenu.addMenu("Video");
        videoSub->addAction("Fullscreen", this, &MainWindow::toggleFullscreen);
        videoSub->addAction("Mini Player", this, &MainWindow::toggleMiniPlayer);
        videoSub->addSeparator();
        createVideoSubmenus(videoSub);
        
        contextMenu.addSeparator();
        contextMenu.addAction("Audio Equalizer...", this, &MainWindow::showEqualizer);
        contextMenu.addAction("Media Information...", this, &MainWindow::showMediaInfo);
        
        contextMenu.exec(event->globalPos());
    } else {
        QMainWindow::contextMenuEvent(event);
    }
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (obj == m_videoWidget && isFullScreen()) {
        if (event->type() == QEvent::MouseMove) {
            animateControls(true);
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::animateControls(bool show) {
    if (show && m_controlsWidget->maximumHeight() == 0) {
        m_controlsAnimation->setStartValue(0);
        m_controlsAnimation->setEndValue(70);
        m_controlsAnimation->start();
        QTimer::singleShot(3000, this, [this](){ if(isFullScreen()) animateControls(false); });
    } else if (!show && m_controlsWidget->maximumHeight() > 0) {
        m_controlsAnimation->setStartValue(70);
        m_controlsAnimation->setEndValue(0);
        m_controlsAnimation->start();
    }
}



void MainWindow::dragEnterEvent(QDragEnterEvent *event) {
    if (event->mimeData()->hasUrls()) event->acceptProposedAction();
}

void MainWindow::dropEvent(QDropEvent *event) {
    QList<QUrl> urls = event->mimeData()->urls();
    if (!urls.isEmpty()) {
        m_playlistManager->addMediaList(urls);
        if (m_playlistManager->currentIndex() == -1) m_playlistManager->setCurrentIndex(0);
    }
}

void MainWindow::applyLastPosition(const QUrl& url) {
    if (url.isLocalFile()) {
        qint64 lastPos = SettingsManager::instance().getLastPosition(url.toLocalFile());
        if (lastPos > 0) m_player->seek(lastPos);
    }
}

void MainWindow::saveCurrentPosition() {
    if (m_currentUrl.isLocalFile() && m_player->position() > 0) {
        if (m_player->duration() > 0 && (m_player->duration() - m_player->position() < 2000))
            SettingsManager::instance().setLastPosition(m_currentUrl.toLocalFile(), 0);
        else
            SettingsManager::instance().setLastPosition(m_currentUrl.toLocalFile(), m_player->position());
    }
}

void MainWindow::closeEvent(QCloseEvent *event) {
    saveCurrentPosition();
    QMainWindow::closeEvent(event);
}

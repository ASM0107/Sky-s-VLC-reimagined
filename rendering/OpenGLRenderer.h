#ifndef OPENGLRENDERER_H
#define OPENGLRENDERER_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QVideoSink>
#include <QVideoFrame>
#include <QMutex>
#include <QString>

class OpenGLRenderer : public QOpenGLWidget, protected QOpenGLFunctions {
    Q_OBJECT
public:
    enum class DisplayMode { Fit, Fill, Stretch, Native100 };
    enum class AspectRatio { Auto, AR_16_9, AR_4_3, AR_21_9 };

    explicit OpenGLRenderer(QWidget *parent = nullptr);
    ~OpenGLRenderer() override;

    QVideoSink* videoSink() const;
    void setCurrentSubtitle(const QString& text);
    
    void setDisplayMode(DisplayMode mode);
    void setAspectRatio(AspectRatio ratio);
    
    void cycleDisplayMode();
    void cycleAspectRatio();

signals:
    void doubleClicked();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;

private slots:
    void setVideoFrame(const QVideoFrame& frame);

private:
    void updateTexture(const QImage& image);
    void drawSubtitles();

    QVideoSink* m_videoSink;
    QVideoFrame m_currentFrame;
    QMutex m_mutex;
    QString m_currentSubtitle;

    QOpenGLShaderProgram* m_program;
    QOpenGLTexture* m_texture;
    QOpenGLBuffer m_vbo;
    QOpenGLVertexArrayObject m_vao;
    QSize m_frameSize;
    
    DisplayMode m_displayMode = DisplayMode::Fit;
    AspectRatio m_aspectRatio = AspectRatio::Auto;
};

#endif // OPENGLRENDERER_H

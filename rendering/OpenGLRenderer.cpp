#include "OpenGLRenderer.h"
#include <QPainter>
#include <QMouseEvent>
#include <QImage>
#include <QPainterPath>

OpenGLRenderer::OpenGLRenderer(QWidget *parent) : QOpenGLWidget(parent), m_program(nullptr), m_texture(nullptr) {
    m_videoSink = new QVideoSink(this);
    connect(m_videoSink, &QVideoSink::videoFrameChanged, this, &OpenGLRenderer::setVideoFrame);
}

OpenGLRenderer::~OpenGLRenderer() {
    makeCurrent();
    delete m_texture;
    delete m_program;
    m_vao.destroy();
    m_vbo.destroy();
    doneCurrent();
}

QVideoSink* OpenGLRenderer::videoSink() const {
    return m_videoSink;
}

void OpenGLRenderer::setCurrentSubtitle(const QString& text) {
    QMutexLocker locker(&m_mutex);
    if (m_currentSubtitle != text) {
        m_currentSubtitle = text;
        update();
    }
}

void OpenGLRenderer::setVideoFrame(const QVideoFrame& frame) {
    QMutexLocker locker(&m_mutex);
    m_currentFrame = frame;
    update();
}

void OpenGLRenderer::setDisplayMode(DisplayMode mode) {
    if (m_displayMode != mode) {
        m_displayMode = mode;
        update();
    }
}

void OpenGLRenderer::setAspectRatio(AspectRatio ratio) {
    if (m_aspectRatio != ratio) {
        m_aspectRatio = ratio;
        update();
    }
}

void OpenGLRenderer::cycleDisplayMode() {
    int current = static_cast<int>(m_displayMode);
    current = (current + 1) % 4; // 4 modes
    setDisplayMode(static_cast<DisplayMode>(current));
}

void OpenGLRenderer::cycleAspectRatio() {
    int current = static_cast<int>(m_aspectRatio);
    current = (current + 1) % 4; // 4 aspect ratios
    setAspectRatio(static_cast<AspectRatio>(current));
}

void OpenGLRenderer::initializeGL() {
    initializeOpenGLFunctions();
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_program = new QOpenGLShaderProgram(this);
    m_program->addShaderFromSourceCode(QOpenGLShader::Vertex,
        "attribute vec2 position;\n"
        "attribute vec2 texCoord;\n"
        "varying vec2 v_texCoord;\n"
        "void main() {\n"
        "    gl_Position = vec4(position, 0.0, 1.0);\n"
        "    v_texCoord = texCoord;\n"
        "}\n");
    m_program->addShaderFromSourceCode(QOpenGLShader::Fragment,
        "uniform sampler2D textureSampler;\n"
        "varying vec2 v_texCoord;\n"
        "void main() {\n"
        "    gl_FragColor = texture2D(textureSampler, v_texCoord);\n"
        "}\n");
    m_program->link();

    float vertices[] = {
        // Position (X, Y)  // TexCoords (U, V)
        -1.0f, -1.0f,       0.0f, 1.0f, // Bottom-left (inverted V for QImage mapping)
         1.0f, -1.0f,       1.0f, 1.0f, // Bottom-right
        -1.0f,  1.0f,       0.0f, 0.0f, // Top-left
         1.0f,  1.0f,       1.0f, 0.0f  // Top-right
    };

    m_vao.create();
    m_vao.bind();

    m_vbo.create();
    m_vbo.bind();
    m_vbo.allocate(vertices, sizeof(vertices));

    m_program->bind();
    int posAttr = m_program->attributeLocation("position");
    m_program->enableAttributeArray(posAttr);
    m_program->setAttributeBuffer(posAttr, GL_FLOAT, 0, 2, 4 * sizeof(float));

    int texAttr = m_program->attributeLocation("texCoord");
    m_program->enableAttributeArray(texAttr);
    m_program->setAttributeBuffer(texAttr, GL_FLOAT, 2 * sizeof(float), 2, 4 * sizeof(float));

    m_program->release();
    m_vao.release();
}

void OpenGLRenderer::resizeGL(int w, int h) {
    glViewport(0, 0, w, h);
}

void OpenGLRenderer::updateTexture(const QImage& image) {
    if (!m_texture || m_frameSize != image.size()) {
        delete m_texture;
        m_texture = new QOpenGLTexture(QOpenGLTexture::Target2D);
        m_texture->setFormat(QOpenGLTexture::RGBA8_UNorm);
        m_texture->setSize(image.width(), image.height());
        m_texture->allocateStorage();
        m_texture->setMinificationFilter(QOpenGLTexture::Linear);
        m_texture->setMagnificationFilter(QOpenGLTexture::Linear);
        m_texture->setWrapMode(QOpenGLTexture::ClampToEdge);
        m_frameSize = image.size();
    }
    m_texture->setData(QOpenGLTexture::RGBA, QOpenGLTexture::UInt8, image.constBits());
}

void OpenGLRenderer::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    QImage imgToDraw;
    QString subToDraw;
    {
        QMutexLocker locker(&m_mutex);
        if (m_currentFrame.isValid()) {
            imgToDraw = m_currentFrame.toImage();
            // Ensure format is compatible with basic texture upload without conversion overhead if possible
            if(imgToDraw.format() != QImage::Format_RGBA8888) {
                imgToDraw = imgToDraw.convertToFormat(QImage::Format_RGBA8888);
            }
        }
        subToDraw = m_currentSubtitle;
    }

    if (!imgToDraw.isNull()) {
        updateTexture(imgToDraw);

        float frameAspect = static_cast<float>(imgToDraw.width()) / imgToDraw.height();
        
        switch (m_aspectRatio) {
            case AspectRatio::AR_16_9: frameAspect = 16.0f / 9.0f; break;
            case AspectRatio::AR_4_3: frameAspect = 4.0f / 3.0f; break;
            case AspectRatio::AR_21_9: frameAspect = 21.0f / 9.0f; break;
            default: break; // Auto
        }
        
        float widgetAspect = static_cast<float>(width()) / height();
        
        qreal dpr = devicePixelRatio();
        int devWidth = qRound(width() * dpr);
        int devHeight = qRound(height() * dpr);
        
        int viewWidth = devWidth;
        int viewHeight = devHeight;
        int viewX = 0;
        int viewY = 0;

        if (m_displayMode == DisplayMode::Stretch) {
            // Do nothing, use full widget area
        } else if (m_displayMode == DisplayMode::Native100) {
            viewWidth = qRound(imgToDraw.width() * dpr); // Optional: multiply by DPR if we want 1:1 logical pixels
            viewHeight = static_cast<int>(viewWidth / frameAspect);
            viewX = (devWidth - viewWidth) / 2;
            viewY = (devHeight - viewHeight) / 2;
        } else {
            bool fitByWidth = (widgetAspect < frameAspect);
            if (m_displayMode == DisplayMode::Fill) {
                fitByWidth = !fitByWidth; // Reverse logic for fill (crop)
            }
            
            if (fitByWidth) {
                viewWidth = devWidth;
                viewHeight = static_cast<int>(devWidth / frameAspect);
                viewX = 0;
                viewY = (devHeight - viewHeight) / 2;
            } else {
                viewHeight = devHeight;
                viewWidth = static_cast<int>(devHeight * frameAspect);
                viewY = 0;
                viewX = (devWidth - viewWidth) / 2;
            }
        }

        glViewport(viewX, viewY, viewWidth, viewHeight);

        m_program->bind();
        m_texture->bind();
        m_vao.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        m_vao.release();
        m_texture->release();
        m_program->release();
        
        // Restore viewport for QPainter
        glViewport(0, 0, devWidth, devHeight);
    }

    // Overlay Subtitles
    if (!subToDraw.isEmpty()) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::TextAntialiasing);

        QFont font("Sans Serif", qMax(16, height() / 25), QFont::Bold);
        painter.setFont(font);
        
        int margin = height() / 15;
        QRect textRect(0, 0, width(), height() - margin);

        QPainterPath path;
        path.addText(textRect.center().x(), textRect.bottom(), font, subToDraw); // simplified centering

        // Accurate multi-line bounding box and rendering
        painter.setPen(QPen(Qt::black, 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter.drawText(textRect, Qt::AlignBottom | Qt::AlignHCenter | Qt::TextWordWrap, subToDraw);
        
        painter.setPen(QPen(Qt::white, 1));
        painter.drawText(textRect, Qt::AlignBottom | Qt::AlignHCenter | Qt::TextWordWrap, subToDraw);
    }
}

void OpenGLRenderer::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit doubleClicked();
    }
    QOpenGLWidget::mouseDoubleClickEvent(event);
}

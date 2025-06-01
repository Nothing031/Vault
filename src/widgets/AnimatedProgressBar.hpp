#pragma once

#include <QObject>
#include <QProgressBar>
#include <QPropertyAnimation>

class AnimatedProgressBar : public QProgressBar
{
    Q_OBJECT
    Q_PROPERTY(QColor chunkColor READ _chunkColor WRITE _setChunkColor)
public:
    AnimatedProgressBar(QWidget* parent = nullptr);
    void setAnimationProperty(const QColor& base, const QColor& start, const QColor& end, const int& duration);

public slots:
    void startAnimation();
    void stopAnimation();

private:
    QColor _chunkColor() const;
    void _setChunkColor(const QColor& color);
    void updateStyleSheet();

private:
    QPropertyAnimation *animation;
    QColor m_baseChunkColor;
    QColor m_chunkColor;
};

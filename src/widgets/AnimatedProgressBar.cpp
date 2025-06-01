#include "AnimatedProgressBar.hpp"

AnimatedProgressBar::AnimatedProgressBar(QWidget *parent) :
    QProgressBar(parent),
    animation(new QPropertyAnimation(this, "chunkColor", this))
{

}

void AnimatedProgressBar::setAnimationProperty(const QColor &base, const QColor &start, const QColor &end, const int &duration)
{
    m_baseChunkColor = base;
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setDuration(duration);
}

void AnimatedProgressBar::startAnimation()
{
    stopAnimation();
    animation->start();
}

void AnimatedProgressBar::stopAnimation()
{
    animation->stop();
    m_chunkColor = m_baseChunkColor;
    updateStyleSheet();
}

QColor AnimatedProgressBar::_chunkColor() const
{
    return m_chunkColor;
}

void AnimatedProgressBar::_setChunkColor(const QColor &color)
{
    m_chunkColor = color;
    updateStyleSheet();
}

void AnimatedProgressBar::updateStyleSheet()
{
    QString style = QString(
                        "QProgressBar::chunk { background-color: rgba(%1, %2, %3, %4); }")
                        .arg(m_chunkColor.red())
                        .arg(m_chunkColor.green())
                        .arg(m_chunkColor.blue())
                        .arg(m_chunkColor.alpha());
    this->setStyleSheet(style);
}

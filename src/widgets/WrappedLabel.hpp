#pragma once

#include <QLabel>
#include <QTextBlock>
#include <QTextLayout>
#include <QPainter>

class WrappedLabel : public QLabel {
public:
    using QLabel::QLabel;

protected:
    void paintEvent(QPaintEvent* event) override ;

public:
    QSize sizeHint() const override;
};

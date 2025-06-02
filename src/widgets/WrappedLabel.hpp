#pragma once

#include <QLabel>
#include <QPainter>
#include <QTextBlock>
#include <QTextLayout>

#define NO_WRAPPEDLABEL
#ifndef NO_WRAPPEDLABEL

class WrappedLabel : public QLabel {
public:
    using QLabel::QLabel;

protected:
    void paintEvent(QPaintEvent* event) override ;

public:
    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

private:

};

#endif

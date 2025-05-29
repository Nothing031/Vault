#include "WrappedLabel.hpp"

#include <QLabel>
#include <QTextBlock>
#include <QTextLayout>
#include <QTextOption>
#include <QPainter>
#include <QStyle>

void WrappedLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setFont(font());

    QTextLayout layout(text(), font());
    QTextOption option;
    option.setWrapMode(QTextOption::WrapAnywhere);
    layout.setTextOption(option);
    layout.beginLayout();

    QVector<QTextLine> lines;
    qreal totalHeight = 0;

    while (true) {
        QTextLine line = layout.createLine();
        if (!line.isValid()) break;

        line.setLineWidth(width());
        lines.append(line);
        totalHeight += line.height();
    }
    layout.endLayout();

    qreal yOffset = 0;
    if (alignment() & Qt::AlignVCenter) {
        yOffset = (height() - totalHeight) / 2.0;
    } else if (alignment() & Qt::AlignBottom) {
        yOffset = height() - totalHeight;
    }

    qreal y = yOffset;
    for (auto& line : std::as_const(lines)) {
        QPointF pos;
        pos.setY(y);

        if (alignment() & Qt::AlignHCenter) {
            pos.setX((width() - line.naturalTextWidth()) / 2.0);
        } else if (alignment() & Qt::AlignRight) {
            pos.setX(width() - line.naturalTextWidth());
        } else {
            pos.setX(0); // Qt::AlignLeft or default
        }

        line.draw(&painter, pos);
        y += line.height();
    }
}

QSize WrappedLabel::sizeHint() const
{
    QTextLayout layout(text(), font());
    layout.beginLayout();
    qreal height = 0;
    while (true) {
        QTextLine line = layout.createLine();
        if (!line.isValid()) break;
        line.setLineWidth(width());
        height += line.height();
    }
    layout.endLayout();
    return QSize(width(), static_cast<int>(height));
}


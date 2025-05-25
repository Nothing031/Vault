#include "WrappedLabel.hpp"

#include <QLabel>
#include <QTextBlock>
#include <QTextLayout>
#include <QPainter>

void WrappedLabel::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setFont(font());

    QString text = this->text();

    QTextLayout textLayout(text, font());
    QTextOption option;
    option.setWrapMode(QTextOption::WrapAnywhere);
    textLayout.setTextOption(option);

    textLayout.beginLayout();

    QVector<QTextLine> lines;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(width());
        lines.append(line);
    }
    textLayout.endLayout();

    QPointF position(0, 0);
    for (const QTextLine &line : lines) {
        line.draw(&painter, position);
        position.ry() += line.height();
    }
}

QSize WrappedLabel::sizeHint() const
{
    QString text = this->text();

    QTextLayout textLayout(text, font());
    QTextOption option;
    option.setWrapMode(QTextOption::WrapAnywhere);
    textLayout.setTextOption(option);

    textLayout.beginLayout();
    QVector<QTextLine> lines;
    while (true) {
        QTextLine line = textLayout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(width());
        lines.append(line);
    }
    textLayout.endLayout();

    QPointF position(0, 0);
    int height = 0;
    for (const QTextLine &line : lines) {
        position.ry() += line.height();
        height = position.ry();
    }
    return QSize(260, height);
}


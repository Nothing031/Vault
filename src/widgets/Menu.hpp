#pragma once

#include <QMenu>

class Menu : public QMenu
{
    Q_OBJECT
public:
    explicit Menu(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent* event) override;

signals:

public slots:

};


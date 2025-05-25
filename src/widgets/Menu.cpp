#include "Menu.hpp"

#include <QEvent>
#include <QPaintEvent>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QPainterPath>


Menu::Menu(QWidget *parent) :
    QMenu(parent)
{
    setAttribute(Qt::WA_TranslucentBackground);
    setWindowFlags(Qt::Popup | Qt::FramelessWindowHint | Qt::NoDropShadowWindowHint);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setStyleSheet(R"(
QMenu{
    background: rgb(50, 50, 50);
    color: rgb(240, 240, 240);
    font: 10pt;
    border: 1px solid rgb(70, 70, 70);
    background: rgb(50, 50, 50);
    border-radius: 5px;
}QMenu::separator{
    height:1px;
    background: rgb(70, 70, 70);
    margin-left: 5px;
    margin-right: 5px;
}
QMenu::item {
    margin: 2px 2px 2px 2px;
    padding: 3px 20px;
    background: transparent;
}
QMenu::item:selected {
    background: rgb(70, 70, 70);
    border-radius: 3px;
}
)");
}

void Menu::paintEvent(QPaintEvent *event)
{
    QMenu::paintEvent(event);
}


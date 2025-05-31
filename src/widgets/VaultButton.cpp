#include "VaultButton.hpp"

#include <QContextMenuEvent>
#include <QGraphicsDropShadowEffect>
#include <QMenu>
#include <QPainter>
#include <QFont>
#include <QRect>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QTimer>
#include <QDesktopServices>

#include "WrappedLabel.hpp"
#include "Menu.hpp"

VaultButton::VaultButton(std::shared_ptr<Vault> s_vault, QWidget* parent)
    : QPushButton(parent),
    vault(s_vault)
{
    this->setMaximumWidth(300);
    this->setMinimumWidth(300);
    this->setStyleSheet(R"(
QPushButton{
    background: rgb(40, 40, 40);
    border: 0px;
}
QPushButton:hover{
    background: rgb(50, 50, 50);
}
QPushButton::pressed{
    background: rgb(40, 40, 40);
}
    )");

    QVBoxLayout* vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(20, 20, 20, 20);
    vLayout->setSpacing(5);

    titleLabel = new QLabel(QUrl(vault->directory.path()).fileName() , this);
    titleLabel->setMaximumWidth(260);
    titleLabel->setMinimumWidth(260);
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    titleLabel->setStyleSheet(R"(
color: rgb(255, 255, 255);
font: 700;
background: transparent;
    )");


    subtitleLabel = new QLabel(vault->directory.path(), this);
    subtitleLabel->setMaximumWidth(260);
    subtitleLabel->setMinimumWidth(260);
    subtitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    subtitleLabel->setWordWrap(true);
    subtitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    subtitleLabel->setStyleSheet(R"(
color: rgb(200, 200, 200);
font: 9pt;
background: transparent;
    )");

    vLayout->addWidget(titleLabel);
    vLayout->addWidget(subtitleLabel);
    setLayout(vLayout);
}

QSize VaultButton::sizeHint() const
{
    return QSize(300 , titleLabel->sizeHint().height() + subtitleLabel->sizeHint().height() + 20 + 20 + 5);
}

std::shared_ptr<Vault> VaultButton::getVault()
{
    return vault;
}

void VaultButton::UpdateDirectory()
{
    titleLabel->setText(QUrl(vault->directory.path()).fileName());
    subtitleLabel->setText(vault->directory.path());
    int margin = 20;
    int spacing = 5;
    QSize size(this->sizeHint().width(),
               titleLabel->sizeHint().height() + subtitleLabel->sizeHint().height() + spacing + margin * 2);
    emit onSizeHintChange(size, this);
}

void VaultButton::contextMenuEvent(QContextMenuEvent *event)
{
    Menu menu;
    QAction* openAction = new QAction("open", &menu);
    QAction* detachAction = new QAction("detach", &menu);
    QAction* openPathAction = new QAction("open path", &menu);
    menu.addAction(openAction);
    menu.addAction(openPathAction);
    menu.addSeparator();
    menu.addAction(detachAction);

    connect(openAction, &QAction::triggered, this, [this](){
        emit requestOpenVault(vault);
    });
    connect(openPathAction, &QAction::triggered, this, [this](){
        QDesktopServices::openUrl(QUrl(vault->directory.path()));
    });
    connect(detachAction, &QAction::triggered, this, [this](){
        emit requestDetachVault(vault);
    });

    menu.exec(event->globalPos());
}


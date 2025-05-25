#include "VaultButton.hpp"

#include <QStyleOptionButton>
#include <QContextMenuEvent>
#include <QGraphicsDropShadowEffect>
#include <QMenu>
#include <QPainter>
#include <QFont>
#include <QRect>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QTextBlock>
#include <QTextStream>
#include <QTextLayout>

#include <QTimer>

#include "WrappedLabel.hpp"
#include "Menu.hpp"

VaultButton::VaultButton(Vault* vault, QWidget* parent)
    : QPushButton(parent),
    m_vault(nullptr)
{
    this->m_vault = vault;

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

    titleLabel = new WrappedLabel(QUrl(vault->directory.path()).fileName() , this);
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


    subtitleLabel = new WrappedLabel(vault->directory.path(), this);
    subtitleLabel->setMaximumWidth(260);
    subtitleLabel->setMinimumWidth(260);
    subtitleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
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

Vault *VaultButton::getVault()
{
    return m_vault;
}

void VaultButton::UpdateDirectory()
{
    titleLabel->setText(QUrl(m_vault->directory.path()).fileName());
    subtitleLabel->setText(m_vault->directory.path());
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
    QAction* deleteAction = new QAction("delete", &menu);
    menu.addAction(openAction);
    menu.addSeparator();
    menu.addAction(deleteAction);

    connect(openAction, &QAction::triggered, this, [this](){
        emit requestOpenVault(m_vault);
    });
    connect(deleteAction, &QAction::triggered, this, [this](){
        emit requestDetachVault(m_vault);
    });

    menu.exec(event->globalPos());
}


#include "VaultInfoButton.hpp"

#include <QStyleOptionButton>
#include <QPainter>
#include <QFont>
#include <QRect>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QTextBlock>
#include <QTextStream>
#include <QTextLayout>

#include "WrapLabel.hpp"

VaultInfoButton::VaultInfoButton(Vault* vault, QWidget* parent)
    : QPushButton(parent),
    vault(nullptr)
{
    this->vault = vault;

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

    vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(20, 20, 20, 20);
    vLayout->setSpacing(5);

    QFileInfo path(vault->directory.path());

    titleLabel = new QLabel(path.fileName() , this);
    titleLabel->setMaximumWidth(260);
    titleLabel->setMinimumWidth(260);
    titleLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    titleLabel->setStyleSheet(R"(
color: rgb(255, 255, 255);
background: transparent;
    )");


    subtitleLabel = new WrapLabel(path.absoluteFilePath(), this);
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
}


QSize VaultInfoButton::sizeHint() const
{
    return QSize(300 , titleLabel->sizeHint().height() + subtitleLabel->sizeHint().height() + 20 + 20 + 5);
}

Vault *VaultInfoButton::getVault()
{
    return vault;
}


#include "VaultTitle.hpp"
#include "ui_VaultTitle.h"

VaultTitle::VaultTitle(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::VaultTitle)
{
    ui->setupUi(this);
    connect(ui->CreateButton, &QPushButton::clicked, this, &VaultTitle::createButtonPressed);
    connect(ui->OpenButton, &QPushButton::clicked, this, &VaultTitle::openButtonPressed);
}

VaultTitle::~VaultTitle()
{
    delete ui;
}

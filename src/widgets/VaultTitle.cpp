#include "VaultTitle.hpp"

VaultTitle::VaultTitle(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::VaultTitle)
{
    ui->setupUi(this);

}

VaultTitle::~VaultTitle()
{
    delete ui;
}

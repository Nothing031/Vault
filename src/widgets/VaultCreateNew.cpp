#include "VaultCreateNew.hpp"

VaultCreateNew::VaultCreateNew(QWidget *parent)
    : QWidget(parent),
    ui(new Ui::VaultCreateNew)
{
    ui->setupUi(this);





}

VaultCreateNew::~VaultCreateNew()
{
    delete ui;
}

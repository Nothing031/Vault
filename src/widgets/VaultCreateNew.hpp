#pragma once

#include <QWidget>

#include "ui_VaultCreateNew.h"

namespace Ui{
class VaultCreateNew;
}

class VaultCreateNew : public QWidget
{
    Q_OBJECT
public:
    explicit VaultCreateNew(QWidget *parent = nullptr);
    ~VaultCreateNew();

signals:

private:
    Ui::VaultCreateNew *ui;

};

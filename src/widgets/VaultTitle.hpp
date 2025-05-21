#pragma once

#include <QWidget>

#include "ui_VaultTitle.h"

namespace Ui{
class VaultTitle;
}

class VaultTitle : public QWidget
{
    Q_OBJECT
public:
    explicit VaultTitle(QWidget *parent = nullptr);
    ~VaultTitle();

signals:

private:
    Ui::VaultTitle *ui;
};

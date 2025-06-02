#pragma once

#include <QWidget>

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
    void openButtonPressed();
    void createButtonPressed();

private:
    Ui::VaultTitle *ui;
};

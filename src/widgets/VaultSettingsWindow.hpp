#pragma once


#include <QMainWindow>

namespace Ui {
class VaultSettingsWindow;
}

class VaultSettingsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit VaultSettingsWindow(QWidget *parent = nullptr);
    ~VaultSettingsWindow();

private:
    Ui::VaultSettingsWindow *ui;
};



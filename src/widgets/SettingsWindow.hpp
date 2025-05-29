#pragma once

#include "ui_SettingsWindow.h"

#include <QMainWindow>

namespace Ui{
class SettingsWindow;
}

class SettingsWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();


private:
    Ui::SettingsWindow *ui;
};


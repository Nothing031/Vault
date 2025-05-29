#include "SettingsWindow.hpp"


#pragma comment(lib, "dwmapi.lib")
#include <dwmapi.h>
#include <Windows.h>

SettingsWindow::SettingsWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    // dark caption
    HWND hWnd = (HWND)this->winId();
    BOOL USE_DARK_MODE = true;
    DwmSetWindowAttribute(hWnd, DWMWA_USE_IMMERSIVE_DARK_MODE, &USE_DARK_MODE, sizeof(USE_DARK_MODE));
    // flag
    setAttribute(Qt::WA_DeleteOnClose);
















}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

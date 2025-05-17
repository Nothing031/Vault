#pragma once

#include <QMainWindow>
#include <QVector>
#include <QDir>
#include <QStringList>


namespace Ui {
class FrontendMainWindow;
}

class FrontendMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FrontendMainWindow(QWidget *parent = nullptr);
    ~FrontendMainWindow();

private slots:


private:
    Ui::FrontendMainWindow *ui;

};


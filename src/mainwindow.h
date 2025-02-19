#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "vault.h"
#include "crypto.hpp"
#include <mutex>
#include <vector>
#include <QFileSystemWatcher>
#include <string>
#include <QStandardItemModel>

using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:


private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H

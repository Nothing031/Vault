#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include "qdir.h"
#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QStandardItemModel>

#include <QDir>

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

    void on_vault_select_comboBox_currentIndexChanged(int index);

    void on_vault_createNew_button_clicked();

    void on_vault_createExisting_button_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

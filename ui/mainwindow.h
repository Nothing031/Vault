#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <QMainWindow>
#include <QFileSystemWatcher>
#include <QStandardItemModel>
#include "window_crypto.h"

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

private:
    Ui::MainWindow *ui;
    Window_crypto *window_crypto;
};
#endif // MAINWINDOW_H

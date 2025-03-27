#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QDir>
#include <QStringList>


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
    enum class page : int{
        Empty = 0,
        NewVault = 1,
        Crypto = 2
    };

    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H

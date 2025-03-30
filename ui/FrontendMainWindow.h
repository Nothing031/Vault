#ifndef FRONTENDMAINWINDOW_H
#define FRONTENDMAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QDir>
#include <QStringList>


QT_BEGIN_NAMESPACE
namespace Ui {
class FrontendMainWindow;
}
QT_END_NAMESPACE

class FrontendMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    FrontendMainWindow(QWidget *parent = nullptr);
    
    ~FrontendMainWindow();

private slots:


private:
    enum class page : int{
        Empty = 0,
        NewVault = 1,
        Crypto = 2
    };

    Ui::FrontendMainWindow *ui;
};
#endif // FRONTENDMAINWINDOW_H

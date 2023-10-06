#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QTcpServer;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleButtonPress();
    void newConnection();

private:
    void initServer();

    Ui::MainWindow *ui;
    QTcpServer *tcpServer = nullptr;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"

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
    void handleDisconnect();

private:
    void initServer();

    Ui::MainWindow *ui;
    QTcpServer *tcpServer = nullptr;
    QVector<Client*> clients;
};
#endif // MAINWINDOW_H

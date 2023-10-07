#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
class QTcpServer;
QT_END_NAMESPACE

typedef enum
{
    CLOSED,
    LISTENING
} serverstate_t;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void handleListenButton();
    void newConnection();
    void handleDisconnect();

private:
    void startListen();
    QVector<Client*>::iterator removeConnection(QVector<Client*>::iterator it);
    void closeConnections();

    Ui::MainWindow *ui;
    QTcpServer *tcpServer_ = nullptr;
    QVector<Client*> clients_;
    serverstate_t serverstate_;
};
#endif // MAINWINDOW_H

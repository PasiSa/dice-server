#include <QtNetwork>
#include <QMessageBox>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->listenButton, SIGNAL(clicked()), this, SLOT(handleButtonPress()));
}


MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::handleButtonPress()
{
    QString text = ui->portEdit->text();
    qDebug() << "Button pressed: " << text;
    initServer();
    connect(tcpServer, &QTcpServer::newConnection, this, &MainWindow::newConnection);
}


void MainWindow::newConnection()
{
    qDebug() << "New connection";

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected,
            this, &MainWindow::handleDisconnect);

    QString name = QString("%1/%2")
                .arg(clientConnection->peerAddress().toString())
                .arg(clientConnection->peerPort());

    int rowIndex = ui->connectionsTable->rowCount();
    ui->connectionsTable->insertRow(rowIndex);
    QTableWidgetItem *addrItem = new QTableWidgetItem(name);
    ui->connectionsTable->setItem(rowIndex, 0, addrItem);
    QTableWidgetItem *bytesItem = new QTableWidgetItem("0");
    ui->connectionsTable->setItem(rowIndex, 1, bytesItem);

    Client *c = new Client(name, clientConnection, bytesItem, rowIndex);
    connect(clientConnection, &QIODevice::readyRead, c, &Client::readBytes);
    clients.push_back(c);
}


void MainWindow::handleDisconnect()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    for(QVector<Client*>::iterator it = clients.begin(); it != clients.end(); ++it)
    {
        if ((*it)->isMySocket(sock))
        {
            qDebug() << "Erasing " << (*it)->getName();
            ui->connectionsTable->removeRow((*it)->getRowIndex());
            delete *it;
            clients.erase(it);
            return;
        }
    }
    qDebug() << "handleDisconnect called but no matching socket found";
}


void MainWindow::initServer()
{
    tcpServer = new QTcpServer(this);
    if (!tcpServer->listen()) {
        QMessageBox::critical(this, tr("Fortune Server"),
                              tr("Unable to start the server: %1.")
                              .arg(tcpServer->errorString()));
        close();
        return;
    }

    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();

    ui->ipAddrLabel->setText(ipAddress);
    ui->portEdit->setText(QString::number(tcpServer->serverPort()));
}

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
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_10);

    out << "Jepeti jee";

    QTcpSocket *clientConnection = tcpServer->nextPendingConnection();
    connect(clientConnection, &QAbstractSocket::disconnected,
            clientConnection, &QObject::deleteLater);

    int rowIndex = ui->connectionsTable->rowCount();
    ui->connectionsTable->insertRow(rowIndex);
    QTableWidgetItem *addrItem = new QTableWidgetItem(tr("%1/%2")
                .arg(clientConnection->peerAddress().toString())
                .arg(clientConnection->peerPort()));
    ui->connectionsTable->setItem(rowIndex, 0, addrItem);
    QTableWidgetItem *bytesItem = new QTableWidgetItem("0");
    ui->connectionsTable->setItem(rowIndex, 1, bytesItem);

    clientConnection->write(block);
    clientConnection->disconnectFromHost();
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

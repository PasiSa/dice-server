#include <QtNetwork>
#include <QMessageBox>

#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), serverstate_(CLOSED)
{
    ui->setupUi(this);
    ui->portEdit->setValidator(new QIntValidator(1, 65535, this));
    connect(ui->listenButton, SIGNAL(clicked()), this, SLOT(handleListenButton()));
    ui->connectionsTable->setColumnWidth(0, 160);
    log_.setQTextBrowser(ui->connectionBrowser);
}


MainWindow::~MainWindow()
{
    closeConnections();
    delete ui;
}


void MainWindow::handleListenButton()
{
    if (serverstate_ == CLOSED) {
        startListen();
        ui->activityLabel->setText("Server is accepting connections");
        ui->activityLabel->setStyleSheet("color: green;");
        ui->listenButton->setText("Close");
        serverstate_ = LISTENING;
    } else {
        closeConnections();
        ui->activityLabel->setText("Server is not active");
        ui->activityLabel->setStyleSheet("color: red;");
        ui->listenButton->setText("Listen");
        serverstate_ = CLOSED;

    }
}


void MainWindow::newConnection()
{
    QTcpSocket *clientConnection = tcpServer_->nextPendingConnection();
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
    clients_.push_back(c);
}


void MainWindow::handleDisconnect()
{
    QTcpSocket *sock = qobject_cast<QTcpSocket*>(sender());
    for(QVector<Client*>::iterator it = clients_.begin(); it != clients_.end(); ++it)
    {
        if ((*it)->isMySocket(sock))
        {
            removeConnection(it);
            return;
        }
    }
    qDebug() << "handleDisconnect called but no matching socket found";
}


void MainWindow::startListen()
{
    int port = 0;
    if (ui->portEdit->text().length() > 0) {
        port = ui->portEdit->text().toInt();
    }
    tcpServer_ = new QTcpServer(this);
    if (!tcpServer_->listen(QHostAddress::Any, port)) {
        QMessageBox::critical(this, tr("Dice Server"),
                              tr("Failed to bind to a port: %1.")
                              .arg(tcpServer_->errorString()));
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
    ui->portEdit->setText(QString::number(tcpServer_->serverPort()));
    connect(tcpServer_, &QTcpServer::newConnection, this, &MainWindow::newConnection);
}


QVector<Client*>::iterator MainWindow::removeConnection(QVector<Client*>::iterator it)
{
    qDebug() << "Erasing " << (*it)->getName();

    (*it)->makeLogEntry(log_);

    (*it)->removeFromTable(ui->connectionsTable);
    QTcpSocket *sock = (*it)->getSocket();
    disconnect(sock, &QAbstractSocket::disconnected, this, &MainWindow::handleDisconnect);
    sock->disconnectFromHost();
    delete *it;
    return clients_.erase(it);
}

void MainWindow::closeConnections()
{
    for(QVector<Client*>::iterator it = clients_.begin(); it != clients_.end(); ) {
        it = removeConnection(it);
    }
}

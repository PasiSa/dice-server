#include <QtNetwork>
#include "client.h"
#include "mainwindow.h"

Client::Client(QString name, QTcpSocket *sock, QTableWidgetItem *byteWidget, int rowIndex)
        : name_(name), socket_(sock), byteWidget_(byteWidget), rowIndex_(rowIndex), byteCount_(0)
{
    qDebug() << "constructor called";
}


Client::Client(Client&& c)
{
    qDebug() << "move constructor called";
    name_ = c.name_;
    socket_ = c.socket_;
    byteWidget_ = c.byteWidget_;
    rowIndex_ = c.rowIndex_;
    c.socket_ = nullptr;
}


Client::~Client()
{
    qDebug() << "deleting client " << name_;
    if (socket_)
        socket_->deleteLater();
}


void Client::readBytes()
{
    QByteArray data = socket_->readAll();
    qDebug() << "Received data: " << data;
    byteCount_ += data.size();
    byteWidget_->setText(QString::number(byteCount_));
}

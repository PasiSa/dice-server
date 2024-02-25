#include "client.h"
#include "mainwindow.h"

Client::Client(QString name, QTcpSocket *sock, QTableWidgetItem *byteWidget, int rowIndex)
        : name_(name),
        socket_(sock),
        byteWidget_(byteWidget),
        byteCount_(0),
        startTime_(QDateTime::currentDateTime())
{
}


Client::Client(Client&& c)
{
    qDebug() << "move constructor called";
    name_ = c.name_;
    socket_ = c.socket_;
    byteWidget_ = c.byteWidget_;
    c.socket_ = nullptr;
}


Client::~Client()
{
    qDebug() << "deleting client " << name_;
    if (socket_)
        socket_->deleteLater();
}


void Client::removeFromTable(QTableWidget *table)
{
    QList<QTableWidgetItem *> found = table->findItems(name_, Qt::MatchExactly);
    if (found.begin() != found.end()) {
        QTableWidgetItem *item = *(found.begin());
        table->removeRow(item->row());
    }
}


void Client::makeLogEntry(ConnectionLog& log) const
{
    QDateTime currentTime = QDateTime::currentDateTime();
    qint64 msecs = startTime_.msecsTo(currentTime);
    double bps = (double) byteCount_ / ((double) msecs / 1000);

    QString line = QString("%1: %2; %3 bytes; %4 ms; %5 b/s")
            .arg(currentTime.time().toString("hh:mm:ss"))
            .arg(name_)
            .arg(byteCount_)
            .arg(msecs)
            .arg(bps);
    log.addLine(line);
}


void Client::readBytes()
{
    QByteArray data = socket_->readAll();
    byteCount_ += data.size();
    byteWidget_->setText(QString::number(byteCount_));
}

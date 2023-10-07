#ifndef CLIENT_H
#define CLIENT_H

#include <QTableWidgetItem>
#include <QTableWidget>
#include <QTcpSocket>
#include <QString>
#include <QObject>
#include <QMainWindow>
#include <QDateTime>

#include "connectionlog.h"

class Client : public QObject
{
    Q_OBJECT

public:
    Client(QString name, QTcpSocket *sock, QTableWidgetItem *byteWidget, int rowIndex);
    ~Client();
    Client& operator=(const Client&) { qDebug() << "assignment called"; return *this; }
    Client(const Client &c) { qDebug() << "copy constructor called"; }
    Client(Client&& c);

    void removeFromTable(QTableWidget *table);
    void makeLogEntry(ConnectionLog& log) const;

    bool isMySocket(QTcpSocket *sock) const { return sock == socket_; }
    QTcpSocket *getSocket() const { return socket_; }
    const QString getName() const { return name_; }

public slots:
    void readBytes();

private:
    QString name_;
    QTcpSocket *socket_;
    QTableWidgetItem *byteWidget_;  // owned by the QTableWidget, do not delete
    unsigned int byteCount_;
    QDateTime startTime_;
};

#endif // CLIENT_H

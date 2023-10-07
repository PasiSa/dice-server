#ifndef CONNECTIONLOG_H
#define CONNECTIONLOG_H

#include <QTextBrowser>
#include <QString>

class ConnectionLog
{
public:
    ConnectionLog();

    void setQTextBrowser(QTextBrowser *browser) { browser_ = browser; }
    void addLine(const QString& str);

private:
    QTextBrowser* browser_ = nullptr;  // no ownership
    QString log_;
};

#endif  // CONNECTIONLOG_H

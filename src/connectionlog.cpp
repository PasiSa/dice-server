#include "connectionlog.h"

ConnectionLog::ConnectionLog()
{
}


void ConnectionLog::addLine(const QString& str)
{
    log_.append(str);
    log_.append("\n");
    if (browser_) {
        browser_->setPlainText(log_);
    }
}
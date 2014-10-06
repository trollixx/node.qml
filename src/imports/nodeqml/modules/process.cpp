#include "process.h"

#include <QCoreApplication>

Process::Process(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{
}

void Process::exit(int code) const
{
    QCoreApplication::exit(code);
}

QStringList Process::argv() const
{
    return QCoreApplication::arguments();
}

int Process::pid() const
{
    /// NOTE: QCoreApplication::applicationPid() returns int64
    return QCoreApplication::applicationPid();
}

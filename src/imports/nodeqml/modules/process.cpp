#include "process.h"

#include <QCoreApplication>

Process::Process(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{
}

int Process::pid() const
{
    /// NOTE: QCoreApplication::applicationPid() returns int64
    return QCoreApplication::applicationPid();
}

#include "os.h"

#include <QDir>
#include <QHostInfo>

Os::Os(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{

}

QString Os::tmpdir() const
{
    return QDir::tempPath();
}

QString Os::endianness() const
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return QStringLiteral("LE");
#else
    return QStringLiteral("BE");
#endif
}

QString Os::hostname() const
{
    return QHostInfo::localHostName();
}

QString Os::eol() const
{
#ifdef Q_OS_WIN
    return QStringLiteral("\r\n");
#else
    return QStringLiteral("\n");
#endif
}

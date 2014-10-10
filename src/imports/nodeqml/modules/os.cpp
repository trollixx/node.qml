#include "os.h"

#include <QDir>

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

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

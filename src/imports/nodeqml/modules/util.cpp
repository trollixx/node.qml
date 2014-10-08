#include "util.h"

#include <QJSValue>
#include <QTextStream>

Util::Util(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{

}

void Util::debug(const QString &string) const
{
    QTextStream stream(stderr);
    stream << string;
}

bool Util::isArray(const QJSValue &object) const
{
    return object.isArray();
}

bool Util::isRegExp(const QJSValue &object) const
{
    return object.isRegExp();
}

bool Util::isDate(const QJSValue &object) const
{
    return object.isDate();
}

bool Util::isError(const QJSValue &object) const
{
    return object.isError();
}

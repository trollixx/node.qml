#include "util.h"

#include <QJSValue>

Util::Util(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{

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

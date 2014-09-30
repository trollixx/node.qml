#ifndef UTIL_H
#define UTIL_H

#include "coremodule.h"

class QJSValue;

class Util : public CoreModule
{
    Q_OBJECT
public:
    explicit Util(QJSEngine *jsEngine, QObject *parent = nullptr);

    /// TODO: util.format(format, [...])
    /// TODO: util.debug(string)
    /// TODO: util.error([...])
    /// TODO: util.puts([...])
    /// TODO: util.print([...])
    /// TODO: util.log(string)
    /// TODO: util.inspect(object, [options])
    Q_INVOKABLE bool isArray(const QJSValue &object) const;
    Q_INVOKABLE bool isRegExp(const QJSValue &object) const;
    Q_INVOKABLE bool isDate(const QJSValue &object) const;
    Q_INVOKABLE bool isError(const QJSValue &object) const;
    /// TODO: util.inherits(constructor, superConstructor)


signals:

public slots:

};

#endif // UTIL_H

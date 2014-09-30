#include "coremodule.h"

#include <QJSEngine>

CoreModule::CoreModule(QJSEngine *jsEngine, QObject *parent) :
    QObject(parent),
    m_jsEngine(jsEngine)
{

}

QJSValue CoreModule::createError(const QString &message, int code)
{
    QJSValue errorCtr = m_jsEngine->globalObject().property(QStringLiteral("Error"));
    QJSValue error = errorCtr.callAsConstructor({message});
    error.setProperty(QStringLiteral("code"), code);
    return error;
}

QJSEngine *CoreModule::jsEngine() const
{
    return m_jsEngine;
}

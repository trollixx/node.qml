#include "coremodule.h"

CoreModule::CoreModule(QJSEngine *jsEngine, QObject *parent) :
    QObject(parent),
    m_jsEngine(jsEngine)
{

}

QJSEngine *CoreModule::jsEngine() const
{
    return m_jsEngine;
}

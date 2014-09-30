#include "node.h"

#include "modules/dns.h"
#include "modules/filesystem.h"
#include "modules/path.h"
#include "modules/util.h"

#include <QJSEngine>

Node::Node(QQmlEngine *qmlEngine, QJSEngine *jsEngine, QObject *parent) :
    QObject(parent),
    m_jsEngine(jsEngine),
    m_qmlEngine(qmlEngine)
{
    setupCoreModules();
}

QJSValue Node::require(const QString &module)
{
    if (m_coreModules.contains(module))
        return m_jsEngine->newQObject(m_coreModules.value(module));
    else
        return m_jsEngine->newObject(); /// TODO: Implement real requre()
}

void Node::setupCoreModules()
{
    m_coreModules.insert(QStringLiteral("dns"), new Dns(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("fs"), new FileSystem(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("path"), new Path(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("util"), new Util(m_jsEngine, this));
}

#include "node.h"

#include "modules/dns.h"
#include "modules/filesystem.h"
#include "modules/os.h"
#include "modules/path.h"
#include "modules/process.h"
#include "modules/util.h"

#include <QJSEngine>
#include <QLoggingCategory>
#include <QTimerEvent>

namespace {
const QLoggingCategory logCategory("nodeqml.core");
}

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

QJSValue Node::setTimeout(QJSValue cb, int ms)
{
    if (!cb.isCallable()) {
        qCWarning(logCategory, "Invalid callback.");
        return QJSValue::NullValue;
    }

    if (ms <= 0)
        ms = 1;

    int timerId = startTimer(ms, Qt::PreciseTimer);
    if (!timerId) {
        qCWarning(logCategory, "Cannot set timer.");
        return QJSValue::NullValue;
    }
    m_timeoutCallbacks.insert(timerId, cb);
    /// TODO: Return an object similar to Node's
    return timerId;
}

void Node::clearTimeout(QJSValue t)
{
    int timerId = t.toInt();
    if (!m_timeoutCallbacks.contains(timerId))
        return;
    killTimer(timerId);
    m_timeoutCallbacks.remove(timerId);
}

QJSValue Node::setInterval(QJSValue cb, int ms)
{
    if (!cb.isCallable()) {
        qCWarning(logCategory, "Invalid callback.");
        return QJSValue::NullValue;
    }

    if (ms <= 0)
        ms = 1;

    int timerId = startTimer(ms, Qt::PreciseTimer);
    if (!timerId) {
        qCWarning(logCategory, "Cannot set timer.");
        return QJSValue::NullValue;
    }
    m_intervalCallbacks.insert(timerId, cb);
    /// TODO: Return an object similar to Node's
    return timerId;
}

void Node::clearInterval(QJSValue t)
{
    int timerId = t.toInt();
    if (!m_intervalCallbacks.contains(timerId))
        return;
    killTimer(timerId);
    m_intervalCallbacks.remove(timerId);
}

void Node::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();
    QJSValue cb;

    if (m_timeoutCallbacks.contains(timerId)) {
        killTimer(timerId);
        cb = m_timeoutCallbacks.take(timerId);
    } else if (m_intervalCallbacks.contains(timerId)) {
        cb = m_intervalCallbacks.value(timerId);
    } else {
        return;
    }

    event->accept();
    cb.call();
}

void Node::setupCoreModules()
{
    m_coreModules.insert(QStringLiteral("dns"), new Dns(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("fs"), new FileSystem(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("os"), new Os(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("path"), new Path(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("process"), new Process(m_jsEngine, this));
    m_coreModules.insert(QStringLiteral("util"), new Util(m_jsEngine, this));
}

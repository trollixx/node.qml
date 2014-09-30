#ifndef NODE_H
#define NODE_H

#include <QHash>
#include <QJSValue>
#include <QObject>

class QJSEngine;
class QQmlEngine;

class CoreModule;

class Node : public QObject
{
    Q_OBJECT
public:
    explicit Node(QQmlEngine *qmlEngine, QJSEngine *jsEngine, QObject *parent = 0);

    Q_INVOKABLE QJSValue require(const QString &module);

    /// Timers module
    /// TODO: Move to somewhere else and export globally
    Q_INVOKABLE QJSValue setTimeout(QJSValue cb, int ms);
    Q_INVOKABLE void clearTimeout(QJSValue t);

    Q_INVOKABLE QJSValue setInterval(QJSValue cb, int ms);
    Q_INVOKABLE void clearInterval(QJSValue t);

protected:
    void timerEvent(QTimerEvent *event);

private:
    void setupCoreModules();

    QJSEngine *m_jsEngine;
    QQmlEngine *m_qmlEngine;
    QHash<QString, CoreModule *> m_coreModules;
    QHash<int, QJSValue> m_timeoutCallbacks;
    QHash<int, QJSValue> m_intervalCallbacks;
};

#endif // NODE_H

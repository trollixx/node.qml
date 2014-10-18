#ifndef NODE_H
#define NODE_H

#include <QHash>
#include <QObject>

#include <private/qv4engine_p.h>

class QQmlEngine;

namespace NodeQml {

class Engine : public QObject
{
    Q_OBJECT
public:
    static Engine *get(QV4::ExecutionEngine *v4);

    explicit Engine(QQmlEngine *qmlEngine, QObject *parent = 0);
    ~Engine();

    QV4::ReturnedValue require(QV4::CallContext *ctx);

    QV4::ReturnedValue setTimeout(QV4::CallContext *ctx);
    QV4::ReturnedValue clearTimeout(QV4::CallContext *ctx);

    QV4::ReturnedValue setInterval(QV4::CallContext *ctx);
    QV4::ReturnedValue clearInterval(QV4::CallContext *ctx);

public:
    QV4::Value bufferCtor;
    QV4::InternalClass *bufferClass;

protected:
    void timerEvent(QTimerEvent *event) override;

private:
    void registerTypes();
    void setupCoreModules();

    QQmlEngine *m_qmlEngine;
    QV4::ExecutionEngine *m_v4;
    QHash<QString, QV4::Object *> m_coreModules;
    QHash<int, QV4::FunctionObject *> m_timeoutCallbacks;
    QHash<int, QV4::FunctionObject *> m_intervalCallbacks;

    static QHash<QV4::ExecutionEngine *, Engine*> m_nodeEngines;
};

}
#endif // NODE_H

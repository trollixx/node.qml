#ifndef ENGINE_P_H
#define ENGINE_P_H

#include <QHash>
#include <QObject>

#include <private/qv4engine_p.h>

class QQmlEngine;

namespace NodeQml {

class Engine;
struct ModuleObject;

class EnginePrivate : public QObject
{
    Q_OBJECT
public:
    static EnginePrivate *get(QV4::ExecutionEngine *v4);

    explicit EnginePrivate(QQmlEngine *qmlEngine, Engine *engine = 0);
    ~EnginePrivate();

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
    Engine * const q_ptr;
    Q_DECLARE_PUBLIC(Engine)

    void registerTypes();
    void registerModules();

    QQmlEngine *m_qmlEngine;
    QV4::ExecutionEngine *m_v4;
    QHash<QString, QV4::Object *> m_coreModules;
    QHash<QString, ModuleObject *> m_cachedModules;
    QHash<int, QV4::FunctionObject *> m_timeoutCallbacks;
    QHash<int, QV4::FunctionObject *> m_intervalCallbacks;

    static QHash<QV4::ExecutionEngine *, EnginePrivate*> m_nodeEngines;
};

}

#endif // ENGINE_P_H

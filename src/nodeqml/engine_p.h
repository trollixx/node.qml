#ifndef ENGINE_P_H
#define ENGINE_P_H

#include <QHash>
#include <QObject>

#include <private/qv4engine_p.h>
#include <private/qv4persistent_p.h>

class QJSEngine;

namespace NodeQml {

namespace Heap {
struct ModuleObject;
}

class Engine;
struct ModuleObject;

class EnginePrivate : public QObject
{
    Q_OBJECT
public:
    static EnginePrivate *get(QV4::ExecutionEngine *v4);

    explicit EnginePrivate(QJSEngine *jsEngine, Engine *engine = 0);
    ~EnginePrivate();

    void doneCheck();
    void exceptionCheck();

    bool hasNativeModule(const QString &id) const;
    QV4::Heap::Object *nativeModule(const QString &id) const;

    void cacheModule(const QString& id, Heap::ModuleObject *module);
    bool hasCachedModule(const QString &id) const;
    Heap::ModuleObject *cachedModule(const QString &id) const;

    QV4::ReturnedValue require(const QString &id);

    QV4::ReturnedValue setTimeout(QV4::CallContext *ctx);
    QV4::ReturnedValue clearTimeout(QV4::CallContext *ctx);

    QV4::ReturnedValue setInterval(QV4::CallContext *ctx);
    QV4::ReturnedValue clearInterval(QV4::CallContext *ctx);

    QV4::ReturnedValue nextTick(QV4::CallContext *ctx);

    QV4::ReturnedValue throwErrnoException(int errorNo, const QString &syscall);

public:
    QV4::Value bufferCtor;
    QV4::Value bufferPrototype;

    QV4::Value errnoExceptionPrototype;

protected:
    void customEvent(QEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

private:
    Engine * const q_ptr;
    Q_DECLARE_PUBLIC(Engine)

    void registerTypes();
    void registerModules();

    QV4::ExecutionEngine *m_v4;

    QHash<QString, QV4::PersistentValue> m_coreModules;
    QHash<QString, QV4::PersistentValue> m_cachedModules;

    QHash<int, QV4::PersistentValue> m_timeoutCallbacks;
    QHash<int, QV4::PersistentValue> m_intervalCallbacks;

    static QHash<QV4::ExecutionEngine *, EnginePrivate*> m_nodeEngines;
};

}

#endif // ENGINE_P_H

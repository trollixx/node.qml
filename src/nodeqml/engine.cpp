#include "engine.h"
#include "engine_p.h"

#include "globalextensions.h"
#include "moduleobject.h"
#include "modules/filesystem.h"
#include "modules/os.h"
#include "modules/path.h"
#include "modules/util.h"
#include "types/buffer.h"

#include <QFileInfo>
#include <QLoggingCategory>
#include <QQmlEngine>
#include <QTimerEvent>

#include <private/qjsvalue_p.h>
#include <private/qv4engine_p.h>
#include <private/qv8engine_p.h>

namespace {
const QLoggingCategory logCategory("nodeqml.core");
}

using namespace NodeQml;

Engine::Engine(QQmlEngine *qmlEngine, QObject *parent) :
    QObject(parent),
    d_ptr(new EnginePrivate(qmlEngine, this))
{

}

QJSValue Engine::require(const QString &id)
{
    Q_D(Engine);

    QV4::Scope scope(d->m_v4);
    QV4::ScopedCallData callData(scope, 1);
    callData->args[0] = d->m_v4->newString(id);
    callData->thisObject = d->m_v4->globalObject;

    QV4::ScopedString requireString(scope, d->m_v4->newString(QStringLiteral("require")));
    QV4::Scoped<QV4::FunctionObject> f(scope, d->m_v4->globalObject->get(requireString));

    //QV4::Scoped<QV4::ScriptFunction> f(scope, );

    /*QV4::CallContext *ctx
            = reinterpret_cast<QV4::CallContext *>(
                d->m_v4->currentContext()->newCallContext(f, callData));

    QV4::ExecutionContextSaver ctxSaver(d->m_v4->currentContext());*/
    //f->call(d->m_v4->globalObject, callData);

    //QV4::ScopedValue result(scope, d->require(ctx));
    QV4::ScopedValue result(scope, f->call(d->m_v4->globalObject, callData));
    return new QJSValuePrivate(d->m_v4, result);
}

QHash<QV4::ExecutionEngine *, EnginePrivate*> EnginePrivate::m_nodeEngines;

EnginePrivate *EnginePrivate::get(QV4::ExecutionEngine *v4)
{
    return m_nodeEngines.value(v4);
}

EnginePrivate::EnginePrivate(QQmlEngine *qmlEngine, Engine *engine) :
    QObject(engine),
    q_ptr(engine),
    m_qmlEngine(qmlEngine),
    m_v4(QV8Engine::getV4(qmlEngine))
{
    /// TODO: Mutex
    m_nodeEngines.insert(m_v4, this);

    NodeQml::GlobalExtensions::init(m_qmlEngine);
    registerTypes();
    /// TODO: Core modules should not be loaded unless required
    registerModules();
}

EnginePrivate::~EnginePrivate()
{
    m_nodeEngines.remove(m_v4);
}

bool EnginePrivate::hasNativeModule(const QString &id) const
{
    return m_coreModules.contains(id);
}

QV4::Object *EnginePrivate::nativeModule(const QString &id) const
{
    return m_coreModules.value(id);
}

void EnginePrivate::cacheModule(const QString &id, ModuleObject *module)
{
    Q_ASSERT(!m_cachedModules.contains(id));
    m_cachedModules.insert(id, module);
}

bool EnginePrivate::hasCachedModule(const QString &id) const
{
    return m_cachedModules.contains(id);
}

QV4::Object *EnginePrivate::cachedModule(const QString &id) const
{
    return m_cachedModules.value(id);
}

QV4::ReturnedValue EnginePrivate::require(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    if (!callData->argc || !callData->args[0].isString())
        return ctx->throwError(QStringLiteral("require: id must be a string"));

    const QString id = callData->args[0].toQStringNoThrow();
    return ModuleObject::require(ctx, id)->asReturnedValue();
}

QV4::ReturnedValue EnginePrivate::setTimeout(QV4::CallContext *ctx)
{
    if (ctx->d()->callData->argc < 2)
        return ctx->throwError("setTimeout: missing arguments");

    QV4::FunctionObject *cb = ctx->d()->callData->args[0].asFunctionObject();
    if (!cb)
        return ctx->throwTypeError("setTimeout: callback must be a function");

    if (!ctx->d()->callData->args[1].isInt32())
        return ctx->throwTypeError("setTimeout: timeout must be an integer");

    int delay = ctx->d()->callData->args[1].toInt32();
    if (delay <= 0)
        delay = 1;

    int timerId = startTimer(delay, Qt::PreciseTimer);
    if (!timerId)
        return ctx->throwError("setTimeout: cannot start timer");

    m_timeoutCallbacks.insert(timerId, cb);

    /// TODO: Return an object similar to Node's
    return QV4::Encode(timerId);
}

QV4::ReturnedValue EnginePrivate::clearTimeout(QV4::CallContext *ctx)
{
    if (ctx->d()->callData->argc < 1)
        return ctx->throwError("clearTimeout: missing arguments");

    if (!ctx->d()->callData->args[0].isInt32())
        return ctx->throwTypeError("clearTimeout: timeout must be an integer (at the moment)");

    int timerId = ctx->d()->callData->args[0].toInt32();
    if (m_timeoutCallbacks.contains(timerId)) {
        killTimer(timerId);
        m_timeoutCallbacks.remove(timerId);
    }

    return QV4::Encode::undefined();
}

QV4::ReturnedValue EnginePrivate::setInterval(QV4::CallContext *ctx)
{
    if (ctx->d()->callData->argc < 2)
        return ctx->throwError("setInterval: missing arguments");

    QV4::FunctionObject *cb = ctx->d()->callData->args[0].asFunctionObject();
    if (!cb)
        return ctx->throwTypeError("setInterval: callback must be a function");

    if (!ctx->d()->callData->args[1].isInt32())
        return ctx->throwTypeError("setInterval: timeout must be an integer");

    int delay = ctx->d()->callData->args[1].toInt32();
    if (delay <= 0)
        delay = 1;

    int timerId = startTimer(delay, Qt::PreciseTimer);
    if (!timerId)
        return ctx->throwError("setInterval: cannot start timer");

    m_intervalCallbacks.insert(timerId, cb);

    /// TODO: Return an object similar to Node's
    return QV4::Encode(timerId);
}

QV4::ReturnedValue EnginePrivate::clearInterval(QV4::CallContext *ctx)
{
    if (ctx->d()->callData->argc < 1)
        return ctx->throwError("clearInterval: missing arguments");

    if (!ctx->d()->callData->args[0].isInt32())
        return ctx->throwTypeError("clearInterval: timeout must be an integer (at the moment)");

    int timerId = ctx->d()->callData->args[0].toInt32();
    if (!m_intervalCallbacks.contains(timerId)) {
        killTimer(timerId);
        m_intervalCallbacks.remove(timerId);
    }

    return QV4::Encode::undefined();
}

void EnginePrivate::timerEvent(QTimerEvent *event)
{
    int timerId = event->timerId();
    QV4::FunctionObject *cb;

    if (m_timeoutCallbacks.contains(timerId)) {
        killTimer(timerId);
        cb = m_timeoutCallbacks.take(timerId);
    } else if (m_intervalCallbacks.contains(timerId)) {
        cb = m_intervalCallbacks.value(timerId);
    } else {
        return;
    }

    event->accept();

    QV4::Scope scope(m_v4);
    QV4::ScopedCallData callData(scope, 0);
    callData->thisObject = m_v4->globalObject->asReturnedValue();
    cb->call(callData);
}

void EnginePrivate::registerTypes()
{
    QV4::Scope scope(m_v4);

    QV4::ScopedObject bufferPrototype(
                scope, m_v4->memoryManager->alloc<BufferPrototype>(
                    QV4::InternalClass::create(m_v4, BufferPrototype::staticVTable(),
                                               m_v4->objectClass->prototype)));
    bufferClass = QV4::InternalClass::create(m_v4, BufferObject::staticVTable(), bufferPrototype);
    bufferCtor = m_v4->memoryManager->alloc<BufferCtor>(m_v4->rootContext);
    static_cast<BufferPrototype *>(bufferPrototype.getPointer())->init(m_v4, bufferCtor.asObject());
    m_v4->globalObject->defineDefaultProperty(QStringLiteral("Buffer"), bufferCtor);
    m_v4->globalObject->defineDefaultProperty(QStringLiteral("SlowBuffer"), bufferCtor);
}

void EnginePrivate::registerModules()
{
    m_coreModules.insert(QStringLiteral("fs"), m_v4->memoryManager->alloc<FileSystemModule>(m_v4));
    m_coreModules.insert(QStringLiteral("os"), m_v4->memoryManager->alloc<OsModule>(m_v4));
    m_coreModules.insert(QStringLiteral("path"), m_v4->memoryManager->alloc<PathModule>(m_v4));
    m_coreModules.insert(QStringLiteral("util"), m_v4->memoryManager->alloc<UtilModule>(m_v4));
}

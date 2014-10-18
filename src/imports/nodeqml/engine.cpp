#include "engine.h"

#include "globalextensions.h"
#include "modules/dns.h"
#include "modules/filesystem.h"
#include "modules/os.h"
#include "modules/path.h"
#include "modules/process.h"
#include "modules/util.h"
#include "types/buffer.h"

#include <QLoggingCategory>
#include <QTimerEvent>
#include <QQmlEngine>

#include <private/qv4engine_p.h>
#include <private/qv8engine_p.h>

namespace {
const QLoggingCategory logCategory("nodeqml.core");
}

using namespace NodeQml;

QHash<QV4::ExecutionEngine *, Engine*> Engine::m_nodeEngines;

Engine *Engine::get(QV4::ExecutionEngine *v4)
{
    if (!m_nodeEngines.contains(v4))
        return nullptr;

    return m_nodeEngines.value(v4);
}

Engine::Engine(QQmlEngine *qmlEngine, QObject *parent) :
    QObject(parent),
    m_qmlEngine(qmlEngine),
    m_v4(QV8Engine::getV4(qmlEngine))
{
    /// TODO: Mutex
    m_nodeEngines.insert(m_v4, this);

    NodeQml::GlobalExtensions::init(m_qmlEngine);
    registerTypes();
    registerModules();
}

Engine::~Engine()
{
    m_nodeEngines.remove(m_v4);
}

QV4::ReturnedValue Engine::require(QV4::CallContext *ctx)
{
    const QString id = ctx->d()->callData->args[0].toQStringNoThrow();

    if (!m_coreModules.contains(id))
        return ctx->throwError(QString("Cannot find module '%1'").arg(id));

    QV4::Scope scope(m_v4);
    return QV4::ScopedObject(scope, m_coreModules.value(id)).asReturnedValue();
}

QV4::ReturnedValue Engine::setTimeout(QV4::CallContext *ctx)
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
        return ctx->throwError("setInterval: cannot start timer");

    m_timeoutCallbacks.insert(timerId, cb);

    /// TODO: Return an object similar to Node's
    return QV4::Encode(timerId);
}

QV4::ReturnedValue Engine::clearTimeout(QV4::CallContext *ctx)
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

QV4::ReturnedValue Engine::setInterval(QV4::CallContext *ctx)
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

QV4::ReturnedValue Engine::clearInterval(QV4::CallContext *ctx)
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

void Engine::timerEvent(QTimerEvent *event)
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

void Engine::registerTypes()
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
}

void Engine::registerModules()
{
    m_coreModules.insert(QStringLiteral("util"), m_v4->memoryManager->alloc<UtilModule>(m_v4));
    m_coreModules.insert(QStringLiteral("fs"), m_v4->memoryManager->alloc<FileSystemModule>(m_v4));
}

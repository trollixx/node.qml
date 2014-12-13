#include "globalextensions.h"

#include "engine_p.h"
#include "modules/process.h"
#include "modules/console.h"

#include <QQmlEngine>

#include <private/qv8engine_p.h>

using namespace NodeQml;

void GlobalExtensions::init(QQmlEngine *qmlEngine)
{
    QV4::ExecutionEngine *v4 = QV8Engine::getV4(qmlEngine);
    QV4::Object *globalObject = v4->globalObject();

    globalObject->defineDefaultProperty(QStringLiteral("require"), method_require);

    globalObject->defineDefaultProperty(QStringLiteral("setTimeout"), method_setTimeout);
    globalObject->defineDefaultProperty(QStringLiteral("clearTimeout"), method_clearTimeout);

    globalObject->defineDefaultProperty(QStringLiteral("setInterval"), method_setInterval);
    globalObject->defineDefaultProperty(QStringLiteral("clearInterval"), method_clearInterval);

    QV4::Scope scope(v4);
    QV4::ScopedObject process(scope, v4->memoryManager->alloc<ProcessModule>(v4));
    globalObject->defineDefaultProperty(QStringLiteral("process"), process);

    QV4::ScopedObject console(scope, v4->memoryManager->alloc<ConsoleModule>(v4));
    globalObject->defineDefaultProperty(QStringLiteral("console"), console);

    // Error object modification
    // See: https://code.google.com/p/v8/wiki/JavaScriptStackTraceApi
    // TODO: Replace with a custom Error object?
    QV4::ScopedString s(scope, v4->newString(QStringLiteral("Error")));
    QV4::ScopedObject errorObject(scope, globalObject->get(s));
    errorObject->defineDefaultProperty(QStringLiteral("captureStackTrace"), method_captureStackTrace);
    errorObject->defineDefaultProperty(QStringLiteral("stackTraceLimit"), QV4::Primitive::fromInt32(10));
}

QV4::ReturnedValue GlobalExtensions::method_require(QV4::CallContext *ctx)
{
    QV4::ExecutionEngine *v4 = ctx->engine();
    NODE_CTX_CALLDATA(ctx);

    if (!callData->argc)
        return v4->throwError("require() requires an argument");
    if (!callData->args[0].isString())
        return v4->throwTypeError("require(): argument (id) must be a string");

    const QString id = callData->args[0].toQStringNoThrow();
    return EnginePrivate::get(ctx->engine())->require(id, ctx);
}

QV4::ReturnedValue GlobalExtensions::method_setTimeout(QV4::CallContext *ctx)
{
    return EnginePrivate::get(ctx->engine())->setTimeout(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_clearTimeout(QV4::CallContext *ctx)
{
    return EnginePrivate::get(ctx->engine())->clearTimeout(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_setInterval(QV4::CallContext *ctx)
{
    return EnginePrivate::get(ctx->engine())->setInterval(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_clearInterval(QV4::CallContext *ctx)
{
    return EnginePrivate::get(ctx->engine())->clearInterval(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_captureStackTrace(QV4::CallContext *ctx)
{
    qWarning("captureStackTrace() is not implemented!");
    return QV4::Encode::undefined();
}

#include "globalextensions.h"

#include "engine.h"

#include <QQmlEngine>

#include <private/qv8engine_p.h>

using namespace NodeQml;

void GlobalExtensions::init(QQmlEngine *qmlEngine)
{
    QV4::ExecutionEngine *v4 = QV8Engine::getV4(qmlEngine);
    QV4::Object *globalObject = v4->globalObject;

    globalObject->defineDefaultProperty(QStringLiteral("require"), method_require);

    globalObject->defineDefaultProperty(QStringLiteral("setTimeout"), method_setTimeout);
    globalObject->defineDefaultProperty(QStringLiteral("clearTimeout"), method_clearTimeout);

    globalObject->defineDefaultProperty(QStringLiteral("setInterval"), method_setInterval);
    globalObject->defineDefaultProperty(QStringLiteral("clearInterval"), method_clearInterval);
}

QV4::ReturnedValue GlobalExtensions::method_require(QV4::CallContext *ctx)
{
    if (ctx->d()->callData->argc < 1)
        return ctx->throwError("require() requires an argument");
    if (!ctx->d()->callData->args[0].isString())
        return ctx->throwTypeError("require(): argument (id) must be a string");

    return Engine::get(ctx->engine())->require(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_setTimeout(QV4::CallContext *ctx)
{
    return Engine::get(ctx->engine())->setTimeout(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_clearTimeout(QV4::CallContext *ctx)
{
    return Engine::get(ctx->engine())->clearTimeout(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_setInterval(QV4::CallContext *ctx)
{
    return Engine::get(ctx->engine())->setInterval(ctx);
}

QV4::ReturnedValue GlobalExtensions::method_clearInterval(QV4::CallContext *ctx)
{
    return Engine::get(ctx->engine())->clearInterval(ctx);
}

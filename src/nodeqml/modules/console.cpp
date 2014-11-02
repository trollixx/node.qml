#include "console.h"

#include "util.h"

#include <QDateTime>
#include <QTextStream>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(ConsoleModule);

ConsoleModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    setVTable(staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);

    o->defineDefaultProperty(QStringLiteral("log"), method_log);
    o->defineDefaultProperty(QStringLiteral("info"), method_info);
    o->defineDefaultProperty(QStringLiteral("error"), method_error);
    o->defineDefaultProperty(QStringLiteral("warn"), method_warn);
    o->defineDefaultProperty(QStringLiteral("dir"), method_dir);
    o->defineDefaultProperty(QStringLiteral("time"), method_time);
    o->defineDefaultProperty(QStringLiteral("timeEnd"), method_timeEnd);
    o->defineDefaultProperty(QStringLiteral("trace"), method_trace);
    o->defineDefaultProperty(QStringLiteral("assert"), method_assert);
}

QV4::ReturnedValue ConsoleModule::method_log(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.log()"));
}

QV4::ReturnedValue ConsoleModule::method_info(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.info()"));
}

QV4::ReturnedValue ConsoleModule::method_error(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.error()"));
}

QV4::ReturnedValue ConsoleModule::method_warn(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.warn()"));
}

QV4::ReturnedValue ConsoleModule::method_dir(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.dir()"));
}

QV4::ReturnedValue ConsoleModule::method_time(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    QV4::Scope scope(ctx);
    QV4::Scoped<ConsoleModule> self(scope, callData->thisObject.as<ConsoleModule>());

    QV4::ScopedValue key(scope, callData->args[0]);
    self->d()->timeMarks.insert(key.asReturnedValue(), QDateTime::currentMSecsSinceEpoch());

    return QV4::Encode::undefined();
}

QV4::ReturnedValue ConsoleModule::method_timeEnd(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    QV4::Scope scope(ctx);
    QV4::Scoped<ConsoleModule> self(scope, callData->thisObject.as<ConsoleModule>());

    QV4::ScopedValue key(scope, callData->args[0]);

    if (!self->d()->timeMarks.contains(key.asReturnedValue())) {
        qDebug("No such label: %s", qPrintable(key->toQStringNoThrow()));
        return ctx->throwError(QString("No such label: %1").arg(key->toQStringNoThrow()));
    }

    const qint64 delta
            = QDateTime::currentMSecsSinceEpoch() - self->d()->timeMarks[key.asReturnedValue()];
    /// TODO: Use QTextStream
    qDebug("%s: %lldms", qPrintable(callData->args[0].toQStringNoThrow()), delta);

    return QV4::Encode::undefined();
}

QV4::ReturnedValue ConsoleModule::method_trace(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.trace()"));
}

QV4::ReturnedValue ConsoleModule::method_assert(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.assert()"));
}

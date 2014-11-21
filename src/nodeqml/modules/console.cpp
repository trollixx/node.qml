#include "console.h"

#include "util.h"

#include <QDateTime>
#include <QTextStream>

#include <private/qv4global_p.h>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(ConsoleModule);

ConsoleModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    setVTable(staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject self(scope, this);

    self->defineDefaultProperty(QStringLiteral("log"), method_log);
    self->defineDefaultProperty(QStringLiteral("info"), method_log);
    self->defineDefaultProperty(QStringLiteral("error"), method_error);
    self->defineDefaultProperty(QStringLiteral("warn"), method_error);
    self->defineDefaultProperty(QStringLiteral("dir"), method_dir);
    self->defineDefaultProperty(QStringLiteral("time"), method_time);
    self->defineDefaultProperty(QStringLiteral("timeEnd"), method_timeEnd);
    self->defineDefaultProperty(QStringLiteral("trace"), method_trace);
    self->defineDefaultProperty(QStringLiteral("assert"), method_assert);
}

QV4::ReturnedValue ConsoleModule::method_log(QV4::CallContext *ctx)
{
    QV4::Scope scope(ctx);
    QV4::ScopedString s(scope, UtilModule::method_format(ctx));

    QTextStream(stdout) << s->toQString() << endl;

    return QV4::Encode::undefined();
}

QV4::ReturnedValue ConsoleModule::method_error(QV4::CallContext *ctx)
{
    QV4::Scope scope(ctx);
    QV4::ScopedString s(scope, UtilModule::method_format(ctx));

    QTextStream(stderr) << s->toQString() << endl;

    return QV4::Encode::undefined();
}

QV4::ReturnedValue ConsoleModule::method_dir(QV4::CallContext *ctx)
{
    return ctx->engine()->throwUnimplemented(QStringLiteral("console.dir()"));
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
        return ctx->engine()->throwError(QString("No such label: %1").arg(key->toQStringNoThrow()));
    }

    const qint64 delta
            = QDateTime::currentMSecsSinceEpoch() - self->d()->timeMarks[key.asReturnedValue()];
    /// TODO: Use QTextStream
    qDebug("%s: %lldms", qPrintable(callData->args[0].toQStringNoThrow()), delta);

    return QV4::Encode::undefined();
}

QV4::ReturnedValue ConsoleModule::method_trace(QV4::CallContext *ctx)
{
    return ctx->engine()->throwUnimplemented(QStringLiteral("console.trace()"));
}

QV4::ReturnedValue ConsoleModule::method_assert(QV4::CallContext *ctx)
{
    return ctx->engine()->throwUnimplemented(QStringLiteral("console.assert()"));
}

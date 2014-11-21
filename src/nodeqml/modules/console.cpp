#include "console.h"

#include "util.h"

#include <QDateTime>
#include <QTextStream>

#include <private/qv4context_p.h>
#include <private/qv4global_p.h>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(ConsoleModule);

Heap::ConsoleModule::ConsoleModule(QV4::ExecutionEngine *v4) :
    QV4::Heap::Object(v4)
{
    setVTable(NodeQml::ConsoleModule::staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject self(scope, this);

    self->defineDefaultProperty(QStringLiteral("log"), NodeQml::ConsoleModule::method_log);
    self->defineDefaultProperty(QStringLiteral("info"), NodeQml::ConsoleModule::method_log);
    self->defineDefaultProperty(QStringLiteral("error"), NodeQml::ConsoleModule::method_error);
    self->defineDefaultProperty(QStringLiteral("warn"), NodeQml::ConsoleModule::method_error);
    self->defineDefaultProperty(QStringLiteral("dir"), NodeQml::ConsoleModule::method_dir);
    self->defineDefaultProperty(QStringLiteral("time"), NodeQml::ConsoleModule::method_time);
    self->defineDefaultProperty(QStringLiteral("timeEnd"), NodeQml::ConsoleModule::method_timeEnd);
    self->defineDefaultProperty(QStringLiteral("trace"), NodeQml::ConsoleModule::method_trace);
    self->defineDefaultProperty(QStringLiteral("assert"), NodeQml::ConsoleModule::method_assert);
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

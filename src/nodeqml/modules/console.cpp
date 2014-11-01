#include "console.h"

#include "util.h"

#include <QTextStream>

using namespace NodeQml;

ConsoleModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
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
    return ctx->throwUnimplemented(QStringLiteral("console.time()"));
}

QV4::ReturnedValue ConsoleModule::method_timeEnd(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.timeEnd()"));
}

QV4::ReturnedValue ConsoleModule::method_trace(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.trace()"));
}

QV4::ReturnedValue ConsoleModule::method_assert(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("console.assert()"));
}

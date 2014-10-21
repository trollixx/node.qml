#include "util.h"

#include <QTextStream>

//#include <private/qv4regexpobject_p.h>

using namespace NodeQml;

UtilModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);

    o->defineDefaultProperty(QStringLiteral("format"), method_format);
    o->defineDefaultProperty(QStringLiteral("debug"), method_debug);
    o->defineDefaultProperty(QStringLiteral("error"), method_error);
    o->defineDefaultProperty(QStringLiteral("puts"), method_puts);
    o->defineDefaultProperty(QStringLiteral("print"), method_print);
    o->defineDefaultProperty(QStringLiteral("log"), method_log);
    o->defineDefaultProperty(QStringLiteral("inspect"), method_inspect);
    o->defineDefaultProperty(QStringLiteral("isArray"), method_isArray);
    o->defineDefaultProperty(QStringLiteral("isRegExp"), method_isRegExp);
    o->defineDefaultProperty(QStringLiteral("isDate"), method_isDate);
    o->defineDefaultProperty(QStringLiteral("isError"), method_isError);
    o->defineDefaultProperty(QStringLiteral("pump"), method_pump);
    o->defineDefaultProperty(QStringLiteral("inherits"), method_inherits);
}

/// TODO: util.format(format, [...])
QV4::ReturnedValue UtilModule::method_format(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("util.format()"));
}

QV4::ReturnedValue UtilModule::method_debug(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (callData->argc < 1)
        ctx->throwError("require() requires an argument");
    if (!callData->args[0].isString())
        ctx->throwTypeError("require(): argument must be a string");

    QTextStream stream(stderr);
    stream << callData->args[0].toQStringNoThrow();

    return QV4::Encode::undefined();
}

/// TODO: util.error([...])
QV4::ReturnedValue UtilModule::method_error(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("util.error()"));
}

/// TODO: util.puts([...])
QV4::ReturnedValue UtilModule::method_puts(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("util.puts()"));
}

/// TODO: util.print([...])
QV4::ReturnedValue UtilModule::method_print(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("util.print()"));
}

/// TODO: util.log(string)
QV4::ReturnedValue UtilModule::method_log(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("util.log()"));
}

/// TODO: util.inspect(object, [options])
QV4::ReturnedValue UtilModule::method_inspect(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("util.inspect()"));
}

QV4::ReturnedValue UtilModule::method_isArray(QV4::CallContext *ctx)
{
    bool isArray = ctx->d()->callData->argc && ctx->d()->callData->args[0].asArrayObject();
    return QV4::Encode(isArray);
}

QV4::ReturnedValue UtilModule::method_isRegExp(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("util.isRegExp()"));
    //bool isRegExp = ctx->d()->callData->argc && ctx->d()->callData->args[0].as<QV4::RegExpObject>();
    //return QV4::Encode(isRegExp);
}

QV4::ReturnedValue UtilModule::method_isDate(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    bool isDate = callData->argc && callData->args[0].asDateObject();
    return QV4::Encode(isDate);
}

QV4::ReturnedValue UtilModule::method_isError(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    bool isError = callData->argc && callData->args[0].asErrorObject();
    return QV4::Encode(isError);
}

QV4::ReturnedValue UtilModule::method_pump(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(
                QStringLiteral("util.pump: Deprecated. Use readableStream.pipe(writableStream)."));
}

QV4::ReturnedValue UtilModule::method_inherits(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    if (callData->argc < 2)
        return ctx->throwError(QStringLiteral("inherits: two arguments are required"));

    if (!callData->args[0].isObject())
        return ctx->throwTypeError(QStringLiteral("inherits: constructor must be an object"));
    if (!callData->args[1].isObject())
        return ctx->throwTypeError(QStringLiteral("inherits: super constructor must be an object"));

    QV4::ExecutionEngine *v4 = ctx->engine();

    QV4::Scope scope(v4);
    QV4::ScopedObject ctor(scope, callData->args[0].asObject());
    QV4::ScopedObject superCtor(scope, callData->args[1].asObject());

    ctor->defineDefaultProperty(QStringLiteral("super_"), superCtor);

    QV4::ScopedObject prototype(scope, v4->newObject());
    prototype->setPrototype(superCtor->prototype());

    QV4::ScopedObject constructorProperty(scope, v4->newObject());
    constructorProperty->defineDefaultProperty(QStringLiteral("value"), ctor);
    constructorProperty->defineDefaultProperty(QStringLiteral("enumerable"),
                                               QV4::Primitive::fromBoolean(false));
    constructorProperty->defineDefaultProperty(QStringLiteral("writable"),
                                               QV4::Primitive::fromBoolean(true));
    constructorProperty->defineDefaultProperty(QStringLiteral("configurable"),
                                               QV4::Primitive::fromBoolean(true));

    prototype->defineDefaultProperty(QStringLiteral("constructor"), constructorProperty);
    ctor->setPrototype(prototype);

    return QV4::Encode::undefined();
}

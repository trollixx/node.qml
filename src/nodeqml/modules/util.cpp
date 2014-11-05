#include "util.h"

#include <QDateTime>
#include <QTextStream>
#include <QRegularExpression>

#include <private/qv4regexpobject_p.h>
#include <private/qv4jsonobject_p.h>

using namespace NodeQml;

UtilModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);

    o->defineDefaultProperty(QStringLiteral("format"), method_format, 1);
    o->defineDefaultProperty(QStringLiteral("log"), method_log, 1);
    o->defineDefaultProperty(QStringLiteral("inspect"), method_inspect, 2);
    o->defineDefaultProperty(QStringLiteral("isArray"), method_isArray, 1);
    o->defineDefaultProperty(QStringLiteral("isRegExp"), method_isRegExp, 1);
    o->defineDefaultProperty(QStringLiteral("isDate"), method_isDate, 1);
    o->defineDefaultProperty(QStringLiteral("isError"), method_isError, 1);
    o->defineDefaultProperty(QStringLiteral("inherits"), method_inherits, 2);
}

QV4::ReturnedValue UtilModule::method_format(QV4::CallContext *ctx)
{
    QV4::ExecutionEngine *v4 = ctx->engine();
    const QV4::CallData * const callData = ctx->d()->callData;

    QV4::Scope scope(ctx);
    QV4::ScopedString s(scope);

    // TODO: Call inspect anyway
    if (!callData->argc)
        return (s = v4->newString(QString()))->asReturnedValue();

    if (!callData->args[0].isString()) {
        QStringList objects;
        for (int i = 0; i < callData->argc; ++i)
            objects << inspect(callData->args[i]);
        return (s = v4->newString(objects.join(QStringLiteral(" "))))->asReturnedValue();
    }

    const QString format = callData->args[0].toQStringNoThrow();

    QString result;
    int i = 1;
    int lastPos = 0;

    QRegularExpression re(QStringLiteral("%[sdj%]"));
    QRegularExpressionMatchIterator it = re.globalMatch(format);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();

        /// TODO: That might be slow
        if (match.capturedStart(0))
            result += format.mid(lastPos, match.capturedStart(0) - lastPos);
        lastPos = match.capturedEnd(0);

        const QString placeholder = match.captured(0);

        if (i >= callData->argc) {
            result += placeholder;
        } else if (placeholder == QStringLiteral("%%")) {
            result += QStringLiteral("%");
        } else if (placeholder == QStringLiteral("%s") || placeholder == QStringLiteral("%d")) {
            result += inspect(callData->args[i++]);
        } else if (placeholder == QStringLiteral("%j")) {
            /// TODO: Call JSON.stringify(). See: Stringify::Str() in qv4jsonobject.cpp.
            result += QStringLiteral("[JSON]");
            ++i;
        }
    }

    result += format.mid(lastPos);

    for (; i < callData->argc; ++i)
        result += QStringLiteral(" ") + inspect(callData->args[i]);

    return (s = v4->newString(result))->asReturnedValue();
}

QV4::ReturnedValue UtilModule::method_log(QV4::CallContext *ctx)
{
    const QString label = QDateTime::currentDateTime().toString(QStringLiteral("d MMM HH:mm:ss"));
    QV4::Scope scope(ctx);
    QV4::ScopedValue v(scope, method_format(ctx));
    QTextStream(stdout) << label << QStringLiteral(" - ") << v->toQStringNoThrow() << endl;
    return QV4::Encode::undefined();
}

QV4::ReturnedValue UtilModule::method_inspect(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    QV4::Scope scope(ctx);
    QV4::ScopedString s(scope);
    return (s = ctx->engine()->newString(inspect(callData->args[0])))->asReturnedValue();
}

QV4::ReturnedValue UtilModule::method_isArray(QV4::CallContext *ctx)
{
    bool isArray = ctx->d()->callData->argc && ctx->d()->callData->args[0].asArrayObject();
    return QV4::Encode(isArray);
}

QV4::ReturnedValue UtilModule::method_isRegExp(QV4::CallContext *ctx)
{
    bool isRegExp = ctx->d()->callData->argc && ctx->d()->callData->args[0].as<QV4::RegExpObject>();
    return QV4::Encode(isRegExp);
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

QV4::ReturnedValue UtilModule::method_isUndefined(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    bool isUndefined = callData->argc && callData->args[0].isUndefined();
    return QV4::Encode(isUndefined);
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

/// TODO: Real implementation (See: https://github.com/joyent/node/blob/master/lib/util.js)
QString UtilModule::inspect(QV4::Value value)
{
    if (value.isNullOrUndefined())
        return QStringLiteral("undefined");

    return value.toQStringNoThrow();
}

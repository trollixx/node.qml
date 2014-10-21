#include "path.h"

#include <QDir>
#include <QFileInfo>

using namespace NodeQml;

PathModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    QV4::ScopedString s(scope);

#ifdef Q_OS_WIN
    o->defineReadonlyProperty(QStringLiteral("delimiter"),
                              (s = v4->newString(QStringLiteral(";"))));
#else
    o->defineReadonlyProperty(QStringLiteral("delimiter"),
                              (s = v4->newString(QStringLiteral(":"))));
#endif
    o->defineReadonlyProperty(QStringLiteral("sep"), (s = v4->newString(QDir::separator())));


    o->defineDefaultProperty(QStringLiteral("normalize"), method_normalize);
    o->defineDefaultProperty(QStringLiteral("dirname"), method_dirname);
    o->defineDefaultProperty(QStringLiteral("extname"), method_extname);
}

QV4::ReturnedValue PathModule::method_normalize(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc || !callData->args[0].isString())
        ctx->throwTypeError(QStringLiteral("path.normalize: argument must be a string"));

    return ctx->engine()->newString(
                QDir::cleanPath(callData->args[0].toQStringNoThrow()))->asReturnedValue();
}

/// TODO: path.join([path1], [path2], [...])
QV4::ReturnedValue PathModule::method_join(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("path.join()"));
}

/// TODO: path.resolve([from ...], to)
QV4::ReturnedValue PathModule::method_resolve(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("path.resolve()"));
}

/// TODO: path.relative(from, to)
QV4::ReturnedValue PathModule::method_relative(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("path.relative()"));
}

QV4::ReturnedValue PathModule::method_dirname(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc || !callData->args[0].isString())
        ctx->throwTypeError(QStringLiteral("path.dirname: argument must be a string"));

    return ctx->engine()->newString(
                QFileInfo(callData->args[0].toQStringNoThrow()).absolutePath())->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_basename(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc || !callData->args[0].isString())
        ctx->throwTypeError(QStringLiteral("path.basename: argument must be a string"));

    const QString path = callData->args[0].toQStringNoThrow();
    const QString fileName = QFileInfo(path).fileName();
    const QString ext = callData->argc > 1 ? callData->args[1].toQStringNoThrow() : QString();
    QString basename;
    if (!ext.isEmpty() && path.endsWith(ext))
        basename = fileName.left(fileName.size() - ext.size());
    else
        basename = fileName;

    return ctx->engine()->newString(basename)->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_extname(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc || !callData->args[0].isString())
        ctx->throwTypeError(QStringLiteral("path.extname: argument must be a string"));

    return ctx->engine()->newString(
                QFileInfo(callData->args[0].toQStringNoThrow()).completeSuffix())->asReturnedValue();
}

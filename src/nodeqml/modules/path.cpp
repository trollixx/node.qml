#include "path.h"

#include <QDir>
#include <QFileInfo>

#include <private/qv4context_p.h>

using namespace NodeQml;

Heap::PathModule::PathModule(QV4::ExecutionEngine *v4) :
    QV4::Heap::Object(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject self(scope, this);
    QV4::ScopedString s(scope);

#ifdef Q_OS_WIN
    self->defineReadonlyProperty(QStringLiteral("delimiter"),
                                (s = v4->newString(QStringLiteral(";"))));
#else
    self->defineReadonlyProperty(QStringLiteral("delimiter"),
                              (s = v4->newString(QStringLiteral(":"))));
#endif
    self->defineReadonlyProperty(QStringLiteral("sep"), (s = v4->newString(QDir::separator())));


    self->defineDefaultProperty(QStringLiteral("normalize"), NodeQml::PathModule::method_normalize);
    self->defineDefaultProperty(QStringLiteral("dirname"), NodeQml::PathModule::method_dirname);
    self->defineDefaultProperty(QStringLiteral("extname"), NodeQml::PathModule::method_extname);
}

QV4::ReturnedValue PathModule::method_normalize(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc || !callData->args[0].isString())
        ctx->engine()->throwTypeError(QStringLiteral("path.normalize: argument must be a string"));

    return ctx->engine()->newString(
                QDir::cleanPath(callData->args[0].toQStringNoThrow()))->asReturnedValue();
}

/// TODO: path.join([path1], [path2], [...])
QV4::ReturnedValue PathModule::method_join(QV4::CallContext *ctx)
{
    return ctx->engine()->throwUnimplemented(QStringLiteral("path.join()"));
}

/// TODO: path.resolve([from ...], to)
QV4::ReturnedValue PathModule::method_resolve(QV4::CallContext *ctx)
{
    return ctx->engine()->throwUnimplemented(QStringLiteral("path.resolve()"));
}

/// TODO: path.relative(from, to)
QV4::ReturnedValue PathModule::method_relative(QV4::CallContext *ctx)
{
    return ctx->engine()->throwUnimplemented(QStringLiteral("path.relative()"));
}

QV4::ReturnedValue PathModule::method_dirname(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc || !callData->args[0].isString())
        return ctx->engine()->throwTypeError(QStringLiteral("path.dirname: argument must be a string"));

    return ctx->engine()->newString(
                QFileInfo(callData->args[0].toQStringNoThrow()).absolutePath())->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_basename(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc || !callData->args[0].isString())
        return ctx->engine()->throwTypeError(QStringLiteral("path.basename: argument must be a string"));

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
        return ctx->engine()->throwTypeError(QStringLiteral("path.extname: argument must be a string"));

    return ctx->engine()->newString(
                QFileInfo(callData->args[0].toQStringNoThrow()).completeSuffix())->asReturnedValue();
}

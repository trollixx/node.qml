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
    self->defineDefaultProperty(QStringLiteral("join"), NodeQml::PathModule::method_join);
    self->defineDefaultProperty(QStringLiteral("resolve"), NodeQml::PathModule::method_resolve);
    self->defineDefaultProperty(QStringLiteral("relative"), NodeQml::PathModule::method_relative);
    self->defineDefaultProperty(QStringLiteral("dirname"), NodeQml::PathModule::method_dirname);
    self->defineDefaultProperty(QStringLiteral("basename"), NodeQml::PathModule::method_basename);
    self->defineDefaultProperty(QStringLiteral("extname"), NodeQml::PathModule::method_extname);
}

QV4::ReturnedValue PathModule::method_normalize(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);
    if (!callData->argc || !callData->args[0].isString())
        v4->throwTypeError(QStringLiteral("path.normalize: argument must be a string"));

    return v4->newString(QDir::cleanPath(callData->args[0].toQStringNoThrow()))->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_join(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);
    QStringList parts;
    for (int i = 0; i < callData->argc; ++i) {
        if (!callData->args[i].isString())
            return v4->throwTypeError(QStringLiteral("Arguments to path.join must be strings"));

        parts.append(callData->args[i].toQString());
    }
    return v4->newString(QDir::cleanPath(parts.join(QLatin1Char('/'))))->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_resolve(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    QStringList parts;
    for (int i = callData->argc - 1; i >= -1; --i) {
        if (i >= 0) {
            if (!callData->args[i].isString())
                return v4->throwTypeError(QStringLiteral("Arguments to path.resolve must be strings"));
            parts.prepend(callData->args[i].toQString());
        } else {
            parts.prepend(QDir::currentPath());
        }
        if (parts.first().startsWith(QLatin1Char('/')))
            break;
    }

    return v4->newString(QDir::cleanPath(parts.join(QLatin1Char('/'))))->asReturnedValue();
}

// path.relative(from, to)
QV4::ReturnedValue PathModule::method_relative(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    if (callData->argc < 2 || !callData->args[0].isString() || !callData->args[1].isString())
        return v4->throwTypeError(QStringLiteral("Arguments to path.resolve must be strings"));

    const QDir from(callData->args[0].toQString());
    return v4->newString(from.relativeFilePath(callData->args[1].toQString()))->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_dirname(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);
    if (!callData->argc || !callData->args[0].isString())
        return v4->throwTypeError(QStringLiteral("path.dirname: argument must be a string"));

    return v4->newString(
                QFileInfo(callData->args[0].toQStringNoThrow()).absolutePath())->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_basename(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);
    if (!callData->argc || !callData->args[0].isString())
        return v4->throwTypeError(QStringLiteral("path.basename: argument must be a string"));

    const QString path = callData->args[0].toQStringNoThrow();
    const QString fileName = QFileInfo(path).fileName();
    const QString ext = callData->argc > 1 ? callData->args[1].toQStringNoThrow() : QString();
    QString basename;
    if (!ext.isEmpty() && path.endsWith(ext))
        basename = fileName.left(fileName.size() - ext.size());
    else
        basename = fileName;

    return v4->newString(basename)->asReturnedValue();
}

QV4::ReturnedValue PathModule::method_extname(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);
    if (!callData->argc || !callData->args[0].isString())
        return v4->throwTypeError(QStringLiteral("path.extname: argument must be a string"));

    return v4->newString(
                QFileInfo(callData->args[0].toQStringNoThrow()).completeSuffix())->asReturnedValue();
}

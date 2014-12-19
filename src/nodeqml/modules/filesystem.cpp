#include "filesystem.h"

#include "../engine_p.h"

#include <QFile>
#include <QFileInfo>

#include <private/qv4context_p.h>

#include <unistd.h>
#include <sys/stat.h>

using namespace NodeQml;

Heap::FileSystemModule::FileSystemModule(QV4::ExecutionEngine *v4) :
    QV4::Heap::Object(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject self(scope, this);

    self->defineDefaultProperty(QStringLiteral("existsSync"), NodeQml::FileSystemModule::method_existsSync);
    self->defineDefaultProperty(QStringLiteral("mkdirSync"), NodeQml::FileSystemModule::method_mkdirSync, 2);
    self->defineDefaultProperty(QStringLiteral("renameSync"), NodeQml::FileSystemModule::method_renameSync);
    self->defineDefaultProperty(QStringLiteral("rmdirSync"), NodeQml::FileSystemModule::method_rmdirSync, 2);
    self->defineDefaultProperty(QStringLiteral("truncateSync"), NodeQml::FileSystemModule::method_truncateSync);
}

QV4::ReturnedValue FileSystemModule::method_existsSync(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    if (!callData->argc)
        return v4->throwError(QStringLiteral("existsSync: argument is required"));

    return QV4::Encode(QFileInfo::exists(callData->args[0].toQStringNoThrow()));
}

QV4::ReturnedValue FileSystemModule::method_mkdirSync(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    if (!callData->argc || !callData->args[0].isString())
        return v4->throwTypeError(QStringLiteral("path must be a string"));

    mode_t mode = 0777;
    /// FIXME: Use QV4::Value::isInt32()
    /// TODO: QV4::Value::isInt32() should be const
    if (callData->argc > 1 && callData->args[1].isNumber())
        mode = callData->args[1].toInt32();

    if (::mkdir(qPrintable(callData->args[0].toQStringNoThrow()), mode) == -1)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("mkdir"));

    return QV4::Encode::undefined();
}

QV4::ReturnedValue FileSystemModule::method_renameSync(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    if (callData->argc < 2)
        v4->throwError(QStringLiteral("renameSync: two arguments are required"));
    if (!callData->args[0].isString())
        v4->throwError(QStringLiteral("renameSync: old path must be a string"));
    if (!callData->args[1].isString())
        v4->throwError(QStringLiteral("renameSync: new path must be a string"));

    return QV4::Encode(QFile::rename(callData->args[0].toQStringNoThrow(),
                       callData->args[1].toQStringNoThrow()));
}

QV4::ReturnedValue FileSystemModule::method_rmdirSync(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    if (!callData->argc || !callData->args[0].isString())
        return v4->throwTypeError(QStringLiteral("path must be a string"));

    if (::rmdir(qPrintable(callData->args[0].toQStringNoThrow())))
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("rmdir"));

    return QV4::Encode::undefined();
}

QV4::ReturnedValue FileSystemModule::method_truncateSync(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    if (callData->argc < 2)
        v4->throwError(QStringLiteral("truncateSync: two arguments are required"));
    if (!callData->args[0].isString())
        v4->throwTypeError(QStringLiteral("truncateSync: path must be a string"));

    const off_t length = callData->args[1].toInt32();

    if (::truncate(qPrintable(callData->args[0].toQStringNoThrow()), length) == -1)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("truncate"));

    return QV4::Encode::undefined();
}

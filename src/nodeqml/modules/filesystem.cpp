#include "filesystem.h"

#include <QFile>
#include <QFileInfo>

#include <private/qv4context_p.h>

using namespace NodeQml;

Heap::FileSystemModule::FileSystemModule(QV4::ExecutionEngine *v4) :
    QV4::Heap::Object(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject self(scope, this);

    self->defineDefaultProperty(QStringLiteral("existsSync"), NodeQml::FileSystemModule::method_existsSync);
    self->defineDefaultProperty(QStringLiteral("renameSync"), NodeQml::FileSystemModule::method_renameSync);
}

QV4::ReturnedValue FileSystemModule::method_existsSync(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc)
        return ctx->engine()->throwError(QStringLiteral("existsSync: argument is required"));

    return QV4::Encode(QFileInfo::exists(callData->args[0].toQStringNoThrow()));
}

QV4::ReturnedValue FileSystemModule::method_renameSync(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (callData->argc < 2)
        ctx->engine()->throwError(QStringLiteral("renameSync: two arguments are required"));
    if (!callData->args[0].isString())
        ctx->engine()->throwError(QStringLiteral("renameSync: old path must be a string"));
    if (!callData->args[1].isString())
        ctx->engine()->throwError(QStringLiteral("renameSync: new path must be a string"));

    return QV4::Encode(QFile::rename(callData->args[0].toQStringNoThrow(),
                       callData->args[1].toQStringNoThrow()));
}

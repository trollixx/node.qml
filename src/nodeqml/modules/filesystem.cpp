#include "filesystem.h"

#include <QFile>
#include <QFileInfo>

using namespace NodeQml;

FileSystemModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);

    o->defineDefaultProperty(QStringLiteral("existsSync"), method_existsSync);
    o->defineDefaultProperty(QStringLiteral("renameSync"), method_renameSync);
}

QV4::ReturnedValue FileSystemModule::method_existsSync(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (!callData->argc)
        return ctx->throwError(QStringLiteral("existsSync: argument is required"));

    return QV4::Encode(QFileInfo::exists(callData->args[0].toQStringNoThrow()));
}

QV4::ReturnedValue FileSystemModule::method_renameSync(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    if (callData->argc < 2)
        return ctx->throwError(QStringLiteral("renameSync: two arguments are required"));
    if (!callData->args[0].isString())
        return ctx->throwError(QStringLiteral("renameSync: old path must be a string"));
    if (!callData->args[1].isString())
        return ctx->throwError(QStringLiteral("renameSync: new path must be a string"));

    return QV4::Encode(QFile::rename(callData->args[0].toQStringNoThrow(),
                       callData->args[1].toQStringNoThrow()));
}

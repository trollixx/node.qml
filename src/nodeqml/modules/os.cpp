#include "os.h"

#include "process.h"
#include "../engine_p.h"

#include <QDir>
#include <QHostInfo>

#ifdef Q_OS_LINUX
#include <sys/sysinfo.h>
#include <sys/utsname.h>
#endif

using namespace NodeQml;

OsModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject self(scope, this);
    QV4::ScopedString s(scope);

#ifdef Q_OS_WIN
    self->defineReadonlyProperty(QStringLiteral("EOL"), (s = v4->newString(QStringLiteral("\r\n"))));
#else
    self->defineReadonlyProperty(QStringLiteral("EOL"), (s = v4->newString(QStringLiteral("\n"))));
#endif

    self->defineDefaultProperty(QStringLiteral("tmpdir"), method_tmpdir);
    self->defineDefaultProperty(QStringLiteral("endianness"), method_endianness);
    self->defineDefaultProperty(QStringLiteral("hostname"), method_hostname);
    self->defineDefaultProperty(QStringLiteral("type"), method_type);
    self->defineDefaultProperty(QStringLiteral("platform"), method_platform);
    self->defineDefaultProperty(QStringLiteral("arch"), method_arch);
    self->defineDefaultProperty(QStringLiteral("release"), method_release);
    self->defineDefaultProperty(QStringLiteral("loadavg"), method_loadavg);
}


QV4::ReturnedValue OsModule::method_tmpdir(QV4::CallContext *ctx)
{
    return ctx->engine()->newString(QDir::tempPath())->asReturnedValue();
}

QV4::ReturnedValue OsModule::method_endianness(QV4::CallContext *ctx)
{
#if Q_BYTE_ORDER == Q_LITTLE_ENDIAN
    return ctx->engine()->newString(QStringLiteral("LE"))->asReturnedValue();
#else
    return ctx->engine()->newString(QStringLiteral("BE"))->asReturnedValue();
#endif
}

QV4::ReturnedValue OsModule::method_hostname(QV4::CallContext *ctx)
{
    return ctx->engine()->newString(QHostInfo::localHostName())->asReturnedValue();
}

QV4::ReturnedValue OsModule::method_type(QV4::CallContext *ctx)
{
#ifdef Q_OS_LINUX
    QV4::ExecutionEngine *v4 = ctx->engine();

    struct utsname info;
    if (uname(&info) < 0)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("uname"));

    return v4->newString(QString::fromLocal8Bit(info.sysname))->asReturnedValue();
#else
    return QV4::Encode::undefined();
#endif
}

QV4::ReturnedValue OsModule::method_platform(QV4::CallContext *ctx)
{
    QV4::Scope scope(ctx);
    QV4::ScopedString s(scope, ctx->engine()->newString(ProcessModule::platform()));
    return s->asReturnedValue();
}

QV4::ReturnedValue OsModule::method_arch(QV4::CallContext *ctx)
{
    QV4::Scope scope(ctx);
    QV4::ScopedString s(scope, ctx->engine()->newString(ProcessModule::arch()));
    return s->asReturnedValue();
}

QV4::ReturnedValue OsModule::method_release(QV4::CallContext *ctx)
{
#ifdef Q_OS_LINUX
    QV4::ExecutionEngine *v4 = ctx->engine();

    struct utsname info;
    if (uname(&info) < 0)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("uname"));

    return v4->newString(QString::fromLocal8Bit(info.release))->asReturnedValue();
#else
    return QV4::Encode::undefined();
#endif
}

QV4::ReturnedValue OsModule::method_loadavg(QV4::CallContext *ctx)
{
#ifdef Q_OS_LINUX
    QV4::ExecutionEngine *v4 = ctx->engine();

    struct sysinfo info;

    if (sysinfo(&info) < 0)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("sysinfo"));

    QV4::Scope scope(v4);
    QV4::ScopedArrayObject array(scope, v4->newArrayObject(3));
    array->arrayPut(0, QV4::Primitive::fromDouble(static_cast<double>(info.loads[0]) / 65536.0));
    array->arrayPut(1, QV4::Primitive::fromDouble(static_cast<double>(info.loads[1]) / 65536.0));
    array->arrayPut(2, QV4::Primitive::fromDouble(static_cast<double>(info.loads[2]) / 65536.0));

    QV4::ScopedValue v(scope, array->asReturnedValue());
    v->toQStringNoThrow();

    return array->asReturnedValue();
#else
    return QV4::Encode::undefined();
#endif
}

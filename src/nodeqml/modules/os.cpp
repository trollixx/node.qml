#include "os.h"

#include "process.h"
#include "../engine_p.h"

#include <QDir>
#include <QHostInfo>
#include <QNetworkInterface>

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
    self->defineDefaultProperty(QStringLiteral("uptime"), method_uptime);
    self->defineDefaultProperty(QStringLiteral("loadavg"), method_loadavg);
    self->defineDefaultProperty(QStringLiteral("totalmem"), method_totalmem);
    self->defineDefaultProperty(QStringLiteral("freemem"), method_freemem);
    self->defineDefaultProperty(QStringLiteral("networkInterfaces"), method_networkInterfaces);
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

QV4::ReturnedValue OsModule::method_uptime(QV4::CallContext *ctx)
{
#ifdef Q_OS_LINUX
    QV4::ExecutionEngine *v4 = ctx->engine();

    struct sysinfo info;

    if (sysinfo(&info) < 0)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("sysinfo"));

    return QV4::Primitive::fromDouble(info.uptime).asReturnedValue();
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

QV4::ReturnedValue OsModule::method_totalmem(QV4::CallContext *ctx)
{
#ifdef Q_OS_LINUX
    QV4::ExecutionEngine *v4 = ctx->engine();

    struct sysinfo info;

    if (sysinfo(&info) < 0)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("sysinfo"));

    return QV4::Primitive::fromDouble(info.totalram).asReturnedValue();
#else
    return QV4::Encode::undefined();
#endif
}

QV4::ReturnedValue OsModule::method_freemem(QV4::CallContext *ctx)
{
#ifdef Q_OS_LINUX
    QV4::ExecutionEngine *v4 = ctx->engine();

    struct sysinfo info;

    if (sysinfo(&info) < 0)
        return EnginePrivate::get(v4)->throwErrnoException(errno, QStringLiteral("sysinfo"));

    return QV4::Primitive::fromDouble(info.freeram).asReturnedValue();
#else
    return QV4::Encode::undefined();
#endif
}

QV4::ReturnedValue OsModule::method_networkInterfaces(QV4::CallContext *ctx)
{
    QV4::ExecutionEngine *v4 = ctx->engine();
    QV4::Scope scope(v4);

    QV4::ScopedObject info(scope, v4->newObject());
    QV4::ScopedValue v(scope);
    QV4::ScopedString s(scope);

    foreach (const QNetworkInterface &interface, QNetworkInterface::allInterfaces()) {
        const QList<QNetworkAddressEntry> addresses = interface.addressEntries();

        if (addresses.isEmpty())
            continue;

        QV4::ScopedArrayObject addressArray(scope, v4->newArrayObject());

        foreach (const QNetworkAddressEntry &address, addresses) {
            QV4::ScopedObject addressObject(scope, v4->newObject());
            // insertMember() to make it enumerable
            const QHostAddress ip = address.ip();
            addressObject->insertMember((s = v4->newString(QStringLiteral("address"))).getPointer(), (v = v4->newString(ip.toString())));
            addressObject->insertMember((s = v4->newString(QStringLiteral("netmask"))).getPointer(), (v = v4->newString(address.netmask().toString())));
            if (ip.protocol() == QAbstractSocket::IPv4Protocol)
                addressObject->insertMember((s = v4->newString(QStringLiteral("family"))).getPointer(), (v = v4->newString(QStringLiteral("IPv4"))));
            else if (ip.protocol() == QAbstractSocket::IPv6Protocol)
                addressObject->insertMember((s = v4->newString(QStringLiteral("family"))).getPointer(), (v = v4->newString(QStringLiteral("IPv6"))));
            addressObject->insertMember((s = v4->newString(QStringLiteral("mac"))).getPointer(), (v = v4->newString(interface.hardwareAddress())));
            if (!ip.scopeId().isEmpty())
                addressObject->insertMember((s = v4->newString(QStringLiteral("scopeid"))).getPointer(), (v = v4->newString(ip.scopeId())));
            addressObject->insertMember((s = v4->newString(QStringLiteral("internal"))).getPointer(), QV4::Primitive::fromBoolean(ip.isLoopback()));

            addressArray->push_back((v = addressObject));
        }

        info->insertMember((s = v4->newString(interface.name())).getPointer(), (v = addressArray));
    }

    return info->asReturnedValue();
}

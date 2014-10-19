#include "process.h"

#include <QCoreApplication>
#include <QDir>

using namespace NodeQml;

ProcessModule::Data::Data(QV4::ExecutionEngine *v4) :
    QV4::Object::Data(v4)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);

    o->defineDefaultProperty(QStringLiteral("abort"), method_abort);
    o->defineDefaultProperty(QStringLiteral("chdir"), method_chdir);
    o->defineDefaultProperty(QStringLiteral("cwd"), method_cwd);
    o->defineDefaultProperty(QStringLiteral("exit"), method_exit);
}

QV4::ReturnedValue ProcessModule::method_abort(QV4::CallContext *ctx)
{
    Q_UNUSED(ctx);
    ::abort();
}

QV4::ReturnedValue ProcessModule::method_chdir(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    if (!callData->argc || !callData->args[0].isString())
        return ctx->throwError(QStringLiteral("chdir: Bad argument"));

    /// TODO: Should have fs error code, like ENOENT or NOACCES
    // { [Error: ENOENT, no such file or directory] errno: 34, code: 'ENOENT', syscall: 'uv_chdir' }
    if (!QDir::setCurrent(callData->args[0].toQStringNoThrow()))
        return ctx->throwError(QStringLiteral("chdir: Cannot change directory"));

    return QV4::Encode::undefined();
}

QV4::ReturnedValue ProcessModule::method_cwd(QV4::CallContext *ctx)
{
    return ctx->engine()->newString(QDir::currentPath())->asReturnedValue();
}

QV4::ReturnedValue ProcessModule::method_exit(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    int code = 0;
    if (callData->argc)
        code = callData->args[0].toInt32();

    QCoreApplication::exit(code);
    return QV4::Encode::undefined();
}

/*QStringList Process::argv() const
{
    return QCoreApplication::arguments();
}

QString Process::execPath() const
{
    return QCoreApplication::applicationFilePath();
}

int Process::pid() const
{
    /// NOTE: QCoreApplication::applicationPid() returns int64
    return QCoreApplication::applicationPid();
}

QString Process::arch() const
{
    // Node supports: 'arm', 'ia32', or 'x64'
    /// TODO: Extend with all Q_PROCESSOR_*?
#if defined(Q_PROCESSOR_ARM)
    return QStringLiteral("arm");
#elif defined(Q_PROCESSOR_X86_32)
    return QStringLiteral("ia32");
#elif defined(Q_PROCESSOR_X86_64)
    return QStringLiteral("x64");
#else
    return QStringLiteral("");
#endif
}

QString Process::platform() const
{
    // Node supports: 'darwin', 'freebsd', 'linux', 'sunos' or 'win32'
    /// TODO: Extend with all Q_OS_*?
#if defined(Q_OS_DARWIN)
    return QStringLiteral("darwin");
#elif defined(Q_OS_FREEBSD)
    return QStringLiteral("freebsd");
#elif defined(Q_OS_FREEBSD)
    return QStringLiteral("freebsd");
#elif defined(Q_OS_LINUX)
    return QStringLiteral("linux");
#elif defined(Q_OS_SOLARIS)
    return QStringLiteral("sunos");
#elif defined(Q_OS_WIN32)
    return QStringLiteral("win32");
#else
    return QStringLiteral("");
#endif
}
*/

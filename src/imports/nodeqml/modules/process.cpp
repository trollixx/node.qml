#include "process.h"

#include <QCoreApplication>
#include <QDir>

Process::Process(QJSEngine *jsEngine, QObject *parent) :
    CoreModule(jsEngine, parent)
{
}

// TODO: Should throw an exception instead of return value
bool Process::chdir(const QString &directory)
{
    return QDir::setCurrent(directory);
}

QString Process::cwd() const
{
    return QDir::currentPath();
}

void Process::exit(int code) const
{
    QCoreApplication::exit(code);
}

QStringList Process::argv() const
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

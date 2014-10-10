#ifndef OS_H
#define OS_H

#include "coremodule.h"

class Os : public CoreModule
{
    Q_OBJECT
    Q_PROPERTY(QString EOL READ eol CONSTANT)
public:
    explicit Os(QJSEngine *jsEngine, QObject *parent = nullptr);

    Q_INVOKABLE QString tmpdir() const;
    Q_INVOKABLE QString endianness() const;
    Q_INVOKABLE QString hostname() const;
    /// TODO: os.type()
    /// TODO: os.platform()
    /// TODO: os.arch()
    /// TODO: os.release()
    /// TODO: os.uptime()
    /// TODO: os.loadavg()
    /// TODO: os.totalmem()
    /// TODO: os.freemem()
    /// TODO: os.cpus()
    /// TODO: os.networkInterfaces()

private:
    QString eol() const;
};

#endif // OS_H

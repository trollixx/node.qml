#ifndef OS_H
#define OS_H

#include <private/qv4object_p.h>

namespace NodeQml {

struct OsModule : QV4::Object
{
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4);
    };

    static QV4::ReturnedValue method_tmpdir(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_endianness(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_hostname(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_type(QV4::CallContext *ctx);
    /// TODO: os.platform()
    /// TODO: os.arch()
    /// TODO: os.release()
    /// TODO: os.uptime()
    /// TODO: os.loadavg()
    /// TODO: os.totalmem()
    /// TODO: os.freemem()
    /// TODO: os.cpus()
    /// TODO: os.networkInterfaces()

};

} // namespace NodeQml

#endif // OS_H

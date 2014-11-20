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
    static QV4::ReturnedValue method_platform(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_arch(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_release(QV4::CallContext *ctx);
    /// TODO: os.uptime()
    static QV4::ReturnedValue method_loadavg(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_totalmem(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_freemem(QV4::CallContext *ctx);
    /// TODO: os.cpus()
    /// TODO: os.networkInterfaces()

};

} // namespace NodeQml

#endif // OS_H

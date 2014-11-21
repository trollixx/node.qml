#ifndef OS_H
#define OS_H

#include "../v4integration.h"

#include <private/qv4object_p.h>

namespace NodeQml {

namespace Heap {

struct OsModule : QV4::Heap::Object {
    OsModule(QV4::ExecutionEngine *v4);
};

}

struct OsModule : QV4::Object
{
    NODE_V4_OBJECT(OsModule, Object)

    static QV4::ReturnedValue method_tmpdir(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_endianness(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_hostname(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_type(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_platform(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_arch(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_release(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_uptime(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_loadavg(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_totalmem(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_freemem(QV4::CallContext *ctx);
    /// TODO: os.cpus()
    static QV4::ReturnedValue method_networkInterfaces(QV4::CallContext *ctx);

};

} // namespace NodeQml

#endif // OS_H

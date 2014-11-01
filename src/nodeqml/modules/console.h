#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <QHash>

#include <private/qv4object_p.h>

namespace NodeQml {

struct ConsoleModule : QV4::Object
{
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4);
    };

    static QV4::ReturnedValue method_log(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_info(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_error(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_warn(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_dir(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_time(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_timeEnd(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_trace(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_assert(QV4::CallContext *ctx);
};

} // namespace NodeQml

#endif // FILESYSTEM_H

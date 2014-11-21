#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../v4integration.h"

#include <QHash>

#include <private/qv4object_p.h>

namespace NodeQml {

namespace Heap {

struct ConsoleModule : QV4::Heap::Object {
    ConsoleModule(QV4::ExecutionEngine *v4);

    QHash<QV4::ReturnedValue, qint64> timeMarks;
};

} // namespace Heap

struct ConsoleModule : QV4::Object
{
    NODE_V4_OBJECT(ConsoleModule, Object)

    static QV4::ReturnedValue method_log(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_error(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_dir(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_time(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_timeEnd(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_trace(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_assert(QV4::CallContext *ctx);
};

} // namespace NodeQml

#endif // FILESYSTEM_H

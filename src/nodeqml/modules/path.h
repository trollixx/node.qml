#ifndef PATH_H
#define PATH_H

#include "../v4integration.h"

#include <private/qv4object_p.h>

namespace NodeQml {

namespace Heap {

struct PathModule : QV4::Heap::Object {
    PathModule(QV4::ExecutionEngine *v4);
};

} // namespace Heap

struct PathModule : QV4::Object
{
    NODE_V4_OBJECT(PathModule, Object)

    static QV4::ReturnedValue method_normalize(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_join(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_resolve(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_relative(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_dirname(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_basename(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_extname(QV4::CallContext *ctx);

};

} // namespace NodeQml

#endif // PATH_H

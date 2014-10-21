#ifndef PATH_H
#define PATH_H

#include <private/qv4object_p.h>

namespace NodeQml {

struct PathModule : QV4::Object
{
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4);
    };

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

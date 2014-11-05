#ifndef UTIL_H
#define UTIL_H

#include <private/qv4object_p.h>

namespace NodeQml {

struct UtilModule : QV4::Object
{
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4);
    };

    static QV4::ReturnedValue method_format(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_log(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_inspect(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_isArray(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_isRegExp(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_isDate(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_isError(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_isUndefined(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_inherits(QV4::CallContext *ctx);

    static QString inspect(QV4::Value value);
};

} // namespace NodeQml

#endif // UTIL_H

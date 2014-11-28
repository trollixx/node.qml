#ifndef GLOBALEXTENSIONS_H
#define GLOBALEXTENSIONS_H

#include <private/qv4object_p.h>

class QQmlEngine;

namespace NodeQml {

struct GlobalExtensions {
    static void init(QQmlEngine *qmlEngine);

    static QV4::ReturnedValue method_require(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_setTimeout(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_clearTimeout(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_setInterval(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_clearInterval(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_captureStackTrace(QV4::CallContext *ctx);
};

} // namespace NodeQml

#endif // GLOBALEXTENSIONS_H

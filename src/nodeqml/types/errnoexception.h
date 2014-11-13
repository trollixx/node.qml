#ifndef ERRNOEXCEPTION_H
#define ERRNOEXCEPTION_H

#include <private/qv4errorobject_p.h>

namespace NodeQml {

struct ErrnoExceptionObject: QV4::ErrorObject {
    struct Data : QV4::ErrorObject::Data {
        Data(QV4::ExecutionEngine *v4, const QString &message, int errorNo,
             const QString &syscall = QString(), const QString &path = QString());
    };
    V4_OBJECT(ErrorObject)
};

} // namespace NodeQml

#endif // ERRNOEXCEPTION_H

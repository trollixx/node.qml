#ifndef ERRNOEXCEPTION_H
#define ERRNOEXCEPTION_H

#include "../v4integration.h"

#include <private/qv4errorobject_p.h>

namespace NodeQml {

namespace Heap {

struct ErrnoExceptionObject : QV4::Heap::ErrorObject {
    ErrnoExceptionObject(QV4::ExecutionEngine *v4, const QString &message, int errorNo,
                         const QString &syscall = QString(), const QString &path = QString());
};

} // namespace Heap

struct ErrnoExceptionObject: QV4::ErrorObject {
    NODE_V4_OBJECT(ErrnoExceptionObject, ErrorObject)
};

} // namespace NodeQml

#endif // ERRNOEXCEPTION_H

#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include "../v4integration.h"

#include <private/qv4object_p.h>

namespace NodeQml {

namespace Heap {

struct FileSystemModule : QV4::Heap::Object {
    FileSystemModule(QV4::ExecutionEngine *v4);
};

} // namespace Heap

struct FileSystemModule : QV4::Object
{
    NODE_V4_OBJECT(FileSystemModule, Object)

    static QV4::ReturnedValue method_existsSync(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_renameSync(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_truncateSync(QV4::CallContext *ctx);
};

} // namespace NodeQml

#endif // FILESYSTEM_H

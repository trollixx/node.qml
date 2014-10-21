#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <private/qv4object_p.h>

namespace NodeQml {

struct FileSystemModule : QV4::Object
{
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4);
    };

    static QV4::ReturnedValue method_existsSync(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_renameSync(QV4::CallContext *ctx);
};

} // namespace NodeQml

#endif // FILESYSTEM_H

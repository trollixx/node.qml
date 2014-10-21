#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include <private/qv4object_p.h>

namespace NodeQml {

struct ModuleObject: QV4::Object {
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4, const QString &id, const QString &filename);

        QString id;
        QString filename;
        bool loaded;
        QV4::Object *exportsObject;
        //QV4::ValueRef parent;
        //QV4::Value children;
    };

    V4_OBJECT(Object)

    enum {
        ExportsPropertyIndex = 0,
        IdPropertyIndex,
        FileNamePropertyIndex,
        LoadedPropertyIndex,
        ParentPropertyIndex,
        ChildrenPropertyIndex
    };
};

} // namespace NodeQml

#endif // MODULEOBJECT_H

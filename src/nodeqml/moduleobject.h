#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include <private/qv4object_p.h>
#include <private/qv4functionobject_p.h>

namespace NodeQml {

struct ModuleObject : QV4::Object {
    struct Data : QV4::Object::Data {
        Data(QV4::ExecutionEngine *v4, const QString &moduleId, ModuleObject *moduleParent = nullptr);

        QString id;
        QString filename;
        QString dirname;
        bool loaded;
        QV4::Object *exportsObject;
        ModuleObject *parent;
        QV4::ArrayObject *childrenArray;
    };

    V4_OBJECT(Object)

    void load(QV4::ExecutionContext *ctx, const QString &path);
    QV4::Object *compile(QV4::ExecutionContext *ctx);

    static QV4::Object *require(QV4::ExecutionContext *ctx, const QString &path, ModuleObject *parent = nullptr, bool isMain = false);

    static QString resolveModule(QV4::ExecutionContext *ctx, const QString &request, const QString &parentPath = QString());

    enum {
        ExportsPropertyIndex = 0,
        IdPropertyIndex,
        FileNamePropertyIndex,
        LoadedPropertyIndex,
        ParentPropertyIndex,
        ChildrenPropertyIndex
    };

    static ModuleObject *getThis(QV4::ExecutionContext *ctx);

    static QV4::ReturnedValue property_filename_getter(QV4::CallContext *ctx);
    static QV4::ReturnedValue property_loaded_getter(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_require(QV4::CallContext *ctx);
};

struct RequireFunction : QV4::FunctionObject
{
    struct Data : QV4::FunctionObject::Data {
        Data(QV4::ExecutionContext *scope, ModuleObject *moduleObject);
        ModuleObject *module;
    };
    V4_OBJECT(FunctionObject)

    static QV4::ReturnedValue call(QV4::Managed *that, QV4::CallData *callData);
};

} // namespace NodeQml

#endif // MODULEOBJECT_H

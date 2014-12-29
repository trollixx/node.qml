#ifndef MODULEOBJECT_H
#define MODULEOBJECT_H

#include "v4integration.h"

#include <private/qv4object_p.h>
#include <private/qv4functionobject_p.h>

namespace NodeQml {

struct ModuleObject;

namespace Heap {

struct ModuleObject : QV4::Heap::Object {
    ModuleObject(QV4::ExecutionEngine *v4, const QString &moduleId, ModuleObject *moduleParent = nullptr);

    QString id;
    QString filename;
    QString dirname;
    bool loaded = false;
    ModuleObject *parent = nullptr;
    QV4::Heap::Object *exports = nullptr;
    QV4::Heap::ArrayObject *childrenArray = nullptr;
};

struct RequireFunction : QV4::Heap::FunctionObject {
    RequireFunction(QV4::ExecutionContext *ctx, NodeQml::Heap::ModuleObject *moduleObject);

    NodeQml::Heap::ModuleObject *module = nullptr;
};

} // namespace Heap

struct ModuleObject : QV4::Object {
    NODE_V4_OBJECT(ModuleObject, Object)

    static void markObjects(QV4::Heap::Base *that, QV4::ExecutionEngine *e);

    static void load(QV4::ExecutionEngine *v4, Heap::ModuleObject *moduleObject, const QString &path);
    static void compile(QV4::ExecutionEngine *v4, Heap::ModuleObject *moduleObject);

    static QV4::ReturnedValue require(QV4::ExecutionEngine *v4, const QString &path, Heap::ModuleObject *parent = nullptr, bool isMain = false);
    static QString resolveModule(QV4::ExecutionEngine *v4, const QString &request, const QString &parentPath = QString());

    static QV4::ReturnedValue property_exports_getter(QV4::CallContext *ctx);
    static QV4::ReturnedValue property_exports_setter(QV4::CallContext *ctx);
    static QV4::ReturnedValue property_filename_getter(QV4::CallContext *ctx);
    static QV4::ReturnedValue property_loaded_getter(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_require(QV4::CallContext *ctx);
};

struct RequireFunction : QV4::FunctionObject
{
    NODE_V4_OBJECT(RequireFunction, FunctionObject)

    static QV4::ReturnedValue call(QV4::Managed *that, QV4::CallData *callData);
};

} // namespace NodeQml

#endif // MODULEOBJECT_H

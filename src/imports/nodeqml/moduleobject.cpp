#include "moduleobject.h"

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(ModuleObject);

ModuleObject::Data::Data(QV4::ExecutionEngine *v4, const QString &id, const QString &filename) :
    QV4::Object::Data(v4),
    id(id),
    filename(filename),
    loaded(false)
    //parent(parent),
    //children(v4->newArrayObject())
{
    setVTable(staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);

    QV4::ScopedString s(scope, this);

    o->defineReadonlyProperty(QStringLiteral("id"), (s = v4->newString(id)));
    o->defineReadonlyProperty(QStringLiteral("filename"), (s = v4->newString(filename)));
    o->defineReadonlyProperty(QStringLiteral("loaded"), QV4::Primitive::fromBoolean(loaded));

    QV4::ScopedObject ex(scope, v4->newObject());
    exportsObject = ex->asObject();
    o->defineDefaultProperty(QStringLiteral("exports"), ex);
}

#include "buffer.h"

#include "../engine.h"

#include <private/qv4engine_p.h>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(BufferObject);

BufferObject::Data::Data(QV4::InternalClass *ic)
    : Object::Data(ic)
{
    Q_ASSERT(internalClass->vtable == staticVTable());
    value = QByteArray();

    QV4::Scope scope(ic->engine);
    QV4::ScopedObject s(scope, this);
    s->defineReadonlyProperty(ic->engine->id_length, QV4::Primitive::fromInt32(0));
}

BufferObject::Data::Data(QV4::ExecutionEngine *v4, quint32 size) :
    Object::Data(Engine::get(v4)->bufferClass)
{
    value.resize(size);
    QV4::Scope scope(v4);
    QV4::ScopedObject s(scope, this);
    s->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(size));
}

BufferObject::Data::Data(QV4::ExecutionEngine *v4, const QString &str, const QString &encoding) :
    Object::Data(Engine::get(v4)->bufferClass)
{

}

QV4::Property *BufferObject::getIndex(uint index) const
{

}

bool BufferObject::deleteIndexedProperty(QV4::Managed *m, uint index)
{

}

void BufferObject::advanceIterator(QV4::Managed *m, QV4::ObjectIterator *it, QV4::String *&name, uint *index, QV4::Property *p, QV4::PropertyAttributes *attrs)
{

}

void BufferObject::markObjects(QV4::Managed *that, QV4::ExecutionEngine *e)
{

}

DEFINE_OBJECT_VTABLE(BufferCtor);

BufferCtor::Data::Data(QV4::ExecutionContext *scope)
    : FunctionObject::Data(scope, QStringLiteral("Buffer"))
{
    setVTable(staticVTable());
}

QV4::ReturnedValue BufferCtor::construct(QV4::Managed *m, QV4::CallData *callData)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::ScopedValue value(scope);
    if (callData->argc) {
        if (callData->args[0].isInt32()) {
            QV4::Scoped<BufferObject> object(scope, v4->memoryManager->alloc<BufferObject>(v4, callData->args[0].toInt32()));
            return QV4::Encode(object->asReturned<QV4::Object>());
        } else if (callData->args[0].asArrayObject()) {
            /// TODO: Implement Buffer(array)
            return QV4::Encode::undefined();
        } else if (callData->args[0].isString()) {
            QString encoding;
            if (callData->argc > 1 && callData->args[1].isString()) {
                encoding = callData->args[1].toQStringNoThrow();
                /// TODO: Call isEncoding() and throw exception
            } else {
                encoding = QStringLiteral("utf8");
            }
            QV4::Scoped<BufferObject> object(scope, v4->memoryManager->alloc<BufferObject>(v4, callData->args[0].toQStringNoThrow(), encoding));
            return QV4::Encode(object->asReturned<QV4::Object>());
        }
    }

    return v4->currentContext()->throwTypeError(QStringLiteral("Buffer: First argument needs to be a number, array or string."));
}

QV4::ReturnedValue BufferCtor::call(QV4::Managed *that, QV4::CallData *callData)
{
    return construct(that, callData);
}


void BufferPrototype::init(QV4::ExecutionEngine *v4, QV4::Object *ctor)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope);
    ctor->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(1));
    ctor->defineReadonlyProperty(v4->id_prototype, (o = this));
    ctor->defineDefaultProperty(QStringLiteral("isBuffer"), method_isBuffer, 1);
    defineDefaultProperty(QStringLiteral("constructor"), (o = ctor));
}

QV4::ReturnedValue BufferPrototype::method_isEncoding(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("Buffer.isEncoding()"));
}

QV4::ReturnedValue BufferPrototype::method_isBuffer(QV4::CallContext *ctx)
{
    return ctx->d()->callData->argc && ctx->d()->callData->args[0].as<BufferObject>();
}

QV4::ReturnedValue BufferPrototype::method_byteLength(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("Buffer.byteLength()"));
}

QV4::ReturnedValue BufferPrototype::method_concat(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("Buffer.concat()"));
}

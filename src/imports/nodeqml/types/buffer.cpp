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

QV4::ReturnedValue BufferObject::getIndexed(QV4::Managed *m, quint32 index, bool *hasProperty)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<BufferObject> that(scope, static_cast<BufferObject *>(m));

    if (index >= static_cast<quint32>(that->d()->value.size())) {
        if (hasProperty)
            *hasProperty = false;
        return QV4::Encode::undefined();
    }

    if (hasProperty)
        *hasProperty = true;
    return QV4::Encode(v4->newNumberObject(QV4::Primitive::fromUInt32(that->d()->value.at(index))));
}

void BufferObject::putIndexed(QV4::Managed *m, uint index, const QV4::ValueRef value)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<BufferObject> that(scope, static_cast<BufferObject *>(m));

    if (index >= static_cast<quint32>(that->d()->value.size()))
        return;

    that->d()->value[index] = value->toUInt16();
}

bool BufferObject::deleteIndexedProperty(QV4::Managed *m, uint index)
{
    Q_UNUSED(m)
    Q_UNUSED(index)
    return true;
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
    defineDefaultProperty(QStringLiteral("constructor"), (o = ctor));

    ctor->defineDefaultProperty(QStringLiteral("isEncoding"), method_isEncoding, 1);
    ctor->defineDefaultProperty(QStringLiteral("isBuffer"), method_isBuffer, 1);
    ctor->defineDefaultProperty(QStringLiteral("byteLength"), method_byteLength);
}

bool BufferPrototype::isEncoding(const QString &encoding)
{
    static QStringList encodings = {
        QStringLiteral("hex"),
        QStringLiteral("utf8"),
        QStringLiteral("utf-8"),
        QStringLiteral("ascii"),
        QStringLiteral("binary"),
        QStringLiteral("base64"),
        QStringLiteral("raw"),
        QStringLiteral("ucs2"),
        QStringLiteral("ucs-2"),
        QStringLiteral("utf16le"),
        QStringLiteral("utf-16le")
    };
    return encodings.contains(encoding.toLower());
}

QV4::ReturnedValue BufferPrototype::method_isEncoding(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    return QV4::Encode(callData->argc && callData->args[0].isString()
            && isEncoding(callData->args[0].toQStringNoThrow()));
}

QV4::ReturnedValue BufferPrototype::method_isBuffer(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    return QV4::Encode(callData->argc && callData->args[0].as<BufferObject>());
}

QV4::ReturnedValue BufferPrototype::method_byteLength(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    if (!callData->argc || !callData->args[0].isString())
        return ctx->throwTypeError(QStringLiteral("byteLength: argument must be a string"));

    QString encoding;
    if (callData->argc > 1 && callData->args[1].isString()
            && isEncoding(callData->args[1].toQStringNoThrow())) {
        encoding = callData->args[1].toQStringNoThrow();
    } else {
        encoding = QStringLiteral("utf8");
    }

    quint32 length = 0;

    if (encoding == QStringLiteral("ascii") || encoding == QStringLiteral("binary")
            || encoding == QStringLiteral("raw")) {
        length = callData->args[0].stringValue()->d()->length();
    } else if (encoding == QStringLiteral("ucs2") || encoding == QStringLiteral("ucs-2")
               || encoding == QStringLiteral("utf16le") || encoding == QStringLiteral("utf-16le")) {
        length = callData->args[0].stringValue()->d()->length() * 2;
    } else if (encoding == QStringLiteral("hex")) {
        length = callData->args[0].stringValue()->d()->length() >> 1;
    } else if (encoding == QStringLiteral("base64")) {
        length = QByteArray::fromBase64(callData->args[0].toQStringNoThrow().toUtf8()).length();
    } else { // utf8
        length = callData->args[0].toQStringNoThrow().toUtf8().length();
    }

    return QV4::Encode(length);
}

QV4::ReturnedValue BufferPrototype::method_concat(QV4::CallContext *ctx)
{
    return ctx->throwUnimplemented(QStringLiteral("Buffer.concat()"));
}

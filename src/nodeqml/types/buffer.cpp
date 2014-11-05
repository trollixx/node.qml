#include "buffer.h"

#include "../engine_p.h"

#include <private/qv4engine_p.h>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(BufferObject);

BufferObject::Data::Data(QV4::InternalClass *ic)
    : Object::Data(ic)
{
    Q_ASSERT(internalClass->vtable == staticVTable());

    QV4::Scope scope(ic->engine);
    QV4::ScopedObject s(scope, this);
    s->defineReadonlyProperty(ic->engine->id_length, QV4::Primitive::fromInt32(0));
}

BufferObject::Data::Data(QV4::ExecutionEngine *v4, quint32 size) :
    Object::Data(EnginePrivate::get(v4)->bufferClass)
{
    setVTable(staticVTable());
    value.resize(size);

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(size));
}

BufferObject::Data::Data(QV4::ExecutionEngine *v4, const QString &str, const QString &encoding) :
    Object::Data(EnginePrivate::get(v4)->bufferClass)
{
    setVTable(staticVTable());
}

BufferObject::Data::Data(QV4::ExecutionEngine *v4, QV4::ArrayObject *array) :
    Object::Data(EnginePrivate::get(v4)->bufferClass)
{
    setVTable(staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    QV4::ScopedArrayObject a(scope, array);
    QV4::ScopedValue v(scope);

    const uint length = a->getLength();
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(length));
    value.resize(length);
    for (uint i = 0; i < length; ++i) {
        v = array->getIndexed(i);
        value[i] = v->toInt32() & 0xff;
    }
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

    return QV4::Primitive::fromUInt32(that->d()->value.at(index) & 0xff).asReturnedValue();
}

void BufferObject::putIndexed(QV4::Managed *m, uint index, const QV4::ValueRef value)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<BufferObject> that(scope, static_cast<BufferObject *>(m));

    if (index >= static_cast<quint32>(that->d()->value.size()))
        return;

    that->d()->value[index] = value->toInt32();
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
    if (callData->argc) {
        if (callData->args[0].isInt32()) {
            QV4::Scoped<BufferObject> object(scope, v4->memoryManager->alloc<BufferObject>(v4, callData->args[0].toInt32()));
            return QV4::Encode(object->asReturned<QV4::Object>());
        } else if (callData->args[0].asArrayObject()) {
            QV4::Scoped<BufferObject> object(scope, v4->memoryManager->alloc<BufferObject>(v4, callData->args[0].asArrayObject()));
            return object->asReturnedValue();
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

    defineDefaultProperty(QStringLiteral("copy"), method_copy, 4);
    defineDefaultProperty(QStringLiteral("fill"), method_fill, 3);
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

// copy(targetBuffer, [targetStart], [sourceStart], [sourceEnd])
QV4::ReturnedValue BufferPrototype::method_copy(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;

    if (!callData->argc || !callData->args[0].as<BufferObject>())
        return ctx->throwTypeError(QStringLiteral("copy: First arg should be a Buffer"));

    QV4::Scope scope(ctx);
    QV4::Scoped<BufferObject> self(scope, getThis(ctx));

    QV4::Scoped<BufferObject> target(scope, callData->args[0].as<BufferObject>());

    size_t targetStart = 0;
    size_t sourceStart = 0;
    size_t sourceEnd = self->getLength();

    if (callData->argc > 1) {
        if (!callData->args[1].isNumber())
            return ctx->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[1].toInt32() < 0)
            return ctx->throwRangeError(QStringLiteral("Out of range index"));

        targetStart = callData->args[1].toInt32();
    }

    if (callData->argc > 2) {
        if (!callData->args[2].isNumber())
            return ctx->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[2].toInt32() < 0)
            return ctx->throwRangeError(QStringLiteral("Out of range index"));
        sourceStart = callData->args[2].toInt32();
    }

    if (callData->argc > 3) {
        if (!callData->args[3].isNumber())
            return ctx->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[3].toInt32() < 0)
            return ctx->throwRangeError(QStringLiteral("Out of range index"));
        sourceEnd = callData->args[3].toInt32();
    }

    qDebug("Target length: %u", target->getLength());

    // Copy zero bytes, we're done
    if (targetStart >= target->getLength() || sourceStart >= sourceEnd)
        return QV4::Encode(0);

    if (sourceStart > self->getLength())
        return ctx->throwRangeError(QStringLiteral("copy: Out of range index"));

    const size_t targetLength = target->getLength();
    if (sourceEnd - sourceStart > targetLength - targetStart)
        sourceEnd = sourceStart + targetLength - targetStart;
    size_t to_copy = qMin(qMin(sourceEnd - sourceStart, targetLength - targetStart),
                          self->getLength() - sourceStart);
    memmove(target->d()->value.data() + targetStart, self->d()->value.constData() + sourceStart, to_copy);
    return QV4::Primitive::fromUInt32(to_copy).asReturnedValue();
}

// fill(value, [offset], [end])
QV4::ReturnedValue BufferPrototype::method_fill(QV4::CallContext *ctx)
{
    const QV4::CallData * const callData = ctx->d()->callData;
    QV4::Scope scope(ctx);
    QV4::Scoped<BufferObject> self(scope, getThis(ctx));

    /// TODO: SLICE_START_END (https://github.com/joyent/node/blob/master/src/node_buffer.cc#L52)

    int offset = 0;
    int end = self->d()->value.size();

    if (!callData->argc)
        return QV4::Encode::undefined();

    if (callData->argc > 1) {
        if (!callData->args[1].isNumber())
            return ctx->throwTypeError(QStringLiteral("Bad argument"));
        offset = callData->args[1].toInt32();
        if (offset < 0)
            return ctx->throwRangeError(QStringLiteral("Out of range index"));
    }

    if (callData->argc > 2) {
        if (!callData->args[2].isNumber())
            return ctx->throwTypeError(QStringLiteral("Bad argument"));
        end = callData->args[2].toInt32();
        if (end < 0)
            return ctx->throwRangeError(QStringLiteral("Out of range index"));
    }

    const int length = end - offset;
    char * const startPtr = self->d()->value.data() + offset;

    if (callData->args[0].isNumber()) {
        const quint8 value = callData->args[0].toUInt32() & 0xff;
        memset(startPtr, value, length);
        return QV4::Encode::undefined();
    }

    if (!callData->args[0].isString())
        return ctx->throwTypeError(QStringLiteral("fill: value is not a number"));

    const QByteArray value = callData->args[0].toQStringNoThrow().toUtf8();

    // optimize single ascii character case
    if (value.size() == 1) {
        memset(startPtr, value.at(0), length);
        return QV4::Encode::undefined();
    }

    int in_there = value.size();
    char * ptr = startPtr + value.size();
    memcpy(startPtr, value.constData(), qMin(value.size(), length));
    if (value.size() >= length)
        return QV4::Encode::undefined();

    while (in_there < length - in_there) {
        memcpy(ptr, startPtr, in_there);
        ptr += in_there;
        in_there *= 2;
    }

    if (in_there < length)
        memcpy(ptr, startPtr, length - in_there);

    return QV4::Encode::undefined();
}

BufferObject *BufferPrototype::getThis(QV4::ExecutionContext *ctx)
{
    QV4::Scope scope(ctx);
    QV4::Scoped<BufferObject> self(scope, ctx->d()->callData->thisObject);
    return self.getPointer();
}

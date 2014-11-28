#include "buffer.h"

#include "../engine_p.h"

#include <QJsonArray>
#include <QJsonObject>

#include <private/qv4engine_p.h>
#include <private/qv4jsonobject_p.h>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(BufferObject);

Heap::BufferObject::BufferObject(QV4::ExecutionEngine *v4, size_t length) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass)
{
    setVTable(NodeQml::BufferObject::staticVTable());

    if (!allocateData(length)) {
        v4->throwRangeError(QStringLiteral("Buffer: Out of memory"));
        return;
    }

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(length));
}

/// TODO: new Buffer(str, [encoding])
Heap::BufferObject::BufferObject(QV4::ExecutionEngine *v4, const QString &str, BufferEncoding encoding) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass)
{
    setVTable(NodeQml::BufferObject::staticVTable());
}

Heap::BufferObject::BufferObject(QV4::ExecutionEngine *v4, QV4::ArrayObject *array) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass)
{
    setVTable(NodeQml::BufferObject::staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedArrayObject a(scope, array);
    QV4::ScopedValue v(scope);

    const uint length = a->getLength();

    if (!allocateData(length)) {
        v4->throwRangeError(QStringLiteral("Buffer: Out of memory"));
        return;
    }

    for (uint i = 0; i < length; ++i) {
        v = array->getIndexed(i);
        data[i] = v->toInt32() & 0xff;
    }

    QV4::ScopedObject o(scope, this);
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(length));
}

Heap::BufferObject::BufferObject(QV4::ExecutionEngine *v4, const QByteArray &ba) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass)
{
    setVTable(NodeQml::BufferObject::staticVTable());

    const size_t length = ba.length();

    if (!allocateData(length)) {
        v4->throwRangeError(QStringLiteral("Buffer: Out of memory"));
        return;
    }

    for (uint i = 0; i < length; ++i)
        data.data()[i] = ba.at(i) & 0xff;

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(length));
}

Heap::BufferObject::BufferObject(QV4::ExecutionEngine *v4, const QTypedArrayDataSlice<char> &slice) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass),
    data(slice)
{
    setVTable(NodeQml::BufferObject::staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(data.size()));
}

Heap::BufferObject::~BufferObject()
{
    data.clearData();
}

bool Heap::BufferObject::allocateData(size_t length)
{
    if (!length)
        return true;

    QTypedArrayData<char> *arrayData = QTypedArrayData<char>::allocate(length + 1);
    if (!arrayData)
        return false;
    arrayData->size = length;

    data.setData(arrayData);
    arrayData->ref.deref(); // Disown data
    return true;
}

QV4::ReturnedValue BufferObject::getIndexed(QV4::Managed *m, quint32 index, bool *hasProperty)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<BufferObject> that(scope, static_cast<BufferObject *>(m));

    if (index >= static_cast<quint32>(that->d()->data.size())) {
        if (hasProperty)
            *hasProperty = false;
        return QV4::Encode::undefined();
    }

    if (hasProperty)
        *hasProperty = true;

    return QV4::Primitive::fromUInt32(that->d()->data.at(index) & 0xff).asReturnedValue();
}

void BufferObject::putIndexed(QV4::Managed *m, uint index, const QV4::ValueRef value)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<BufferObject> that(scope, static_cast<BufferObject *>(m));

    if (index >= static_cast<quint32>(that->d()->data.size()))
        return;

    that->d()->data[index] = value->toInt32();
}

bool BufferObject::deleteIndexedProperty(QV4::Managed *m, uint index)
{
    Q_UNUSED(m)
    Q_UNUSED(index)
    return true;
}

BufferEncoding BufferObject::parseEncoding(const QString &str)
{
    const static QHash<QString, BufferEncoding> encodings = {
        std::pair<QString, BufferEncoding>(QStringLiteral("hex"), BufferEncoding::Hex),
        std::pair<QString, BufferEncoding>(QStringLiteral("utf8"), BufferEncoding::Utf8),
        std::pair<QString, BufferEncoding>(QStringLiteral("utf-8"), BufferEncoding::Utf8),
        std::pair<QString, BufferEncoding>(QStringLiteral("ascii"), BufferEncoding::Ascii),
        std::pair<QString, BufferEncoding>(QStringLiteral("binary"), BufferEncoding::Binary),
        std::pair<QString, BufferEncoding>(QStringLiteral("base64"), BufferEncoding::Base64),
        std::pair<QString, BufferEncoding>(QStringLiteral("raw"), BufferEncoding::Raw),
        std::pair<QString, BufferEncoding>(QStringLiteral("ucs2"), BufferEncoding::Ucs2),
        std::pair<QString, BufferEncoding>(QStringLiteral("ucs-2"), BufferEncoding::Ucs2),
        std::pair<QString, BufferEncoding>(QStringLiteral("utf16le"), BufferEncoding::Utf16le),
        std::pair<QString, BufferEncoding>(QStringLiteral("utf-16le"), BufferEncoding::Utf16le)
    };

    const QString encodingStr = str.toLower();

    if (!encodings.contains(encodingStr))
        return BufferEncoding::Invalid;
    return encodings.value(encodingStr);
}

bool BufferObject::isEncoding(const QString &str)
{
    return parseEncoding(str) != BufferEncoding::Invalid;
}

DEFINE_OBJECT_VTABLE(BufferCtor);

Heap::BufferCtor::BufferCtor(QV4::ExecutionContext *scope) :
    QV4::Heap::FunctionObject(scope, QStringLiteral("Buffer"))
{
    setVTable(NodeQml::BufferCtor::staticVTable());
}

QV4::ReturnedValue BufferCtor::construct(QV4::Managed *m, QV4::CallData *callData)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    if (callData->argc) {
        if (callData->args[0].isInt32()) {
            QV4::Scoped<BufferObject> object(scope, v4->memoryManager->alloc<BufferObject>(v4, callData->args[0].toInt32()));
            return object->asReturnedValue();
        } else if (callData->args[0].asArrayObject()) {
            QV4::Scoped<BufferObject> object(scope, v4->memoryManager->alloc<BufferObject>(v4, callData->args[0].asArrayObject()));
            return object->asReturnedValue();
        } else if (callData->args[0].isString()) {
            BufferEncoding encoding = BufferEncoding::Utf8;
            if (callData->argc > 1) {
                if (!callData->args[1].isString())
                    return v4->throwTypeError(QStringLiteral("Encoding must me a string"));
                const QString encStr = callData->args[1].toQStringNoThrow();
                BufferEncoding enc = BufferObject::parseEncoding(encStr);
                if (enc == BufferEncoding::Invalid)
                    return v4->throwTypeError(QString("Unknown Encoding: %1").arg(encStr));
                encoding = enc;
            }
            QV4::Scoped<BufferObject> object(scope, v4->memoryManager->alloc<BufferObject>(v4, callData->args[0].toQStringNoThrow(), encoding));
            return object->asReturnedValue();
        }
    }

    return v4->throwTypeError(QStringLiteral("Buffer: First argument needs to be a number, array or string."));
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
    defineDefaultProperty(QStringLiteral("slice"), method_slice, 2);
    defineDefaultProperty(QStringLiteral("toString"), method_toString, 3);
    defineDefaultProperty(QStringLiteral("toJSON"), method_toJSON);
}

QV4::ReturnedValue BufferPrototype::method_isEncoding(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    return QV4::Encode(callData->argc && callData->args[0].isString()
            && BufferObject::isEncoding(callData->args[0].toQStringNoThrow()));
}

QV4::ReturnedValue BufferPrototype::method_isBuffer(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    return QV4::Encode(callData->argc && callData->args[0].as<BufferObject>());
}

QV4::ReturnedValue BufferPrototype::method_byteLength(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);

    if (!callData->argc || !callData->args[0].isString())
        return ctx->engine()->throwTypeError(QStringLiteral("byteLength: argument must be a string"));

    BufferEncoding encoding = BufferEncoding::Utf8;
    if (callData->argc > 1 && callData->args[1].isString()
            && BufferObject::isEncoding(callData->args[1].toQStringNoThrow())) {
        encoding = BufferObject::parseEncoding(callData->args[1].toQStringNoThrow());
    }

    switch (encoding) {
    case BufferEncoding::Ascii:
    case BufferEncoding::Binary:
    case BufferEncoding::Raw:
        return QV4::Encode(callData->args[0].stringValue()->d()->length());
    case BufferEncoding::Base64:
        return QV4::Encode(QByteArray::fromBase64(
                               callData->args[0].toQStringNoThrow().toUtf8()).length());
    case BufferEncoding::Hex:
        return QV4::Encode(callData->args[0].stringValue()->d()->length() >> 1);
    case BufferEncoding::Ucs2:
    case BufferEncoding::Utf16le:
        return QV4::Encode(callData->args[0].stringValue()->d()->length() * 2);
    case BufferEncoding::Utf8:
        return QV4::Encode(callData->args[0].toQStringNoThrow().toUtf8().length());
    case BufferEncoding::Invalid:
        return QV4::Encode::undefined();
    }

    return QV4::Encode::undefined();
}

QV4::ReturnedValue BufferPrototype::method_concat(QV4::CallContext *ctx)
{
    return ctx->engine()->throwUnimplemented(QStringLiteral("Buffer.concat()"));
}

// toString([encoding], [start], [end])
QV4::ReturnedValue BufferPrototype::method_toString(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(BufferObject, ctx);
    NODE_CTX_V4(ctx);

    BufferEncoding encoding = BufferEncoding::Utf8;
    if (callData->argc) {
        const QString encodingStr = callData->args[0].toQStringNoThrow();
        encoding = BufferObject::parseEncoding(encodingStr);
        if (encoding == BufferEncoding::Invalid)
            return v4->throwTypeError(QString("Unknown encoding: %1").arg(encodingStr));
    }

    const int dataSize = self->d()->data.size();
    int start = callData->argc > 1
            ? qMin(callData->args[1].toInt32(), dataSize) : 0;
    int end = callData->argc > 2 && !callData->args[2].isUndefined()
            ? qMin(callData->args[2].toInt32(), dataSize) : dataSize - 1;

    if (start < 0)
        start = 0;
    if (end < 0)
        end = dataSize - 1;

    const char *startPtr = self->d()->data.data() + start;
    const int size = end - start + 1;

    const QByteArray data = QByteArray::fromRawData(startPtr, size);
    QString str;

    switch (encoding) {
    case BufferEncoding::Ascii:
    case BufferEncoding::Binary:
    case BufferEncoding::Raw:
        str = QString::fromLatin1(data);
        break;
    case BufferEncoding::Base64:
        str = QByteArray::fromBase64(data);
        break;
    case BufferEncoding::Hex:
        str = QByteArray::fromHex(data);
        break;
    case BufferEncoding::Ucs2:
    case BufferEncoding::Utf16le:
        str = QString::fromUtf16(reinterpret_cast<const ushort *>(startPtr), size >> 1);
        break;
    case BufferEncoding::Utf8:
        str = QString::fromUtf8(data);
        break;
    case BufferEncoding::Invalid:
        // Should never happen
        break;
    }

    QV4::ScopedString s(scope, v4->newString(str));
    return s->asReturnedValue();
}

QV4::ReturnedValue BufferPrototype::method_toJSON(QV4::CallContext *ctx)
{
    NODE_CTX_SELF(BufferObject, ctx);
    NODE_CTX_V4(ctx);

    QJsonObject json;
    json.insert(QStringLiteral("type"), QStringLiteral("Buffer"));

    QJsonArray data;
    for (int i = 0; i < self->d()->data.size(); ++i)
        data.append(self->d()->data[i]);

    json.insert(QStringLiteral("data"), data);
    return QV4::JsonObject::fromJsonObject(v4, json);
}

// copy(targetBuffer, [targetStart], [sourceStart], [sourceEnd])
QV4::ReturnedValue BufferPrototype::method_copy(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);

    if (!callData->argc || !callData->args[0].as<BufferObject>())
        return ctx->engine()->throwTypeError(QStringLiteral("copy: First arg should be a Buffer"));

    NODE_CTX_SELF(BufferObject, ctx);

    QV4::Scoped<BufferObject> target(scope, callData->args[0].as<BufferObject>());

    size_t targetStart = 0;
    size_t sourceStart = 0;
    size_t sourceEnd = self->getLength();

    if (callData->argc > 1) {
        if (!callData->args[1].isNumber())
            return ctx->engine()->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[1].toInt32() < 0)
            return ctx->engine()->throwRangeError(QStringLiteral("Out of range index"));

        targetStart = callData->args[1].toInt32();
    }

    if (callData->argc > 2) {
        if (!callData->args[2].isNumber())
            return ctx->engine()->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[2].toInt32() < 0)
            return ctx->engine()->throwRangeError(QStringLiteral("Out of range index"));
        sourceStart = callData->args[2].toInt32();
    }

    if (callData->argc > 3) {
        if (!callData->args[3].isNumber())
            return ctx->engine()->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[3].toInt32() < 0)
            return ctx->engine()->throwRangeError(QStringLiteral("Out of range index"));
        sourceEnd = callData->args[3].toInt32();
    }

    qDebug("Target length: %u", target->getLength());

    // Copy zero bytes, we're done
    if (targetStart >= target->getLength() || sourceStart >= sourceEnd)
        return QV4::Encode(0);

    if (sourceStart > self->getLength())
        return ctx->engine()->throwRangeError(QStringLiteral("copy: Out of range index"));

    const size_t targetLength = target->getLength();
    if (sourceEnd - sourceStart > targetLength - targetStart)
        sourceEnd = sourceStart + targetLength - targetStart;
    size_t to_copy = qMin(qMin(sourceEnd - sourceStart, targetLength - targetStart),
                          self->getLength() - sourceStart);
    memmove(target->d()->data.data() + targetStart, self->d()->data.constData() + sourceStart, to_copy);
    return QV4::Primitive::fromUInt32(to_copy).asReturnedValue();
}

// fill(value, [offset], [end])
QV4::ReturnedValue BufferPrototype::method_fill(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(BufferObject, ctx);

    /// TODO: SLICE_START_END (https://github.com/joyent/node/blob/master/src/node_buffer.cc#L52)

    int offset = 0;
    int end = self->d()->data.size();

    if (!callData->argc)
        return QV4::Encode::undefined();

    if (callData->argc > 1) {
        if (!callData->args[1].isNumber())
            return ctx->engine()->throwTypeError(QStringLiteral("Bad argument"));
        offset = callData->args[1].toInt32();
        if (offset < 0)
            return ctx->engine()->throwRangeError(QStringLiteral("Out of range index"));
    }

    if (callData->argc > 2) {
        if (!callData->args[2].isNumber())
            return ctx->engine()->throwTypeError(QStringLiteral("Bad argument"));
        end = callData->args[2].toInt32();
        if (end < 0)
            return ctx->engine()->throwRangeError(QStringLiteral("Out of range index"));
    }

    const int length = end - offset;
    char * const startPtr = self->d()->data.data() + offset;

    if (callData->args[0].isNumber()) {
        const quint8 value = callData->args[0].toUInt32() & 0xff;
        memset(startPtr, value, length);
        return QV4::Encode::undefined();
    }

    if (!callData->args[0].isString())
        return ctx->engine()->throwTypeError(QStringLiteral("fill: value is not a number"));

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

// slice([start], [end])
QV4::ReturnedValue BufferPrototype::method_slice(QV4::CallContext *ctx)
{
    NODE_CTX_V4(ctx);
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(BufferObject, ctx);

    if (!self)
        return v4->throwTypeError();

    int start = callData->argc > 0 ? callData->args[0].toInt32() : 0;
    int end = callData->argc < 2 || callData->args[1].isUndefined()
            ? self->d()->data.size() - 1 : callData->args[1].toInt32();

    if (start < 0)
        start = qMax(self->d()->data.size() + start, 0);
    if (end < 0)
        end = qMax(self->d()->data.size() + end, 0);

    if (end < start)
        return v4->throwRangeError(QStringLiteral("slice: start cannot exceed end"));

    QTypedArrayDataSlice<char> slice(self->d()->data, start, end - start + 1);
    QV4::Scoped<BufferObject> newBuffer(scope, v4->memoryManager->alloc<BufferObject>(v4, slice));
    return newBuffer->asReturnedValue();
}

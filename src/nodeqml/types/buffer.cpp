#include "buffer.h"

#include "../engine_p.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QtEndian>

#include <private/qstringiterator_p.h>
#include <private/qv4engine_p.h>
#include <private/qv4jsonobject_p.h>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(BufferCtor);
DEFINE_OBJECT_VTABLE(Buffer);

/// NOTE: This is a result of V8 memory allocation limitations. It would make sense to lift
/// such limit in the future.
namespace {
const size_t kMaxLength = 0x3fffffff;
}

/// TODO: Document no buf.parent property support (see test-buffer.js)

Heap::Buffer::Buffer(QV4::ExecutionEngine *v4, size_t length) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass, EnginePrivate::get(v4)->bufferPrototype.asObject())
{
    setVTable(NodeQml::Buffer::staticVTable());

    if (length > kMaxLength) {
        v4->throwRangeError(QStringLiteral("Attempt to allocate Buffer larger than maximum size: 0x3fffffff bytes"));
        return;
    }

    if (!allocateData(length)) {
        v4->throwRangeError(QStringLiteral("Buffer: Out of memory"));
        return;
    }

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(length));
}

Heap::Buffer::Buffer(QV4::ExecutionEngine *v4, QV4::ArrayObject *array) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass, EnginePrivate::get(v4)->bufferPrototype.asObject())
{
    setVTable(NodeQml::Buffer::staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedArrayObject a(scope, array);
    QV4::ScopedValue v(scope);

    const uint length = a->getLength();

    if (length > kMaxLength) {
        v4->throwRangeError(QStringLiteral("Attempt to allocate Buffer larger than maximum size: 0x3fffffff bytes"));
        return;
    }

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

Heap::Buffer::Buffer(QV4::ExecutionEngine *v4, const QByteArray &ba) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass, EnginePrivate::get(v4)->bufferPrototype.asObject())
{
    setVTable(NodeQml::Buffer::staticVTable());

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

Heap::Buffer::Buffer(QV4::ExecutionEngine *v4, const QTypedArrayDataSlice<char> &slice) :
    QV4::Heap::Object(EnginePrivate::get(v4)->bufferClass, EnginePrivate::get(v4)->bufferPrototype.asObject()),
    data(slice)
{
    setVTable(NodeQml::Buffer::staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope, this);
    o->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(data.size()));
}

bool Heap::Buffer::allocateData(size_t length)
{
    if (!length)
        return true;

    /// TODO: Check if +1 is actually needed
    QTypedArrayData<char> *arrayData = QTypedArrayData<char>::allocate(length + 1);
    if (!arrayData)
        return false;
    arrayData->size = length;
    arrayData->data()[length] = 0;

    data.setData(arrayData);
    arrayData->ref.deref(); // Disown data
    return true;
}

bool Buffer::isEqualTo(QV4::Managed *m, QV4::Managed *other)
{
    QV4::Scope scope(m->engine());
    QV4::Scoped<Buffer> self(scope, m);
    QV4::Scoped<Buffer> otherBuffer(scope, other);

    return !!self && !!otherBuffer
            && (BufferPrototype::compare(self->d()->data, otherBuffer->d()->data) == 0);
}

QV4::ReturnedValue Buffer::getIndexed(QV4::Managed *m, quint32 index, bool *hasProperty)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<Buffer> that(scope, static_cast<Buffer *>(m));

    if (index >= static_cast<quint32>(that->d()->data.size())) {
        if (hasProperty)
            *hasProperty = false;
        return QV4::Encode::undefined();
    }

    if (hasProperty)
        *hasProperty = true;

    return QV4::Primitive::fromUInt32(that->d()->data.at(index) & 0xff).asReturnedValue();
}

void Buffer::putIndexed(QV4::Managed *m, uint index, const QV4::ValueRef value)
{
    QV4::ExecutionEngine *v4 = m->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<Buffer> that(scope, static_cast<Buffer *>(m));

    if (index >= static_cast<quint32>(that->d()->data.size()))
        return;

    that->d()->data[index] = value->toInt32();
}

bool Buffer::deleteIndexedProperty(QV4::Managed *m, uint index)
{
    Q_UNUSED(m)
    Q_UNUSED(index)
    return true;
}

BufferEncoding Buffer::parseEncoding(const QString &str)
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

bool Buffer::isEncoding(const QString &str)
{
    return parseEncoding(str) != BufferEncoding::Invalid;
}

/*!
  \internal
  Returns number of bytes in a given binary array. \l BufferEncoding::Utf8 is assumed if \a encoding
  is \c Invalid or unknown.
 */
int Buffer::byteLength(const QString &str, BufferEncoding encoding)
{
    switch (encoding) {
    case BufferEncoding::Ascii:
    case BufferEncoding::Binary:
    case BufferEncoding::Raw:
        return str.toLatin1().size();
    case BufferEncoding::Base64:
        if (str.contains(QLatin1Char('+')) || str.contains(QLatin1Char('/')))
            return QByteArray::fromBase64(str.toUtf8()).size();
        else
            return QByteArray::fromBase64(str.toUtf8(), QByteArray::Base64UrlEncoding).size();
    case BufferEncoding::Hex:
        return str.size() >> 1;
    case BufferEncoding::Ucs2:
    case BufferEncoding::Utf16le:
        return str.size() * 2;
    case BufferEncoding::Utf8:
    case BufferEncoding::Invalid:
    default:
        return str.toUtf8().size();
    }
}

QByteArray Buffer::decodeString(const QString &str, BufferEncoding encoding, int limit)
{
    QByteArray ba;

    switch (encoding) {
    case BufferEncoding::Ascii:
    case BufferEncoding::Binary:
    case BufferEncoding::Raw:
        ba.resize(limit > -1 ? std::min(str.size(), limit) : str.size());
        for (int i = 0; i < ba.size(); ++i)
            ba[i] = str[i].cell();
        break;
    case BufferEncoding::Base64:
        if (str.contains(QLatin1Char('+')) || str.contains(QLatin1Char('/')))
            ba = QByteArray::fromBase64(str.toUtf8());
        else
            ba = QByteArray::fromBase64(str.toUtf8(), QByteArray::Base64UrlEncoding);
        if (limit > -1 && limit < ba.size())
            ba.resize(limit);
        break;
    case BufferEncoding::Hex:
        ba = QByteArray::fromHex(str.toUtf8());
        if (limit > -1 && limit < ba.size())
            ba.resize(limit);
        break;
    case BufferEncoding::Ucs2:
    case BufferEncoding::Utf16le: {
        int charCount = (limit > -1 && limit / 2 < str.size()) ? limit / 2 : str.size();
        ba.resize(charCount << 1);
        for (int i = 0; i < charCount; ++i) {
            ba[2 * i] = str[i].cell();
            ba[2 * i + 1] = str[i].row();
        }
        break;
    }
    case BufferEncoding::Utf8:
    case BufferEncoding::Invalid:
    default:  {
        // Worst case scenario
        ba.reserve(str.length() * 3);
        QStringIterator it(str);
        if (limit > -1) {
            while (it.hasNext()) {
                const uint ch = it.next();
                const QByteArray chUtf8 = QString::fromUcs4(&ch, 1).toUtf8();
                if (ba.size() + chUtf8.size() > limit)
                    break;
                ba += chUtf8;
            }
        } else {
            while (it.hasNext()) {
                const uint ch = it.next();
                ba += QString::fromUcs4(&ch, 1).toUtf8();
            }
        }
        ba.squeeze();
    }
    }

    return ba;
}

QTypedArrayData<char> *Buffer::fromString(const QByteArray &data)
{
    QTypedArrayData<char> *arrayData = QTypedArrayData<char>::allocate(data.size() + 1);
    if (!arrayData)
        return nullptr;

    ::memcpy(arrayData->data(), data.constData(), data.size());
    arrayData->size = data.size();
    arrayData->data()[data.size()] = 0;

    return arrayData;
}

Heap::BufferCtor::BufferCtor(QV4::ExecutionContext *scope) :
    QV4::Heap::FunctionObject(scope, QStringLiteral("Buffer"))
{
    setVTable(NodeQml::BufferCtor::staticVTable());
}

QV4::ReturnedValue BufferCtor::construct(QV4::Managed *m, QV4::CallData *callData)
{
    QV4::ExecutionEngine *v4 = m->engine();
    if (!callData->argc)
        return v4->throwTypeError(QStringLiteral("must start with number, buffer, array or string"));

    QV4::Scope scope(v4);
    QV4::Scoped<Buffer> buffer(scope);

    if (callData->args[0].isNumber()) {
        buffer = v4->memoryManager->alloc<Buffer>(v4, std::max(0., callData->args[0].toInteger()));
    } else if (callData->args[0].asArrayObject()) {
        buffer = v4->memoryManager->alloc<Buffer>(v4, callData->args[0].asArrayObject());
    } else if (callData->args[0].as<Buffer>()) {
        QV4::Scoped<Buffer> other(scope, callData->args[0].as<Buffer>());
        buffer = v4->memoryManager->alloc<Buffer>(v4, other->d()->data);
    } else if (callData->args[0].isString()) {
        BufferEncoding encoding = BufferEncoding::Utf8;
        if (callData->argc > 1) {
            if (!callData->args[1].isString())
                return v4->throwTypeError(QStringLiteral("Encoding must me a string"));

            const QString encStr = callData->args[1].toQString();
            encoding = Buffer::parseEncoding(encStr);
            if (encoding == BufferEncoding::Invalid)
                return v4->throwTypeError(QString("Unknown Encoding: %1").arg(encStr));
        }

        const QByteArray stringData
                = Buffer::decodeString(callData->args[0].toQString(), encoding);
        QTypedArrayData<char> *arrayData = Buffer::fromString(stringData);

        const QTypedArrayDataSlice<char> slice(arrayData);
        arrayData->ref.deref(); // Disown data
        buffer = v4->memoryManager->alloc<Buffer>(v4, slice);
    } else if (callData->args[0].isObject()) {
        QV4::ScopedObject obj(scope, callData->argument(0));
        QV4::ScopedString s(scope);

        // JSON-serialised Buffer
        QV4::ScopedString type(scope, obj->get(s = v4->newString(QStringLiteral("type"))));
        QV4::ScopedArrayObject data(scope, obj->get(s = v4->newString(QStringLiteral("data"))));
        if (!!type && type->toQString() == QStringLiteral("Buffer") && !!data) {
            buffer = v4->memoryManager->alloc<Buffer>(v4, data);
        } else {
            // Array-like objects
            buffer = v4->memoryManager->alloc<Buffer>(v4, obj->getLength());
            QV4::ScopedValue v(scope);
            /// FIXME: Should be buffer->putIndexed()
            for (size_t i = 0; i < obj->getLength(); ++i)
                Buffer::putIndexed(buffer, i, (v = obj->getIndexed(i)));
        }
    } else {
        return v4->throwTypeError(QStringLiteral("must start with number, buffer, array or string"));
    }

    // In case an exception has been thrown in the Buffer constructor
    if (v4->hasException)
        return QV4::Encode::undefined();

    return buffer.asReturnedValue();
}

QV4::ReturnedValue BufferCtor::call(QV4::Managed *that, QV4::CallData *callData)
{
    return construct(that, callData);
}

QV4::ReturnedValue BufferCtor::method_isEncoding(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    return QV4::Encode(callData->argc && callData->args[0].isString()
            && Buffer::isEncoding(callData->args[0].toQString()));
}

QV4::ReturnedValue BufferCtor::method_isBuffer(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    return QV4::Encode(callData->argc && callData->args[0].as<Buffer>());
}

QV4::ReturnedValue BufferCtor::method_byteLength(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);

    // Special case for 'undefined'
    if (!callData->argc)
        return QV4::Encode(9);

    const BufferEncoding encoding = callData->argc > 1
            ? Buffer::parseEncoding(callData->args[1].toQStringNoThrow())
            : BufferEncoding::Utf8;
    return QV4::Encode(Buffer::byteLength(callData->args[0].toQStringNoThrow(), encoding));
}

// Buffer.concat(list, [totalLength])
QV4::ReturnedValue BufferCtor::method_concat(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    QV4::Scope scope(v4);
    QV4::ScopedArrayObject list(scope, callData->argument(0));

    if (!list)
        return v4->throwTypeError(QStringLiteral("Usage: Buffer.concat(list[, length])"));

    size_t totalLength;
    if (callData->argc >= 2 && callData->args[1].isNumber()) {
        totalLength = callData->args[1].toNumber();
    } else {
        totalLength = 0;
        QV4::Scoped<Buffer> buf(scope);
        for (size_t i = 0; i < list->getLength(); ++i) {
            buf = list->getIndexed(i);
            if (!buf)
                return v4->throwTypeError(QStringLiteral("list must contain Buffer objects"));
            totalLength += buf->getLength();
        }
    }

    QV4::Scoped<Buffer> buffer(scope, v4->memoryManager->alloc<Buffer>(v4, totalLength));
    if (v4->hasException)
        return QV4::Encode::undefined();

    QV4::Scoped<Buffer> buf(scope);
    for (size_t i = 0, pos = 0; i < list->getLength(); ++i) {
        buf = list->getIndexed(i);
        const size_t bufSize = buf->getLength();

        ::memcpy(buffer->d()->data.data() + pos, buf->d()->data.constData(), bufSize);

        pos += bufSize;
    }

    return buffer.asReturnedValue();
}

QV4::ReturnedValue BufferCtor::method_compare(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_V4(ctx);

    QV4::Scope scope(ctx);
    QV4::Scoped<Buffer> a(scope, callData->argument(0));
    QV4::Scoped<Buffer> b(scope, callData->argument(1));

    if (!a || !b)
        return v4->throwTypeError(QStringLiteral("Arguments must be Buffers"));

    return QV4::Encode(BufferPrototype::compare(a->d()->data, b->d()->data));
}

void BufferPrototype::init(QV4::ExecutionEngine *v4, QV4::Object *ctor)
{
    QV4::Scope scope(v4);
    QV4::ScopedObject o(scope);

    ctor->defineReadonlyProperty(v4->id_length, QV4::Primitive::fromInt32(1));
    ctor->defineReadonlyProperty(v4->id_prototype, (o = this));
    defineDefaultProperty(QStringLiteral("constructor"), (o = ctor));

    ctor->defineDefaultProperty(QStringLiteral("isEncoding"), BufferCtor::method_isEncoding, 1);
    ctor->defineDefaultProperty(QStringLiteral("isBuffer"), BufferCtor::method_isBuffer, 1);
    ctor->defineDefaultProperty(QStringLiteral("byteLength"), BufferCtor::method_byteLength, 2);
    ctor->defineDefaultProperty(QStringLiteral("concat"), BufferCtor::method_concat, 2);
    ctor->defineDefaultProperty(QStringLiteral("compare"), BufferCtor::method_compare, 2);

    defineDefaultProperty(QStringLiteral("inspect"), method_inspect);

    defineDefaultProperty(QStringLiteral("compare"), method_compare, 1);
    defineDefaultProperty(QStringLiteral("equals"), method_equals, 1);

    defineDefaultProperty(QStringLiteral("copy"), method_copy, 4);
    defineDefaultProperty(QStringLiteral("fill"), method_fill, 3);
    defineDefaultProperty(QStringLiteral("slice"), method_slice, 2);
    defineDefaultProperty(QStringLiteral("write"), method_write, 4);
    defineDefaultProperty(QStringLiteral("toString"), method_toString, 3);
    defineDefaultProperty(QStringLiteral("toJSON"), method_toJSON);

    defineDefaultProperty(QStringLiteral("readInt8"), method_readInteger<qint8>);
    defineDefaultProperty(QStringLiteral("readUInt8"), method_readInteger<quint8>);
    defineDefaultProperty(QStringLiteral("readInt16LE"), method_readInteger<qint16>);
    defineDefaultProperty(QStringLiteral("readUInt16LE"), method_readInteger<quint16>);
    defineDefaultProperty(QStringLiteral("readInt16BE"), method_readInteger<qint16, false>);
    defineDefaultProperty(QStringLiteral("readUInt16BE"), method_readInteger<quint16, false>);
    defineDefaultProperty(QStringLiteral("readInt32LE"), method_readInteger<qint32>);
    defineDefaultProperty(QStringLiteral("readUInt32LE"), method_readInteger<quint32>);
    defineDefaultProperty(QStringLiteral("readInt32BE"), method_readInteger<qint32, false>);
    defineDefaultProperty(QStringLiteral("readUInt32BE"), method_readInteger<quint32, false>);

    defineDefaultProperty(QStringLiteral("readFloatLE"), method_readFloatingPoint<float>);
    defineDefaultProperty(QStringLiteral("readFloatBE"), method_readFloatingPoint<float, false>);
    defineDefaultProperty(QStringLiteral("readDoubleLE"), method_readFloatingPoint<double>);
    defineDefaultProperty(QStringLiteral("readDoubleBE"), method_readFloatingPoint<double, false>);

    defineDefaultProperty(QStringLiteral("writeInt8"), method_writeInteger<qint8>);
    defineDefaultProperty(QStringLiteral("writeUInt8"), method_writeInteger<quint8>);
    defineDefaultProperty(QStringLiteral("writeInt16LE"), method_writeInteger<qint16>);
    defineDefaultProperty(QStringLiteral("writeUInt16LE"), method_writeInteger<quint16>);
    defineDefaultProperty(QStringLiteral("writeInt16BE"), method_writeInteger<qint16, false>);
    defineDefaultProperty(QStringLiteral("writeUInt16BE"), method_writeInteger<quint16, false>);
    defineDefaultProperty(QStringLiteral("writeInt32LE"), method_writeInteger<qint32>);
    defineDefaultProperty(QStringLiteral("writeUInt32LE"), method_writeInteger<quint32>);
    defineDefaultProperty(QStringLiteral("writeInt32BE"), method_writeInteger<qint32, false>);
    defineDefaultProperty(QStringLiteral("writeUInt32BE"), method_writeInteger<quint32, false>);

    defineDefaultProperty(QStringLiteral("writeFloatLE"), method_writeFloatingPoint<float>);
    defineDefaultProperty(QStringLiteral("writeFloatBE"), method_writeFloatingPoint<float, false>);
    defineDefaultProperty(QStringLiteral("writeDoubleLE"), method_writeFloatingPoint<double>);
    defineDefaultProperty(QStringLiteral("writeDoubleBE"), method_writeFloatingPoint<double, false>);

    v4->globalObject()->defineDefaultProperty(QStringLiteral("Buffer"), (o = ctor));
    v4->globalObject()->defineDefaultProperty(QStringLiteral("SlowBuffer"), (o = ctor));
}

int BufferPrototype::compare(const QTypedArrayDataSlice<char> &a, const QTypedArrayDataSlice<char> &b)
{
    const size_t aSize = a.size();
    const size_t bSize = b.size();

    const int result = ::memcmp(a.constData(), b.constData(), std::min(aSize, bSize));
    if (result)
        return QV4::Encode(result > 0 ? 1 : -1);

    if (aSize > bSize)
        return 1;
    else if (aSize < bSize)
        return -1;

    return 0;
}

bool BufferPrototype::checkRange(size_t bufferSize, size_t offset, size_t length)
{
    return offset < kMaxLength
            && length < kMaxLength
            && offset + length < kMaxLength
            && offset + length <= bufferSize;
}

QV4::ReturnedValue BufferPrototype::method_inspect(QV4::CallContext *ctx)
{
    NODE_CTX_SELF(Buffer, ctx);
    NODE_CTX_V4(ctx);

    /// TODO: make it module variable, once 'buffer' becomes a normal module
    const int INSPECT_MAX_BYTES = 50;

    const QByteArray data
            = QByteArray::fromRawData(self->d()->data.data(),
                                      std::min(INSPECT_MAX_BYTES, self->d()->data.size()));
    const QString hex = data.toHex();
    QString bytes;
    int i = 0;
    while (i < hex.length()) {
        if (i > 0)
            bytes += QStringLiteral(" ");
        bytes += hex[i];
        bytes += hex[i + 1];
        i += 2;
    }

    return v4->newString(QString("<Buffer %1>").arg(bytes))->asReturnedValue();
}

QV4::ReturnedValue BufferPrototype::method_compare(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);
    NODE_CTX_V4(ctx);

    QV4::Scoped<Buffer> other(scope, callData->argument(0));

    if (!self || !other)
        return v4->throwTypeError(QStringLiteral("Arguments must be Buffers"));

    return QV4::Encode(BufferPrototype::compare(self->d()->data, other->d()->data));
}

QV4::ReturnedValue BufferPrototype::method_equals(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);
    NODE_CTX_V4(ctx);

    QV4::Scoped<Buffer> other(scope, callData->argument(0));

    if (!self || !other)
        return v4->throwTypeError(QStringLiteral("Arguments must be Buffers"));

    return QV4::Encode(BufferPrototype::compare(self->d()->data, other->d()->data) == 0);
}

/// TODO: Move somewhere
inline bool isFinite(const QV4::Value &value)
{
    return value.integerCompatible() || std::isfinite(value.toNumber());
}

// buf.write(string, [offset], [length], [encoding])
QV4::ReturnedValue BufferPrototype::method_write(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);
    NODE_CTX_V4(ctx);

    if (!callData->argc || !callData->args[0].isString())
        return v4->throwTypeError(QStringLiteral("Argument must be a string"));

    const QString string = callData->args[0].toQString();

    // Buffer#write(string);
    BufferEncoding encoding = BufferEncoding::Utf8;
    QString encodingStr;
    int length = self->d()->data.size();
    int offset = 0;

    // Buffer#write(string, encoding)
    if (callData->argc == 2 && callData->args[1].isString()) {
        encodingStr = callData->args[1].toQString();
    // Buffer#write(string, offset[, length][, encoding])
    } else if (callData->argc > 1 && isFinite(callData->args[1])) {
        offset = callData->args[1].toInt32();

        if (callData->argc > 2) {
            if (isFinite(callData->args[2])) {
                length = callData->args[2].toInt32();
                if (callData->argc > 3)
                    encodingStr = callData->args[3].toQStringNoThrow();
            } else {
                encodingStr = callData->args[2].toQStringNoThrow();
            }
        }
    }

    if (length < 0 || offset < 0 || offset >= self->d()->data.size())
        return v4->throwRangeError(QStringLiteral("attempt to write outside buffer bounds"));

    if (!encodingStr.isEmpty())
        encoding = Buffer::parseEncoding(encodingStr);

    if (encoding == BufferEncoding::Invalid)
        return v4->throwTypeError(QString("Unknown encoding: %1").arg(encodingStr));

    if (string.isEmpty())
        return QV4::Encode(0);

    if (offset + length > self->d()->data.size())
        length = self->d()->data.size() - offset;

    const QByteArray stringData = Buffer::decodeString(string, encoding, length);

    ::memcpy(self->d()->data.data() + offset, stringData.constData(), stringData.size());

    return QV4::Encode(stringData.size());
}

// toString([encoding], [start], [end])
QV4::ReturnedValue BufferPrototype::method_toString(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);
    NODE_CTX_V4(ctx);

    BufferEncoding encoding = BufferEncoding::Utf8;
    if (callData->argc) {
        const QString encodingStr = callData->args[0].toQStringNoThrow();
        encoding = Buffer::parseEncoding(encodingStr);
        if (encoding == BufferEncoding::Invalid)
            return v4->throwTypeError(QString("Unknown encoding: %1").arg(encodingStr));
    }

    const int dataSize = self->d()->data.size();
    int start = callData->argc > 1
            ? std::min(callData->args[1].toInt32(), dataSize) : 0;
    int end = callData->argc > 2 && !callData->args[2].isUndefined()
            ? std::min(callData->args[2].toInt32(), dataSize) : dataSize;

    if (start < 0)
        start = 0;
    if (end > dataSize)
        end = dataSize;
    if (end <= start)
        return QV4::ScopedString(scope, v4->newString()).asReturnedValue();

    const char *startPtr = self->d()->data.data() + start;
    const int size = end - start;

    const QByteArray data = QByteArray::fromRawData(startPtr, size);
    QString str;

    switch (encoding) {
    case BufferEncoding::Ascii:
    case BufferEncoding::Binary:
    case BufferEncoding::Raw: {
        // Node.js just masks off the highest bit
        for (int i = 0; i < data.size(); ++i)
            str.append(data[i] & 0x7f);
        break;
    }
    case BufferEncoding::Base64:
        str = data.toBase64();
        break;
    case BufferEncoding::Hex:
        str = data.toHex();
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

    return v4->newString(str)->asReturnedValue();
}

QV4::ReturnedValue BufferPrototype::method_toJSON(QV4::CallContext *ctx)
{
    NODE_CTX_SELF(Buffer, ctx);
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
    NODE_CTX_SELF(Buffer, ctx);
    NODE_CTX_V4(ctx);

    QV4::Scoped<Buffer> target(scope, callData->argument(0));
    if (!target)
        return v4->throwTypeError(QStringLiteral("copy: First arg should be a Buffer"));

    size_t targetStart = 0;
    size_t sourceStart = 0;
    size_t sourceEnd = self->getLength();

    if (callData->argc > 1) {
        if (!callData->args[1].isNumber())
            return v4->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[1].toInt32() < 0)
            return v4->throwRangeError(QStringLiteral("Out of range index"));

        targetStart = callData->args[1].toInt32();
    }

    if (callData->argc > 2) {
        if (!callData->args[2].isNumber())
            return v4->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[2].toInt32() < 0)
            return v4->throwRangeError(QStringLiteral("Out of range index"));
        sourceStart = callData->args[2].toInt32();
    }

    if (callData->argc > 3) {
        if (!callData->args[3].isNumber())
            return v4->throwTypeError(QStringLiteral("Bad argument"));
        if (callData->args[3].toInt32() < 0)
            return v4->throwRangeError(QStringLiteral("Out of range index"));
        sourceEnd = callData->args[3].toInt32();
    }

    // Copy zero bytes, we're done
    if (targetStart >= target->getLength() || sourceStart >= sourceEnd)
        return QV4::Encode(0);

    if (sourceStart > self->getLength())
        return v4->throwRangeError(QStringLiteral("copy: Out of range index"));

    const size_t targetLength = target->getLength();
    if (sourceEnd - sourceStart > targetLength - targetStart)
        sourceEnd = sourceStart + targetLength - targetStart;
    size_t to_copy = std::min(std::min(sourceEnd - sourceStart, targetLength - targetStart),
                          self->getLength() - sourceStart);

    ::memmove(target->d()->data.data() + targetStart, self->d()->data.constData() + sourceStart, to_copy);

    return QV4::Primitive::fromUInt32(to_copy).asReturnedValue();
}

// fill(value, [offset], [end])
QV4::ReturnedValue BufferPrototype::method_fill(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);
    NODE_CTX_V4(ctx);

    /// TODO: SLICE_START_END (https://github.com/joyent/node/blob/master/src/node_buffer.cc#L52)

    int offset = 0;
    int end = self->d()->data.size();

    if (!callData->argc)
        return self.asReturnedValue();

    if (callData->argc > 1) {
        if (!callData->args[1].isNumber())
            return v4->throwTypeError(QStringLiteral("Bad argument"));
        offset = callData->args[1].toInt32();
        if (offset < 0)
            return v4->throwRangeError(QStringLiteral("Out of range index"));
    }

    if (callData->argc > 2) {
        if (!callData->args[2].isNumber())
            return v4->throwTypeError(QStringLiteral("Bad argument"));
        end = callData->args[2].toInt32();
        if (end < 0)
            return v4->throwRangeError(QStringLiteral("Out of range index"));
    }

    const int length = end - offset;
    char * const startPtr = self->d()->data.data() + offset;

    if (callData->args[0].isNumber()) {
        const quint8 value = callData->args[0].toUInt32() & 0xff;
        ::memset(startPtr, value, length);
        return self.asReturnedValue();
    }

    if (!callData->args[0].isString())
        return v4->throwTypeError(QStringLiteral("fill: value is not a number"));

    const QByteArray value = callData->args[0].toQString().toUtf8();

    // optimize single ascii character case
    if (value.size() == 1) {
        ::memset(startPtr, value.at(0), length);
        return self.asReturnedValue();
    }

    int in_there = value.size();
    char * ptr = startPtr + value.size();
    ::memcpy(startPtr, value.constData(), std::min(value.size(), length));
    if (value.size() >= length)
        return self.asReturnedValue();

    while (in_there < length - in_there) {
        ::memcpy(ptr, startPtr, in_there);
        ptr += in_there;
        in_there *= 2;
    }

    if (in_there < length)
        ::memcpy(ptr, startPtr, length - in_there);

    return self.asReturnedValue();
}

// slice([start], [end])
QV4::ReturnedValue BufferPrototype::method_slice(QV4::CallContext *ctx)
{
    NODE_CTX_V4(ctx);
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);

    if (!self)
        return v4->throwTypeError();

    const int dataSize = self->d()->data.size();

    int start = callData->argc > 0 ? callData->args[0].toInt32() : 0;
    int end = callData->argc < 2 || callData->args[1].isUndefined()
            ? dataSize : callData->args[1].toInt32();

    if (start < 0)
        start = std::max(dataSize + start, 0);
    else if (start > dataSize)
        start = dataSize;

    if (end < 0)
        end = std::max(dataSize + end, 0);
    else if (end > dataSize)
        end = dataSize;

    if (end < start)
        end = start;

    QTypedArrayDataSlice<char> slice(self->d()->data, start, end - start);
    QV4::Scoped<Buffer> newBuffer(scope, v4->memoryManager->alloc<Buffer>(v4, slice));
    return newBuffer.asReturnedValue();
}

template <typename T, bool LE>
QV4::ReturnedValue BufferPrototype::method_readInteger(QV4::CallContext *ctx)
{
    NODE_CTX_V4(ctx);
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);

    if (!self)
        return v4->throwTypeError();

    const size_t offset = callData->argc && callData->args[0].isNumber()
            ? callData->args[0].toNumber() : 0;

    if (!checkRange(self->d()->data.size(), offset, sizeof(T)))
        return v4->throwRangeError(QStringLiteral("index out of range"));

    const uchar *data = reinterpret_cast<const uchar *>(self->d()->data.constData() + offset);

    if (LE)
        return QV4::Encode(qFromLittleEndian<T>(data));
    else
        return QV4::Encode(qFromBigEndian<T>(data));
}

template <typename T, bool LE>
QV4::ReturnedValue BufferPrototype::method_readFloatingPoint(QV4::CallContext *ctx)
{
    NODE_CTX_V4(ctx);
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);

    if (!self)
        return v4->throwTypeError();

    const size_t offset = callData->argc && callData->args[0].isNumber()
            ? callData->args[0].toNumber() : 0;

    if (!checkRange(self->d()->data.size(), offset, sizeof(T)))
        return v4->throwRangeError(QStringLiteral("index out of range"));

    const uchar *data = reinterpret_cast<const uchar *>(self->d()->data.constData() + offset);

    /// NOTE: Workaround for missing float and double support in QtEndian
    if (sizeof(T) == 4) {
        quint32 value = LE ? qFromLittleEndian<quint32>(data) : qFromBigEndian<quint32>(data);
        return QV4::Encode(*reinterpret_cast<float *>(&value));
    } else {
        quint64 value = LE ? qFromLittleEndian<quint64>(data) : qFromBigEndian<quint64>(data);
        return QV4::Encode(*reinterpret_cast<double *>(&value));
    }
}

template <typename T, bool LE>
QV4::ReturnedValue BufferPrototype::method_writeInteger(QV4::CallContext *ctx)
{
    NODE_CTX_V4(ctx);
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);

    if (!self)
        return v4->throwTypeError();

    // Node.js assumes that the value is 0 if nothing is specified
    /// TODO: Probably it's better to throw an error
    /// FIXME: QV4::Value::isInteger() should be used instead of isNumber()
    const double value = (callData->argc && callData->args[0].isNumber())
            ? callData->args[0].toNumber()
            : 0;

    if (value > std::numeric_limits<T>::max() || value < std::numeric_limits<T>::min())
        return v4->throwRangeError(QStringLiteral("value is out of bounds"));

    const size_t offset = callData->argc > 1 && callData->args[1].isNumber()
            ? callData->args[1].toNumber() : 0;

    if (!checkRange(self->d()->data.size(), offset, sizeof(T)))
        return v4->throwRangeError(QStringLiteral("index out of range"));

    const T src = static_cast<T>(value);
    uchar *dst = reinterpret_cast<uchar *>(self->d()->data.data() + offset);

    if (LE)
        qToLittleEndian<T>(src, dst);
    else
        qToBigEndian<T>(src, dst);

    return QV4::Encode(static_cast<uint>(offset + sizeof(T)));
}

template <typename T, bool LE>
QV4::ReturnedValue BufferPrototype::method_writeFloatingPoint(QV4::CallContext *ctx)
{
    NODE_CTX_V4(ctx);
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(Buffer, ctx);

    if (!self)
        return v4->throwTypeError();

    // Node.js assumes that the value is 0 if nothing is specified
    /// TODO: Probably it's better to throw an error
    const double value = (callData->argc && callData->args[0].isNumber())
            ? callData->args[0].toNumber()
            : 0;

    if (value < std::numeric_limits<T>::lowest() || value > std::numeric_limits<T>::max())
        return v4->throwRangeError(QStringLiteral("value is out of bounds"));

    const size_t offset = callData->argc > 1 && callData->args[1].isNumber()
            ? callData->args[1].toNumber() : 0;

    if (!checkRange(self->d()->data.size(), offset, sizeof(T)))
        return v4->throwRangeError(QStringLiteral("index out of range"));

    const T src = static_cast<T>(value);
    uchar *dst = reinterpret_cast<uchar *>(self->d()->data.data() + offset);

    /// NOTE: Workaround for missing float and double support in QtEndian
    if (sizeof(T) == 4) {
        if (LE)
            qToLittleEndian<quint32>(*reinterpret_cast<const quint32 *>(&src), dst);
        else
            qToBigEndian<quint32>(*reinterpret_cast<const quint32 *>(&src), dst);
    } else {
        if (LE)
            qToLittleEndian<quint64>(*reinterpret_cast<const quint64 *>(&src), dst);
        else
            qToBigEndian<quint64>(*reinterpret_cast<const quint64 *>(&src), dst);
    }

    return QV4::Encode(static_cast<uint>(offset + sizeof(T)));
}

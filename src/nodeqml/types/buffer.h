#ifndef BUFFER_H
#define BUFFER_H

#include "../v4integration.h"
#include "../util/qarraydataslice.h"

#include <private/qv4object_p.h>
#include <private/qv4functionobject_p.h>

namespace NodeQml {

enum class BufferEncoding {
    Invalid,
    Ascii,
    Base64,
    Binary,
    Hex,
    Raw,
    Ucs2,
    Utf8,
    Utf16le
};

namespace Heap {

struct BufferObject : QV4::Heap::Object {
    BufferObject(QV4::ExecutionEngine *v4, size_t length);
    BufferObject(QV4::ExecutionEngine *v4, QV4::ArrayObject *array);
    BufferObject(QV4::ExecutionEngine *v4, const QByteArray &ba);
    BufferObject(QV4::ExecutionEngine *v4, const QTypedArrayDataSlice<char> &slice);
    bool allocateData(size_t length);

    QTypedArrayDataSlice<char> data;
};

struct BufferCtor : QV4::Heap::FunctionObject {
    BufferCtor(QV4::ExecutionContext *scope);
};

} // namespace Heap

struct BufferObject : QV4::Object
{
    NODE_V4_OBJECT(BufferObject, Object)
    V4_NEEDS_DESTROY

    static QV4::ReturnedValue getIndexed(QV4::Managed *m, quint32 index, bool *hasProperty);
    static void putIndexed(QV4::Managed *m, uint index, const QV4::ValueRef value);
    static bool deleteIndexedProperty(QV4::Managed *m, uint index);

    static BufferEncoding parseEncoding(const QString &str);
    static bool isEncoding(const QString &str);
    static int byteLength(const QString &str, BufferEncoding encoding);
    static QByteArray decodeString(const QString &str, BufferEncoding encoding, int limit = -1);
    static QTypedArrayData<char> *fromString(const QByteArray &data);
};

struct BufferCtor : QV4::FunctionObject
{
    NODE_V4_OBJECT(BufferCtor, FunctionObject)

    static QV4::ReturnedValue construct(QV4::Managed *m, QV4::CallData *callData);
    static QV4::ReturnedValue call(QV4::Managed *that, QV4::CallData *callData);
};

struct BufferPrototype : QV4::Object
{
    void init(QV4::ExecutionEngine *v4, QV4::Object *ctor);

    static QV4::ReturnedValue method_isEncoding(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_isBuffer(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_byteLength(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_concat(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_inspect(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_write(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_toString(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_toJSON(QV4::CallContext *ctx);

    static QV4::ReturnedValue method_copy(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_fill(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_slice(QV4::CallContext *ctx);
    /// TODO: buf.readUInt8(offset, [noAssert])
    /// TODO: buf.readUInt16LE(offset, [noAssert])
    /// TODO: buf.readUInt16BE(offset, [noAssert])
    /// TODO: buf.readUInt32LE(offset, [noAssert])
    /// TODO: buf.readUInt32BE(offset, [noAssert])
    /// TODO: buf.readInt8(offset, [noAssert])
    /// TODO: buf.readInt16LE(offset, [noAssert])
    /// TODO: buf.readInt16BE(offset, [noAssert])
    /// TODO: buf.readInt32LE(offset, [noAssert])
    /// TODO: buf.readInt32BE(offset, [noAssert])
    /// TODO: buf.readFloatLE(offset, [noAssert])
    /// TODO: buf.readFloatBE(offset, [noAssert])
    /// TODO: buf.readDoubleLE(offset, [noAssert])
    /// TODO: buf.readDoubleBE(offset, [noAssert])
    /// TODO: buf.writeUInt8(value, offset, [noAssert])
    /// TODO: buf.writeUInt16LE(value, offset, [noAssert])
    /// TODO: buf.writeUInt16BE(value, offset, [noAssert])
    /// TODO: buf.writeUInt32LE(value, offset, [noAssert])
    /// TODO: buf.writeUInt32BE(value, offset, [noAssert])
    /// TODO: buf.writeInt8(value, offset, [noAssert])
    /// TODO: buf.writeInt16LE(value, offset, [noAssert])
    /// TODO: buf.writeInt16BE(value, offset, [noAssert])
    /// TODO: buf.writeInt32LE(value, offset, [noAssert])
    /// TODO: buf.writeInt32BE(value, offset, [noAssert])
    /// TODO: buf.writeFloatLE(value, offset, [noAssert])
    /// TODO: buf.writeFloatBE(value, offset, [noAssert])
    /// TODO: buf.writeDoubleLE(value, offset, [noAssert])
    /// TODO: buf.writeDoubleBE(value, offset, [noAssert])
};

} // namespace NodeQml

#endif // BUFFER_H

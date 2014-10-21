#ifndef BUFFER_H
#define BUFFER_H

#include <QByteArray>

#include <private/qv4object_p.h>
#include <private/qv4functionobject_p.h>

namespace NodeQml {

struct BufferObject: QV4::Object {
    struct Data : QV4::Object::Data {
        Data(QV4::InternalClass *ic);
        Data(QV4::ExecutionEngine *v4, quint32 size);
        Data(QV4::ExecutionEngine *v4, const QString &str, const QString &encoding);

        QByteArray value;
    };
    V4_OBJECT(Object)
    //Q_MANAGED_TYPE(BufferObject)

    static QV4::ReturnedValue getIndexed(QV4::Managed *m, quint32 index, bool *hasProperty);
    static void putIndexed(QV4::Managed *m, uint index, const QV4::ValueRef value);
    static bool deleteIndexedProperty(QV4::Managed *m, uint index);
};

struct BufferCtor: QV4::FunctionObject
{
    struct Data : QV4::FunctionObject::Data {
        Data(QV4::ExecutionContext *scope);
    };
    V4_OBJECT(FunctionObject)

    static QV4::ReturnedValue construct(QV4::Managed *m, QV4::CallData *callData);
    static QV4::ReturnedValue call(QV4::Managed *that, QV4::CallData *callData);
};

struct BufferPrototype: BufferObject
{
    void init(QV4::ExecutionEngine *v4, QV4::Object *ctor);
    static bool isEncoding(const QString &encoding);

    static QV4::ReturnedValue method_isEncoding(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_isBuffer(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_byteLength(QV4::CallContext *ctx);
    static QV4::ReturnedValue method_concat(QV4::CallContext *ctx);

    /// TODO: buf.write(string, [offset], [length], [encoding])
    /// TODO: buf.toString([encoding], [start], [end])
    /// TODO: buf.toJSON()

    /// TODO: buf.copy(targetBuffer, [targetStart], [sourceStart], [sourceEnd])
    /// TODO: buf.slice([start], [end])
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
    /// TODO: buf.fill(value, [offset], [end])    
};

} // namespace NodeQml

#endif // BUFFER_H

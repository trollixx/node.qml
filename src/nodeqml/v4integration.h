#ifndef V4INTEGRATION_H
#define V4INTEGRATION_H

#define NODE_V4_OBJECT(DataClass, superClass) \
    public: \
        Q_MANAGED_CHECK \
        typedef NodeQml::Heap::DataClass Data; \
        typedef superClass SuperClass; \
        static const QV4::ObjectVTable static_vtbl; \
        static inline const QV4::ManagedVTable *staticVTable() { return &static_vtbl.managedVTable; } \
        V4_MANAGED_SIZE_TEST \
        NodeQml::Heap::DataClass *d() const { return static_cast<NodeQml::Heap::DataClass *>(m); }

#define NODE_CTX_CALLDATA(Context) const QV4::CallData * const callData = Context->d()->callData;
#define NODE_CTX_V4(Context) QV4::ExecutionEngine * const v4 = Context->engine();

#define NODE_CTX_SELF(Class, Context) \
    QV4::Scope scope(Context->engine()); \
    QV4::Scoped<Class> self(scope, Context->d()->callData->thisObject);

#endif // V4INTEGRATION_H

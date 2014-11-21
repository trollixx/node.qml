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
        const NodeQml::Heap::DataClass *d() const { return &static_cast<const NodeQml::Heap::DataClass &>(Managed::data); } \
        NodeQml::Heap::DataClass *d() { return &static_cast<NodeQml::Heap::DataClass &>(Managed::data); }

#endif // V4INTEGRATION_H

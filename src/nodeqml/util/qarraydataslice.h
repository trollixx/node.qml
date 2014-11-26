#ifndef QARRAYDATASLICE_H
#define QARRAYDATASLICE_H

#include <QArrayData>
#include <QTypeInfo>
#include <QtAlgorithms>

template<typename T>
class QTypedArrayDataSlice
{
public:
    explicit QTypedArrayDataSlice() {}
    explicit QTypedArrayDataSlice(QTypedArrayData<T> *arrayData, int offset = 0, int size = -1);
    ~QTypedArrayDataSlice();

    QTypedArrayDataSlice &operator=(const QTypedArrayDataSlice&);

    bool isEmpty() const { return !m_size; }
    bool isNull() const { return !m_arrayData; }

    int size() const { return m_size; }

    inline T *data();
    inline const T *constData() const;

    const T &at(int i) const;
    T &operator[](int i);
    const T &operator[](int i) const;

    void clearData();
    void setData(QTypedArrayData<T> *arrayData, int offset = 0, int size = -1);

private:
    QArrayData *m_arrayData = nullptr;
    void *m_begin = nullptr;
    int m_size = 0;
};

template<typename T>
inline bool operator == (const QTypedArrayDataSlice<T> &a, const QTypedArrayDataSlice<T> &b)
{
    return a.size() == b.size() && !memcmp(a.data(), b.data(), a.size());
}

template<typename T>
QTypedArrayDataSlice<T>::QTypedArrayDataSlice(QTypedArrayData<T> *arrayData, int offset, int size)
{
    if (!arrayData)
        return;
    setData(arrayData, offset, size);
}

template<typename T>
QTypedArrayDataSlice<T>::~QTypedArrayDataSlice()
{
    clearData();
}

template<typename T>
QTypedArrayDataSlice<T> &QTypedArrayDataSlice<T>::operator=(const QTypedArrayDataSlice &other)
{
    clearData();
    *this = other;
    if (m_arrayData)
        m_arrayData->ref.ref();
    return *this;
}

template<typename T>
T *QTypedArrayDataSlice<T>::data()
{
    return static_cast<T *>(m_begin);
}

template<typename T>
const T *QTypedArrayDataSlice<T>::constData() const
{
    return static_cast<const T *>(m_begin);
}

template<typename T>
inline const T &QTypedArrayDataSlice<T>::at(int i) const
{
    Q_ASSERT_X(i >= 0 && i < size(), "QTypedArrayDataSlice<T>::at", "index out of range");
    return constData()[i];
}

template<typename T>
T &QTypedArrayDataSlice<T>::operator[](int i)
{
    Q_ASSERT_X(i >= 0 && i < size(), "QTypedArrayDataSlice<T>::operator[]", "index out of range");
    return data()[i];
}

template<typename T>
const T &QTypedArrayDataSlice<T>::operator[](int i) const
{
    Q_ASSERT_X(i >= 0 && i < size(), "QTypedArrayDataSlice<T>::operator[]", "index out of range");
    return constData()[i];
}

template<typename T>
void QTypedArrayDataSlice<T>::clearData()
{
    if (!m_arrayData)
        return;

    if (m_arrayData->ref.deref()) {
        if (QTypeInfo<T>::isComplex) {
            for (int i = 0; i < size() ; ++i)
                at(i).~T();
        }
        QTypedArrayData<T>::deallocate(m_arrayData);
    }
    m_arrayData = nullptr;
    m_begin = nullptr;
    m_size = 0;
}

template<typename T>
void QTypedArrayDataSlice<T>::setData(QTypedArrayData<T> *arrayData, int offset, int size)
{
    Q_ASSERT(arrayData);
    Q_ASSERT_X(offset >= 0 && offset < arrayData->size,
               "QTypedArrayDataSlice<T>::setData", "offset out of range");
    Q_ASSERT_X(size == -1 || (size > 0 && offset + size <= arrayData->size),
               "QTypedArrayDataSlice<T>::setData", "size out of range");
    clearData();
    if (arrayData)
        arrayData->ref.ref();
    m_arrayData = arrayData;
    m_begin = m_arrayData + offset;
    m_size = size == -1 ? arrayData->size : size;
}

#endif // QARRAYDATASLICE_H

/**********************************************************************************
 * Copyright © 2015 Pavel A. Puchkov                                              *
 *                                                                                *
 * This file is part of IqOrm.                                                    *
 *                                                                                *
 * IqOrm is free software: you can redistribute it and/or modify                  *
 * it under the terms of the GNU Lesser General Public License as published by    *
 * the Free Software Foundation, either version 3 of the License, or              *
 * (at your option) any later version.                                            *
 *                                                                                *
 * IqOrm is distributed in the hope that it will be useful,                       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                 *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                  *
 * GNU Lesser General Public License for more details.                            *
 *                                                                                *
 * You should have received a copy of the GNU Lesser General Public License       *
 * along with IqOrm.  If not, see <http://www.gnu.org/licenses/>.                 *
 **********************************************************************************/

#ifndef IQORMLAZYSHAREDPOINTER_H
#define IQORMLAZYSHAREDPOINTER_H

#include <QSharedPointer>
#include "iqorm_global.h"


namespace IqOrmPrivate {

template <class T>
void qobjectDeleter(T* qObject)
{
    if (qObject)
        qObject->deleteLater();
}

template <class T>
    class IqOrmLazySharedPointerData : public QSharedData
    {
    public:
        IqOrmLazySharedPointerData();
        virtual ~IqOrmLazySharedPointerData();

        qint64 objectId;
        T *object;
        void (*deleter)(T *);

    private:
    };

    template<class T>
    IqOrmLazySharedPointerData<T>::IqOrmLazySharedPointerData() :
        objectId(-1),
        object(Q_NULLPTR),
        deleter(qobjectDeleter<T>)
    {
    }

    template <class T>
    IqOrmLazySharedPointerData<T>::~IqOrmLazySharedPointerData()
    {
        deleter(object);
    }
}

template <class T>
class IQORMSHARED_EXPORT IqOrmLazySharedPointer
{
public:
    IqOrmLazySharedPointer();

#ifdef Q_COMPILER_NULLPTR
    IqOrmLazySharedPointer(std::nullptr_t);
#endif

    IqOrmLazySharedPointer(const IqOrmLazySharedPointer<T> &other);

    explicit IqOrmLazySharedPointer(T *object);

    explicit IqOrmLazySharedPointer(const qint64 objectId);

    T* data() const;

    qint64 objectId() const;

    bool isNull() const;

    inline T* operator->() const
    { return data(); }
    inline T& operator*() const
    { return *data(); }
    inline operator T*() const
    { return data(); }

private:

    template <class U>
    friend bool operator==(const IqOrmLazySharedPointer<U> &ptr1, const IqOrmLazySharedPointer<U> &ptr2);

    template <class U>
    friend uint qHash(const IqOrmLazySharedPointer<U> &key, uint seed);

private:
    QExplicitlySharedDataPointer<IqOrmPrivate::IqOrmLazySharedPointerData<T> > d;
};

Q_DECLARE_SMART_POINTER_METATYPE(IqOrmLazySharedPointer)




template<class T>
IqOrmLazySharedPointer<T>::IqOrmLazySharedPointer() :
    d (new IqOrmPrivate::IqOrmLazySharedPointerData<T>)
{
}

#ifdef Q_COMPILER_NULLPTR
template<class T>
IqOrmLazySharedPointer<T>::IqOrmLazySharedPointer(std::nullptr_t) :
    d (new IqOrmPrivate::IqOrmLazySharedPointerData<T>)
{
}
#endif

template<class T>
IqOrmLazySharedPointer<T>::IqOrmLazySharedPointer(const IqOrmLazySharedPointer<T> &other) :
    d (other.d)
{
}

template<class T>
IqOrmLazySharedPointer<T>::IqOrmLazySharedPointer(T *object) :
    d (new IqOrmPrivate::IqOrmLazySharedPointerData<T>)
{
    if (object)
        object->setParent(Q_NULLPTR);

    d->object = object;
}

template<class T>
IqOrmLazySharedPointer<T>::IqOrmLazySharedPointer(const qint64 objectId) :
    d (new IqOrmPrivate::IqOrmLazySharedPointerData<T>)
{
    d->objectId = objectId;
}

template<class T>
T* IqOrmLazySharedPointer<T>::data() const
{
    if (d->object)
        return d->object;

    if (d->objectId != -1) {
        d->object = new T();
        d->object->load(d->objectId);
        return d->object;
    }

    return Q_NULLPTR;
}

template <class T>
qint64 IqOrmLazySharedPointer<T>::objectId() const
{
    if (d->object)
        return d->object->objectId();

    return d->objectId;
}

template <class T>
bool IqOrmLazySharedPointer<T>::isNull() const
{
    return !d->object && d->objectId == -1;
}





template<class T>
inline bool operator==(const IqOrmLazySharedPointer<T> &ptr1, const IqOrmLazySharedPointer<T> &ptr2)
{
    return ptr1.d->object == ptr2.d->object && ptr1.objectId() == ptr2.objectId();
}

template<class T>
inline bool operator!=(const IqOrmLazySharedPointer<T> &ptr1, const IqOrmLazySharedPointer<T> &ptr2)
{
    return !(ptr1 == ptr2);
}

template <class T>
inline uint qHash(const IqOrmLazySharedPointer<T> &key, uint seed)
{
    return qHash(key.objectId(), seed) ^ qHash(key.d->object);
}

#endif // IQORMLAZYSHAREDPOINTER_H

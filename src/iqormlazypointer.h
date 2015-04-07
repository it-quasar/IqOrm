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

#ifndef IQORMLAZYPOINTER_H
#define IQORMLAZYPOINTER_H

#include <QPointer>
#include "iqorm_global.h"

template <class T>
class IQORMSHARED_EXPORT IqOrmLazyPointer
{
public:
    IqOrmLazyPointer();

#ifdef Q_COMPILER_NULLPTR
    IqOrmLazyPointer(std::nullptr_t);
#endif

    explicit IqOrmLazyPointer(T *object);

    explicit IqOrmLazyPointer(const qint64 objectId);

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
    class IqOrmLazyPointerData : public QSharedData
    {
    public:
        IqOrmLazyPointerData();

        qint64 objectId;
        QPointer<T> object;
    };

    template <class U>
    friend bool operator==(const IqOrmLazyPointer<U> &ptr1, const IqOrmLazyPointer<U> &ptr2);

    template <class U>
    friend uint qHash(const IqOrmLazyPointer<U> &key, uint seed);

private:
    QExplicitlySharedDataPointer<IqOrmLazyPointerData> d;
};

Q_DECLARE_SMART_POINTER_METATYPE(IqOrmLazyPointer)




template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointerData::IqOrmLazyPointerData() :
    objectId(-1),
    object(Q_NULLPTR)
{
}

template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer() :
    d (new IqOrmLazyPointerData)
{
}

#ifdef Q_COMPILER_NULLPTR
template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer(std::nullptr_t) :
    d (new IqOrmLazyPointerData)
{
}
#endif

template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer(T *object) :
    d (new IqOrmLazyPointerData)
{
    Q_CHECK_PTR(d);

    d->object = object;
}


template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer(const qint64 objectId) :
    d (new IqOrmLazyPointerData)
{
    Q_CHECK_PTR(d);

    d->objectId = objectId;
}

template<class T>
T* IqOrmLazyPointer<T>::data() const
{
    Q_CHECK_PTR(d);

    if (d->object)
        return d->object.data();

    if (d->objectId != -1) {
        d->object = new T();
        d->object->load(d->objectId);
        return d->object.data();
    }

    return Q_NULLPTR;
}

template <class T>
qint64 IqOrmLazyPointer<T>::objectId() const
{
    Q_CHECK_PTR(d);

    if (d->object)
        return d->object->objectId();

    return d->objectId;
}

template <class T>
bool IqOrmLazyPointer<T>::isNull() const
{
    Q_CHECK_PTR(d);

    return !d->object && d->objectId == -1;
}




template<class T>
inline bool operator==(const IqOrmLazyPointer<T> &ptr1, const IqOrmLazyPointer<T> &ptr2)
{
    Q_CHECK_PTR(ptr1.d);
    Q_CHECK_PTR(ptr2.d);

    return ptr1.d->object == ptr2.d->object && ptr1.objectId() == ptr2.objectId();
}

template<class T>
inline bool operator!=(const IqOrmLazyPointer<T> &ptr1, const IqOrmLazyPointer<T> &ptr2)
{
    Q_CHECK_PTR(ptr1.d);
    Q_CHECK_PTR(ptr2.d);

    return !(ptr1 == ptr2);
}

template <class T>
inline uint qHash(const IqOrmLazyPointer<T> &key, uint seed)
{
    Q_CHECK_PTR(key.d);

    return qHash(key.objectId(), seed) ^ qHash(key.d->object.data());
}

#endif // IQORMLAZYPOINTER_H

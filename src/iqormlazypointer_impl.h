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
#error 'iqormlazypointer_impl.h' is not supposed to be included directly. Include 'iqormlazypointer.h' instead.
#endif

#include <QPointer>
#include "iqorm_global.h"

namespace IqOrmPrivate {

template <class T>
class IqOrmLazyPointerData : public QSharedData
{
public:
    IqOrmLazyPointerData();

    qint64 objectId;
    QPointer<T> object;
};

template<class T>
IqOrmLazyPointerData<T>::IqOrmLazyPointerData() :
    objectId(-1),
    object(Q_NULLPTR)
{
}

}

using namespace IqOrmPrivate;

template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer() :
    d (new IqOrmLazyPointerData<T>)
{
}

#ifdef Q_COMPILER_NULLPTR
template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer(std::nullptr_t) :
    d (new IqOrmLazyPointerData<T>)
{
}
#endif

template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer(T *object) :
    d (new IqOrmLazyPointerData<T>)
{
    Q_CHECK_PTR(d);

    d->object = object;
}


template<class T>
IqOrmLazyPointer<T>::IqOrmLazyPointer(const qint64 objectId) :
    d (new IqOrmLazyPointerData<T>)
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

template <class T>
const IqOrmMetaModel *IqOrmLazyPointer<T>::staticOrmMetaModel() const
{
    return T::staticOrmMetaModel();
}

template <class T>
T *IqOrmLazyPointer<T>::operator->() const
{
    return data();
}

template <class T>
T &IqOrmLazyPointer<T>::operator*() const
{
    return *data();
}

template <class T>
IqOrmLazyPointer<T>::operator T *() const
{
    return data();
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

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

#ifndef IQORMSHAREDLAZYPOINTER_H
#error 'iqormshardlazypointer_impl.h' is not supposed to be included directly. Include 'iqormsharedlazypointer.h' instead.
#endif

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
class IqOrmSharedLazyPointerData : public QSharedData
{
public:
    IqOrmSharedLazyPointerData();
    virtual ~IqOrmSharedLazyPointerData();

    qint64 objectId;
    T *object;
    void (*deleter)(T *);

private:
};

template<class T>
IqOrmSharedLazyPointerData<T>::IqOrmSharedLazyPointerData() :
    objectId(-1),
    object(Q_NULLPTR),
    deleter(qobjectDeleter<T>)
{
}

template <class T>
IqOrmSharedLazyPointerData<T>::~IqOrmSharedLazyPointerData()
{
    deleter(object);
}

}


using namespace IqOrmPrivate;

template<class T>
IqOrmSharedLazyPointer<T>::IqOrmSharedLazyPointer() :
    d (new IqOrmSharedLazyPointerData<T>)
{
}

#ifdef Q_COMPILER_NULLPTR
template<class T>
IqOrmSharedLazyPointer<T>::IqOrmSharedLazyPointer(std::nullptr_t) :
    d (new IqOrmSharedLazyPointerData<T>)
{
}
#endif

template<class T>
IqOrmSharedLazyPointer<T>::IqOrmSharedLazyPointer(T *object) :
    d (new IqOrmSharedLazyPointerData<T>)
{
    if (object)
        object->setParent(Q_NULLPTR);

    d->object = object;
}

template<class T>
IqOrmSharedLazyPointer<T>::IqOrmSharedLazyPointer(const qint64 objectId) :
    d (new IqOrmSharedLazyPointerData<T>)
{
    d->objectId = objectId;
}

template<class T>
T* IqOrmSharedLazyPointer<T>::data() const
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
qint64 IqOrmSharedLazyPointer<T>::objectId() const
{
    if (d->object)
        return d->object->objectId();

    return d->objectId;
}

template <class T>
bool IqOrmSharedLazyPointer<T>::isNull() const
{
    return !d->object && d->objectId == -1;
}


template <class T>
const IqOrmMetaModel *IqOrmSharedLazyPointer<T>::staticOrmMetaModel() const
{
    return T::staticOrmMetaModel();
}

template <class T>
T *IqOrmSharedLazyPointer<T>::operator->() const
{
    return data();
}

template <class T>
T &IqOrmSharedLazyPointer<T>::operator*() const
{
    return *data();
}

template <class T>
IqOrmSharedLazyPointer<T>::operator T *() const
{
    return data();
}



template<class T>
inline bool operator==(const IqOrmSharedLazyPointer<T> &ptr1, const IqOrmSharedLazyPointer<T> &ptr2)
{
    return ptr1.d->object == ptr2.d->object && ptr1.objectId() == ptr2.objectId();
}

template<class T>
inline bool operator!=(const IqOrmSharedLazyPointer<T> &ptr1, const IqOrmSharedLazyPointer<T> &ptr2)
{
    return !(ptr1 == ptr2);
}

template <class T>
inline uint qHash(const IqOrmSharedLazyPointer<T> &key, uint seed)
{
    return qHash(key.objectId(), seed) ^ qHash(key.d->object);
}

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
#define IQORMSHAREDLAZYPOINTER_H

#include <QMetaType>
#include <QExplicitlySharedDataPointer>
#include "iqorm_global.h"

class IqOrmMetaModel;

namespace IqOrmPrivate {
template <class T>
class IqOrmSharedLazyPointerData;
}

template <class T>
class IQORMSHARED_EXPORT IqOrmSharedLazyPointer
{
public:
    IqOrmSharedLazyPointer();

#ifdef Q_COMPILER_NULLPTR
    IqOrmSharedLazyPointer(std::nullptr_t);
#endif

    explicit IqOrmSharedLazyPointer(T *object);

    explicit IqOrmSharedLazyPointer(const qint64 objectId);

    T* data() const;

    qint64 objectId() const;

    bool isNull() const;

    const IqOrmMetaModel *staticOrmMetaModel() const;

    T * operator->() const;
    T & operator*() const;
    operator T*() const;

private:
    template <class U>
    friend bool operator==(const IqOrmSharedLazyPointer<U> &ptr1, const IqOrmSharedLazyPointer<U> &ptr2);

    template <class U>
    friend uint qHash(const IqOrmSharedLazyPointer<U> &key, uint seed);

private:
    QExplicitlySharedDataPointer<IqOrmPrivate::IqOrmSharedLazyPointerData<T> > d;
};


Q_DECLARE_SMART_POINTER_METATYPE(IqOrmSharedLazyPointer)

#include "iqormsharedlazypointer_impl.h"

#endif // IQORMSHAREDLAZYPOINTER_H

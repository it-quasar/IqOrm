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

#ifndef IQORMONEOBJECTDESCRIBINGPROPERTYDESCRIPTION_H
#define IQORMONEOBJECTDESCRIBINGPROPERTYDESCRIPTION_H

#include "iqormpropertydescription.h"
#include "iqormobjectprivateaccessor.h"
#include "iqormlazypointer.h"
#include "iqormlazysharedpointer.h"
#include "iqormdirectmappedpropertydescription.h"
#include "iqorminversedmappedpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmOneObjectDescribingPropertyDescription
{
public:
    virtual QVariant valueAsObjectId(const IqOrmObject *object) const = 0;

    virtual bool setValueFromObjectId(IqOrmObject *object, const QVariant &objectId) const = 0;

protected:
    template <class T>
    static QVariant propertyValueAsObjectId(const IqOrmPropertyDescription *propertyDescription,
                                     const IqOrmObject *object);

    template <class T>
    static bool setPropertyValueFromObjectId(const IqOrmPropertyDescription *propertyDescription,
                                             IqOrmObject *object,
                                             const QVariant &objectId);
};

template <class T>
bool IqOrmOneObjectDescribingPropertyDescription::setPropertyValueFromObjectId(const IqOrmPropertyDescription *propertyDescription,
                                                                               IqOrmObject *object,
                                                                               const QVariant &objectId)
{
    Q_CHECK_PTR(propertyDescription);
    bool ok;
    qint64 intObjectId = objectId.toLongLong(&ok);
    if (!ok)
        return false;

    //FIXME Разобраться почему не работает с propertyDescription->targetStaticMetaPropery().userType()
    QMetaProperty metaProperty = propertyDescription->targetStaticMetaPropery();
    const char * typeName = metaProperty.typeName();
    QString typeNameStr (typeName);

    //В зависимости от типа
    if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<T *>()))) {
        T* newObject = Q_NULLPTR;
        if (!objectId.isNull())
            newObject = new T();
        IqOrmObjectPrivateAccessor::setObjectId(newObject, intObjectId);
        Q_ASSERT(IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(newObject));
        IqOrmObjectPrivateAccessor::setObjectIsLoadedFromDataSource(newObject, false);
        return propertyDescription->setValue(object, QVariant::fromValue(newObject));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<QPointer<T> >()))) {
        QPointer<T> pointer;
        if (!objectId.isNull())
            pointer = new T();
        IqOrmObjectPrivateAccessor::setObjectId(pointer, intObjectId);
        Q_ASSERT(IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(pointer));
        IqOrmObjectPrivateAccessor::setObjectIsLoadedFromDataSource(pointer, false);
        return propertyDescription->setValue(object, QVariant::fromValue(pointer));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazyPointer<T> >()))) {
        IqOrmLazyPointer<T> pointer;
        if (!objectId.isNull())
            pointer = IqOrmLazyPointer<T> (intObjectId);
        return propertyDescription->setValue(object, QVariant::fromValue(pointer));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<QSharedPointer<T> >()))) {
        QSharedPointer<T> pointer;
        if (!objectId.isNull())
            pointer = QSharedPointer<T>::create();
        IqOrmObjectPrivateAccessor::setObjectId(pointer.data(), intObjectId);
        Q_ASSERT(IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(pointer.data()));
        IqOrmObjectPrivateAccessor::setObjectIsLoadedFromDataSource(pointer.data(), false);
        return propertyDescription->setValue(object, QVariant::fromValue(pointer));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazySharedPointer<T> >()))) {
        IqOrmLazySharedPointer<T> pointer;
        if (!objectId.isNull())
            pointer = IqOrmLazySharedPointer<T>(intObjectId);
        return propertyDescription->setValue(object, QVariant::fromValue(pointer));
    }

    return false;
}

template <class T>
QVariant IqOrmOneObjectDescribingPropertyDescription::propertyValueAsObjectId(const IqOrmPropertyDescription *propertyDescription,
                                                                              const IqOrmObject *object)
{
    Q_CHECK_PTR(propertyDescription);

    //FIXME Разобраться почему не работает с propertyDescription->targetStaticMetaPropery().userType()
    QMetaProperty metaProperty = propertyDescription->targetStaticMetaPropery();
    const char * typeName = metaProperty.typeName();
    QString typeNameStr (typeName);

    QVariant propertyValue = propertyDescription->value(object);

    //В зависимости от типа
    if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<T *>()))) {
        T* pointer = propertyValue.value<T *>();
        if (!pointer)
            return QVariant(QVariant::LongLong);
        return pointer->objectId();
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<QPointer<T> >()))) {
        QPointer<T> pointer = propertyValue.value<QPointer<T> >();
        if (!pointer)
            return QVariant(QVariant::LongLong);
        return pointer->objectId();
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazyPointer<T> >()))) {
        IqOrmLazyPointer<T> pointer = propertyValue.value<IqOrmLazyPointer<T> >();
        if (pointer.isNull())
            return QVariant(QVariant::LongLong);
        return pointer.objectId();
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<QSharedPointer<T> >()))) {
        QSharedPointer<T> pointer = propertyValue.value<QSharedPointer<T> >();
        if (!pointer)
            return QVariant(QVariant::LongLong);
        return pointer->objectId();
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazySharedPointer<T> >()))) {
        IqOrmLazySharedPointer<T> pointer = propertyValue.value<IqOrmLazySharedPointer<T> >();
        if (pointer.isNull())
            return QVariant(QVariant::LongLong);
        return pointer.objectId();
    }

    return QVariant(QVariant::LongLong);
}

#endif // IQORMONEOBJECTDESCRIBINGPROPERTYDESCRIPTION_H

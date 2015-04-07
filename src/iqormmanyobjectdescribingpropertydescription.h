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

#ifndef IQORMMANYOBJECTDESCRIBINGPROPERTYDESCRIPTION_H
#define IQORMMANYOBJECTDESCRIBINGPROPERTYDESCRIPTION_H

#include "iqormpropertydescription.h"
#include "iqormpointerset.h"
#include "iqormsharedpointerset.h"
#include "iqormlazypointerset.h"
#include "iqormlazysharedpointerset.h"
#include "iqormdirectmappedpropertydescription.h"
#include "iqorminversedmappedpropertydescription.h"
#include "iqormoneobjectdescribingpropertydescription.h"
#include <QSet>
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmManyObjectDescribingPropertyDescription
{
public:    
    virtual QVariantList valueAsObjectIds(const IqOrmObject *object) const = 0;

    virtual bool setValueFromObjectIds(IqOrmObject *object,
                                       const QVariantList &objectIds) const = 0;

protected:
    template <class T>
    static QVariantList propertyValueAsObjectIds(const IqOrmPropertyDescription *propertyDescription,
                                          const IqOrmObject *object);

    template <class T>
    static bool setPropertyValueFromObjectIds(const IqOrmPropertyDescription *propertyDescription,
                                              IqOrmObject *object,
                                              const QVariantList &objectIds);
};








template <class T>
bool IqOrmManyObjectDescribingPropertyDescription::setPropertyValueFromObjectIds(const IqOrmPropertyDescription *propertyDescription,
                                                                                 IqOrmObject *object,
                                                                                 const QVariantList &objectIds)
{
    Q_CHECK_PTR(propertyDescription);

    //FIXME Разобраться почему не работает с propertyDescription->targetStaticMetaPropery().userType()
    QMetaProperty metaProperty = propertyDescription->targetStaticMetaPropery();
    const char * typeName = metaProperty.typeName();
    QString typeNameStr (typeName);

    //В зависимости от типа
    if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<QSet<T *> >()))) {
        QSet<T *> pointersList;
        foreach (const QVariant &objectId, objectIds) {
            bool ok;
            qint64 intObjectId = objectId.toLongLong(&ok);
            if (!ok)
                return false;

            T* newObject = Q_NULLPTR;
            if (!objectId.isNull())
                newObject = new T();
            IqOrmObjectPrivateAccessor::setObjectId(newObject, intObjectId);
            Q_ASSERT(IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(object));
            IqOrmObjectPrivateAccessor::setObjectIsLoadedFromDataSource(object, false);
            pointersList << newObject;
        }
        return propertyDescription->setValue(object, QVariant::fromValue(pointersList));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmPointerSet<T> >()))) {
        IqOrmPointerSet<T> pointersList;
        foreach (const QVariant &objectId, objectIds) {
            bool ok;
            qint64 intObjectId = objectId.toLongLong(&ok);
            if (!ok)
                return false;

            QPointer<T> pointer;
            if (!objectId.isNull())
                pointer = new T();
            IqOrmObjectPrivateAccessor::setObjectId(pointer, intObjectId);
            Q_ASSERT(IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(object));
            IqOrmObjectPrivateAccessor::setObjectIsLoadedFromDataSource(object, false);
            pointersList << pointer;
        }
        return propertyDescription->setValue(object, QVariant::fromValue(pointersList));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazyPointerSet<T> >()))) {
        IqOrmLazyPointerSet<T> pointersList;
        foreach (const QVariant &objectId, objectIds) {
            bool ok;
            qint64 intObjectId = objectId.toLongLong(&ok);
            if (!ok)
                return false;

            IqOrmLazyPointer<T> pointer;
            if (!objectId.isNull())
                pointer = IqOrmLazyPointer<T> (intObjectId);
            pointersList << pointer;
        }
        return propertyDescription->setValue(object, QVariant::fromValue(pointersList));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmSharedPointerSet<T> >()))) {
        IqOrmSharedPointerSet<T> pointersList;
        foreach (const QVariant &objectId, objectIds) {
            bool ok;
            qint64 intObjectId = objectId.toLongLong(&ok);
            if (!ok)
                return false;

            QSharedPointer<T> pointer;
            if (!objectId.isNull())
                pointer = QSharedPointer<T>::create();
            IqOrmObjectPrivateAccessor::setObjectId(pointer.data(), intObjectId);
            Q_ASSERT(IqOrmObjectPrivateAccessor::isObjectLoadedFromDataSource(object));
            IqOrmObjectPrivateAccessor::setObjectIsLoadedFromDataSource(object, false);
            pointersList << pointer;
        }
        return propertyDescription->setValue(object, QVariant::fromValue(pointersList));
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazySharedPointerSet<T> >()))) {
        IqOrmLazySharedPointerSet<T> pointersList;
        foreach (const QVariant &objectId, objectIds) {
            bool ok;
            qint64 intObjectId = objectId.toLongLong(&ok);
            if (!ok)
                return false;

            IqOrmLazySharedPointer<T> pointer;
            if (!objectId.isNull())
                pointer = IqOrmLazySharedPointer<T>(intObjectId);
            pointersList << pointer;
        }
        return propertyDescription->setValue(object, QVariant::fromValue(pointersList));
    }

    return false;
}

template <class T>
QVariantList IqOrmManyObjectDescribingPropertyDescription::propertyValueAsObjectIds(const IqOrmPropertyDescription *propertyDescription,
                                                                                     const IqOrmObject *object)
{
    Q_CHECK_PTR(propertyDescription);

    //FIXME Разобраться почему не работает с propertyDescription->targetStaticMetaPropery().userType()
    QMetaProperty metaProperty = propertyDescription->targetStaticMetaPropery();
    const char * typeName = metaProperty.typeName();
    QString typeNameStr (typeName);

    QVariant propertyValue = propertyDescription->value(object);

    QVariantList objectIds;

    //В зависимости от типа
    if(typeNameStr == QString(QMetaType::typeName(qMetaTypeId<QSet<T *> >()))) {
        QSet<T *> valueObjectsList = propertyValue.value<QSet<T *> >();
        foreach (const T *object, valueObjectsList) {
            if (object)
                objectIds << object->objectId();
            else
                objectIds << QVariant(QVariant::LongLong);
        }
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmPointerSet<T> >()))) {
        IqOrmPointerSet<T> valueObjectsList = propertyValue.value<IqOrmPointerSet<T> >();
        foreach (const QPointer<T> &pointer, valueObjectsList) {
            if (pointer)
                objectIds << pointer->objectId();
            else
                objectIds << QVariant(QVariant::LongLong);
        }
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazyPointerSet<T> >()))) {
        IqOrmLazyPointerSet<T> valueObjectsList = propertyValue.value<IqOrmLazyPointerSet<T> >();
        foreach (const IqOrmLazyPointer<T> &pointer, valueObjectsList) {
            if (!pointer.isNull())
                objectIds << pointer.objectId();
            else
                objectIds << QVariant(QVariant::LongLong);
        }
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmSharedPointerSet<T> >()))) {
        IqOrmSharedPointerSet<T> valueObjectsList = propertyValue.value<IqOrmSharedPointerSet<T> >();
        foreach (const QSharedPointer<T> &pointer, valueObjectsList) {
            if (pointer)
                objectIds << pointer->objectId();
            else
                objectIds << QVariant(QVariant::LongLong);
        }
    } else if (typeNameStr == QString(QMetaType::typeName(qMetaTypeId<IqOrmLazySharedPointerSet<T> >()))) {
        IqOrmLazySharedPointerSet<T> valueObjectsList = propertyValue.value<IqOrmLazySharedPointerSet<T> >();
        foreach (const IqOrmLazySharedPointer<T> &pointer, valueObjectsList) {
            if (!pointer.isNull())
                objectIds << pointer.objectId();
            else
                objectIds << QVariant(QVariant::LongLong);
        }
    }

    return objectIds;
}

#endif // IQORMMANYOBJECTDESCRIBINGPROPERTYDESCRIPTION_H

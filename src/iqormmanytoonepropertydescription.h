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

#ifndef IQORMMANYTOONEPROPERTYDESCRIPTION_H
#define IQORMMANYTOONEPROPERTYDESCRIPTION_H

#include "iqormbasemanytoonepropertydescription.h"
#include "iqorm_global.h"

template <class T>
class IqOrmManyToOnePropertyDescription : public IqOrmBaseManyToOnePropertyDescription
{
public:
    explicit IqOrmManyToOnePropertyDescription(IqOrmMetaModel *parent = Q_NULLPTR);

public:    
    virtual QVariant valueAsObjectId(const IqOrmObject *object) const Q_DECL_OVERRIDE;

    virtual bool setValueFromObjectId(IqOrmObject *object, const QVariant &objectId) const Q_DECL_OVERRIDE;

    virtual const IqOrmMetaModel *associatedObjectOrmModel() const Q_DECL_OVERRIDE;
};







template <class T>
IqOrmManyToOnePropertyDescription<T>::IqOrmManyToOnePropertyDescription(IqOrmMetaModel *parent) :
    IqOrmBaseManyToOnePropertyDescription(parent)
{
}

template <class T>
bool IqOrmManyToOnePropertyDescription<T>::setValueFromObjectId(IqOrmObject *object,
                                                               const QVariant &objectId) const
{
    return setPropertyValueFromObjectId<T>(this, object, objectId);
}

template <class T>
QVariant IqOrmManyToOnePropertyDescription<T>::valueAsObjectId(const IqOrmObject *object) const
{
    return propertyValueAsObjectId<T>(this, object);
}

template <class T>
const IqOrmMetaModel * IqOrmManyToOnePropertyDescription<T>::associatedObjectOrmModel() const
{
    return T::staticOrmMetaModel();
}
#endif // IQORMMANYTOONEPROPERTYDESCRIPTION_H

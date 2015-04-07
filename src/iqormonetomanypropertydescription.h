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

#ifndef IQORMONETOMANYPROPERTYDESCRIPTION_H
#define IQORMONETOMANYPROPERTYDESCRIPTION_H

#include "iqormbaseonetomanypropertydescription.h"
#include "iqorm_global.h"

template <class T>
class IqOrmOneToManyPropertyDescription : public IqOrmBaseOneToManyPropertyDescription
{
public:
    explicit IqOrmOneToManyPropertyDescription(IqOrmMetaModel *parent = Q_NULLPTR);

public:
    virtual QVariantList valueAsObjectIds(const IqOrmObject *object) const Q_DECL_OVERRIDE;

    virtual bool setValueFromObjectIds(IqOrmObject *object,
                                       const QVariantList &objectIds) const Q_DECL_OVERRIDE;

    virtual const IqOrmMetaModel *associatedObjectOrmModel() const Q_DECL_OVERRIDE;
};

template <class T>
IqOrmOneToManyPropertyDescription<T>::IqOrmOneToManyPropertyDescription(IqOrmMetaModel *parent) :
    IqOrmBaseOneToManyPropertyDescription(parent)
{
}

template <class T>
bool IqOrmOneToManyPropertyDescription<T>::setValueFromObjectIds(IqOrmObject *object,
                                                                 const QVariantList &objectIds) const
{
    return setPropertyValueFromObjectIds<T>(this, object, objectIds);
}

template <class T>
QVariantList IqOrmOneToManyPropertyDescription<T>::valueAsObjectIds(const IqOrmObject *object) const
{
    return propertyValueAsObjectIds<T>(this, object);
}

template <class T>
const IqOrmMetaModel * IqOrmOneToManyPropertyDescription<T>::associatedObjectOrmModel() const
{
    return T::staticOrmMetaModel();
}

#endif // IQORMONETOMANYPROPERTYDESCRIPTION_H

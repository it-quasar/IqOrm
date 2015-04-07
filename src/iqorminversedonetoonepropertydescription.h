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

#ifndef IQORMINVERSEDONETOONEPROPERTYDESCRIPTION_H
#define IQORMINVERSEDONETOONEPROPERTYDESCRIPTION_H

#include "iqormbaseinversedonetoonepropertydescription.h"
#include "iqorm_global.h"

template <class T>
class IqOrmInversedOneToOnePropertyDescription : public IqOrmBaseInversedOneToOnePropertyDescription
{
public:
    explicit IqOrmInversedOneToOnePropertyDescription(IqOrmMetaModel *parent = Q_NULLPTR);

public:
    virtual QVariant valueAsObjectId(const IqOrmObject *object) const Q_DECL_OVERRIDE;

    virtual bool setValueFromObjectId(IqOrmObject *object, const QVariant &objectId) const Q_DECL_OVERRIDE;

    virtual const IqOrmMetaModel *associatedObjectOrmModel() const Q_DECL_OVERRIDE;
};

template <class T>
IqOrmInversedOneToOnePropertyDescription<T>::IqOrmInversedOneToOnePropertyDescription(IqOrmMetaModel *parent) :
    IqOrmBaseInversedOneToOnePropertyDescription(parent)
{
}

template <class T>
bool IqOrmInversedOneToOnePropertyDescription<T>::setValueFromObjectId(IqOrmObject *object,
                                                                       const QVariant &objectId) const
{
    return setPropertyValueFromObjectId<T>(this, object, objectId);
}

template <class T>
QVariant IqOrmInversedOneToOnePropertyDescription<T>::valueAsObjectId(const IqOrmObject *object) const
{
    return propertyValueAsObjectId<T>(this, object);
}

template <class T>
const IqOrmMetaModel * IqOrmInversedOneToOnePropertyDescription<T>::associatedObjectOrmModel() const
{
    return T::staticOrmMetaModel();
}
#endif // IQORMINVERSEDONETOONEPROPERTYDESCRIPTION_H

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
#error 'iqorminversedonetoonepropertydescription_impl.h' is not supposed to be included directly. Include 'iqorminversedonetoonepropertydescription.h' instead.
#endif

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

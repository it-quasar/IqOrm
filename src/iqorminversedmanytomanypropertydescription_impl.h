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

#ifndef IQORMINVERSEDMANYTOMANYPROPERTYDESCRIPTION_H
#error 'iqorminversedmanytomanypropertydescription_impl.h' is not supposed to be included directly. Include 'iqorminversedmanytomanypropertydescription.h' instead.
#endif

template <class T>
IqOrmInversedManyToManyPropertyDescription<T>::IqOrmInversedManyToManyPropertyDescription(IqOrmMetaModel *parent) :
    IqOrmBaseInversedManyToManyPropertyDescription(parent)
{
}

template <class T>
bool IqOrmInversedManyToManyPropertyDescription<T>::setValueFromObjectIds(IqOrmObject *object,
                                                                          const QVariantList &objectIds) const
{
    return setPropertyValueFromObjectIds<T>(this, object, objectIds);
}

template <class T>
QVariantList IqOrmInversedManyToManyPropertyDescription<T>::valueAsObjectIds(const IqOrmObject *object) const
{
    return propertyValueAsObjectIds<T>(this, object);
}

template <class T>
const IqOrmMetaModel * IqOrmInversedManyToManyPropertyDescription<T>::associatedObjectOrmModel() const
{
    return T::staticOrmMetaModel();
}

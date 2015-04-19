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

#ifndef IQORMDATASOURCECHANGES_H
#error 'iqormdatasourcechanges_impl.h' is not supposed to be included directly. Include 'iqormdatasourcechanges.h' instead.
#endif

template <class T>
QSet<T *> IqOrmDataSourceChanges::propertiesChanges() const
{
    QSet<T *> result;
    foreach (IqOrmPropertyChanges *changes, d->propertiesChanges.values()) {
        T *castChanges = dynamic_cast<T *>(changes);
        if (castChanges)
            result << castChanges;
    }
    return result;
}

template <class T>
T * IqOrmDataSourceChanges::propertyChanges(const IqOrmPropertyDescription *propertyDescription)
{
    if (d->propertiesChanges.contains(propertyDescription))
        return dynamic_cast<T *>(d->propertiesChanges[propertyDescription]);

    T* newChanges = new T();
    if (true)
        Q_CHECK_PTR(dynamic_cast<IqOrmPropertyChanges *>(newChanges));

    newChanges->setPropertyDescription(propertyDescription);
    d->propertiesChanges[propertyDescription] = newChanges;

    return newChanges;
}

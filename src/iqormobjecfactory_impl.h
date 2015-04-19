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

#ifndef IQORMOBJECTFACTORY_H
#error 'iqormobjectfactory_impl.h' is not supposed to be included directly. Include 'iqormobjectfactory.h' instead.
#endif

template <class T>
void IqOrmObjectFactory::registerClass()
{
    QString className = T::staticMetaObject.className();
    if (!m_builders.contains(className)) {
        m_builders[className] = new IqOrmObjectBulder<T>();
    }
}

template <class T>
IqOrmObject *IqOrmObjectFactory::IqOrmObjectBulder<T>::build(QObject *parent) const
{
    return new T(parent);
}

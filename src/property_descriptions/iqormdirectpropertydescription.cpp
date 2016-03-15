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

#include "iqormdirectpropertydescription.h"
#include "iqormobject.h"

IqOrmDirectPropertyDescription::IqOrmDirectPropertyDescription(IqOrmMetaModel *parent) :
    IqOrmPropertyDescription(parent),
    m_columnName("")
{
}

IqOrmPropertyDescription::MappedType IqOrmDirectPropertyDescription::mappedType() const
{
    return Direct;
}

IqOrmPropertyDescription::StoredValue IqOrmDirectPropertyDescription::storedValue() const
{
    return SimpleVariant;
}

QString IqOrmDirectPropertyDescription::columnName() const
{
    return m_columnName;
}

void IqOrmDirectPropertyDescription::setColumnName(const QString &columnName)
{
    if (m_columnName != columnName) {
        m_columnName = columnName;
        emit columnNameChanged();
    }
}

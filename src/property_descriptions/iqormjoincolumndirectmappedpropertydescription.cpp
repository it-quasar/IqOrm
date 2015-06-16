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

#include "iqormjoincolumndirectmappedpropertydescription.h"

IqOrmJoinColumnDirectMappedPropertyDescription::IqOrmJoinColumnDirectMappedPropertyDescription(IqOrmMetaModel *parent) :
    IqOrmDirectMappedPropertyDescription(parent)
{
}

QString IqOrmJoinColumnDirectMappedPropertyDescription::joinColumnName() const
{
    return m_joinColumnName;
}

void IqOrmJoinColumnDirectMappedPropertyDescription::setJoinColumnName(const QString &joinColumnName)
{
    if (m_joinColumnName != joinColumnName) {
        m_joinColumnName = joinColumnName;
        emit joinColumnNameChanged();
    }
}



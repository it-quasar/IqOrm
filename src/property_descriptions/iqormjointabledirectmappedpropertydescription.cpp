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

#include "iqormjointabledirectmappedpropertydescription.h"

IqOrmJoinTableDirectMappedPropertyDescription::IqOrmJoinTableDirectMappedPropertyDescription(IqOrmMetaModel *parent) :
    IqOrmDirectMappedPropertyDescription(parent),
    m_joinTable(Q_NULLPTR)
{
}

IqOrmJoinTable *IqOrmJoinTableDirectMappedPropertyDescription::joinTable() const
{
    return m_joinTable;
}

void IqOrmJoinTableDirectMappedPropertyDescription::setJoinTable(IqOrmJoinTable *joinTable)
{
    if (m_joinTable != joinTable) {
        if (m_joinTable)
            m_joinTable->deleteLater();
        m_joinTable = joinTable;
        joinTable->setParent(this);
        emit joinTableChanged();
    }
}

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

#include "iqormsqlmanytomanypropertychanges.h"

IqOrmSqlManyToManyPropertyChanges::IqOrmSqlManyToManyPropertyChanges()
{
}

QHash<qint64, qint64> IqOrmSqlManyToManyPropertyChanges::newObjectIds() const
{
    return m_newObjectIds;
}

void IqOrmSqlManyToManyPropertyChanges::addNewObjectId(qint64 objectId, qint64 joinTableRowId)
{
    if (m_removedObjectIds.contains(objectId))
        m_removedObjectIds.remove(objectId);
    else
        m_newObjectIds[objectId] = joinTableRowId;
}

QHash<qint64, qint64> IqOrmSqlManyToManyPropertyChanges::removedObjectIds() const
{
    return m_removedObjectIds;
}

void IqOrmSqlManyToManyPropertyChanges::addRemovedObjectId(qint64 objectId, qint64 joinTableRowId)
{
    if (m_newObjectIds.contains(objectId))
        m_newObjectIds.remove(objectId);
    else
        m_removedObjectIds[objectId] = joinTableRowId;
}

void IqOrmSqlManyToManyPropertyChanges::addNewObjectId(qint64 objectId)
{
    Q_UNUSED(objectId);
}

void IqOrmSqlManyToManyPropertyChanges::addRemovedObjectId(qint64 objectId)
{
    Q_UNUSED(objectId);
}



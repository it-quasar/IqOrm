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

#include "iqormmanyobjectdiscribingpropertychanges.h"

IqOrmManyObjectDiscribingPropertyChanges::IqOrmManyObjectDiscribingPropertyChanges()
{
}

QSet<qint64> IqOrmManyObjectDiscribingPropertyChanges::removedObjectIds() const
{
    return m_removedObjectIds;
}

void IqOrmManyObjectDiscribingPropertyChanges::addRemovedObjectId(qint64 objectId)
{
    if (m_newObjectIds.contains(objectId))
        m_newObjectIds.remove(objectId);
    else
        m_removedObjectIds.insert(objectId);
}

QSet<qint64> IqOrmManyObjectDiscribingPropertyChanges::newObjectIds() const
{
    return m_newObjectIds;
}

void IqOrmManyObjectDiscribingPropertyChanges::addNewObjectId(qint64 objectId)
{
    if (m_removedObjectIds.contains(objectId))
        m_removedObjectIds.remove(objectId);
    else
        m_newObjectIds.insert(objectId);
}



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

#ifndef IQORMSQLMANYTOMANYPROPERTYCHANGES_H
#define IQORMSQLMANYTOMANYPROPERTYCHANGES_H

#include "iqormmanytomanypropertychanges.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlManyToManyPropertyChanges : public IqOrmManyToManyPropertyChanges
{
public:
    IqOrmSqlManyToManyPropertyChanges();

    QHash<qint64, qint64> newObjectIds() const;
    void addNewObjectId(qint64 objectId, qint64 joinTableRowId);

    QHash<qint64, qint64> removedObjectIds() const;
    void addRemovedObjectId(qint64 objectId, qint64 joinTableRowId);

private:
    void addNewObjectId(qint64 objectId);
    void addRemovedObjectId(qint64 objectId);

private:
    QHash<qint64, qint64> m_newObjectIds;
    QHash<qint64, qint64> m_removedObjectIds;
};

#endif // IQORMSQLMANYTOMANYPROPERTYCHANGES_H

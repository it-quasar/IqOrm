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

#ifndef IQORMSQLMANYTOMANYPROPERTYDESCRIPTIONPROCESSOR_H
#define IQORMSQLMANYTOMANYPROPERTYDESCRIPTIONPROCESSOR_H

#include "iqormsqlmanyobjectsdescribingpropertydescriptionprocessor.h"
#include "iqormsqlmanytomanypropertydescriptionprocessor.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlManyToManyPropertyDescriptionProcessor : public IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor
{
public:
    explicit IqOrmSqlManyToManyPropertyDescriptionProcessor();

public:
    const IqOrmBaseManyToManyPropertyDescription *propertyDescription() const;

    virtual QString selectFieldName() const Q_DECL_OVERRIDE;
    virtual QList<IqOrmSqlJoinOperation> selectJoinOperations() const Q_DECL_OVERRIDE;
    virtual bool updateAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool removeAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;

protected:
    virtual QSet<qint64> ownerObjectIds(IqOrmDataSourceOperationResult *result,
                                        bool *ok) const Q_DECL_OVERRIDE;
    virtual bool removeOwner(QSet<qint64> ownerIds,
                             IqOrmDataSourceOperationResult *result,
                             QHash<qint64, qint64> *joinTableChanges = Q_NULLPTR) const Q_DECL_OVERRIDE;

    virtual bool setOwner(QSet<qint64> ownerIds,
                          IqOrmDataSourceOperationResult *result,
                          QHash<qint64, qint64> *joinTableChanges = Q_NULLPTR) const Q_DECL_OVERRIDE;

    virtual void addNewObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                       qint64 objectId,
                                       qint64 joinTableRowId) const Q_DECL_OVERRIDE;

    virtual void addRemovedObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                           qint64 objectId,
                                           qint64 joinTableRowId) const Q_DECL_OVERRIDE;

private:
    const IqOrmBaseInversedManyToManyPropertyDescription *associatedPropertyDescription() const;
    QString escapedAssociatedObjectTableName() const;
};

#endif // IQORMSQLMANYTOMANYPROPERTYDESCRIPTIONPROCESSOR_H

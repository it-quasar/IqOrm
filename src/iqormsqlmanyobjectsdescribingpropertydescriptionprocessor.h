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

#ifndef IQORMSQLMANYOBJECTSDESCRIBINGPROPERTYDESCRIPTIONPROCESSOR_H
#define IQORMSQLMANYOBJECTSDESCRIBINGPROPERTYDESCRIPTIONPROCESSOR_H

#include "iqormsqlmappedpropertydescriptionprocessor.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor : public IqOrmSqlMappedPropertyDescriptionProcessor
{
public:
    explicit IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor();

public:
    const IqOrmManyObjectDescribingPropertyDescription *propertyDescription() const;

    virtual QVariant propertyValue() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QString insertFieldName() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QString updateFieldName() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QVariant insertValue() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QVariant updateValue() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool selectAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool insertAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool preSelect(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool preInsert(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool preUpdate(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool preRemove(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool postSelect(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool postInsert(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool postUpdate(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool postRemove(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;

protected:
    virtual QSet<qint64> ownerObjectIds(IqOrmDataSourceOperationResult *result,
                                                 bool *ok) const = 0;
    virtual bool removeOwner(QSet<qint64> ownerIds,
                             IqOrmDataSourceOperationResult *result,
                             QHash<qint64, qint64> *joinTableChanges = Q_NULLPTR) const = 0;

    virtual bool setOwner(QSet<qint64> ownerIds,
                          IqOrmDataSourceOperationResult *result,
                          QHash<qint64, qint64> *joinTableChanges = Q_NULLPTR) const = 0;

    virtual void addNewObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                       qint64 objectId,
                                       qint64 joinTableRowId) const = 0;

    virtual void addRemovedObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                           qint64 objectId,
                                           qint64 joinTableRowId) const = 0;
};

#endif // IQORMSQLMANYOBJECTSDESCRIBINGPROPERTYDESCRIPTIONPROCESSOR_H

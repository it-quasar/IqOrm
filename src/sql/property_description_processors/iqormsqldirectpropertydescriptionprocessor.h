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

#ifndef IQORMSQLDIRECTPROPERTYDESCRIPTIONPROCESSOR_H
#define IQORMSQLDIRECTPROPERTYDESCRIPTIONPROCESSOR_H

#include "iqormsqlabstractpropertydescriptionprocessor.h"
#include "iqormdirectpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlDirectPropertyDescriptionProcessor : public IqOrmSqlAbstractPropertyDescriptionProcessor
{
public:
    explicit IqOrmSqlDirectPropertyDescriptionProcessor();

public:
    const IqOrmDirectPropertyDescription *propertyDescription() const;

    virtual QVariant propertyValue() const Q_DECL_OVERRIDE;
    virtual QString selectFieldName() const Q_DECL_OVERRIDE;
    virtual QString insertFieldName() const Q_DECL_OVERRIDE;
    virtual QString updateFieldName() const Q_DECL_OVERRIDE;
    virtual QVariant insertValue() const Q_DECL_OVERRIDE;
    virtual QVariant updateValue() const Q_DECL_OVERRIDE;
    virtual QList<IqOrmSqlJoinOperation> selectJoinOperations() const Q_DECL_OVERRIDE;
    virtual bool selectAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool insertAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool updateAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool removeAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool preSelect(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool preInsert(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool preUpdate(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool preRemove(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool postSelect(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool postInsert(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool postUpdate(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual bool postRemove(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE;
    virtual QVariant convertSqlValue(const QVariant &sqlValue,
                                     bool *ok = Q_NULLPTR,
                                     QString *error = Q_NULLPTR) const Q_DECL_OVERRIDE;
};

#endif // IQORMSQLDIRECTPROPERTYDESCRIPTIONPROCESSOR_H

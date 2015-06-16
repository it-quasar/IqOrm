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

#ifndef IQORMSQLONEOBJECTDESCRTIBINGPROPERTYDESCRIPTIONPROCESSOR_H
#define IQORMSQLONEOBJECTDESCRTIBINGPROPERTYDESCRIPTIONPROCESSOR_H

#include "iqormsqlmappedpropertydescriptionprocessor.h"
#include "iqormoneobjectdescribingpropertydescription.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor : public IqOrmSqlMappedPropertyDescriptionProcessor
{
public:
    explicit IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor();

public:
    const IqOrmOneObjectDescribingPropertyDescription *propertyDescription() const;

    virtual QVariant propertyValue() const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool selectAllowed(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool preSelect(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual bool postSelect(IqOrmDataSourceOperationResult *result) const Q_DECL_OVERRIDE Q_DECL_FINAL;
    virtual QVariant convertSqlValue(const QVariant &sqlValue,
                                     bool *ok = Q_NULLPTR,
                                     QString *error = Q_NULLPTR) const Q_DECL_OVERRIDE Q_DECL_FINAL;
};

#endif // IQORMSQLONEOBJECTDESCRTIBINGPROPERTYDESCRIPTIONPROCESSOR_H

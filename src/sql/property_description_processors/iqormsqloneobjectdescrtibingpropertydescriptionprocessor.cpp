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


#include "iqormsqloneobjectdescrtibingpropertydescriptionprocessor.h"

IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor::IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor() :
    IqOrmSqlMappedPropertyDescriptionProcessor()
{
}

const IqOrmOneObjectDescribingPropertyDescription *IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor::propertyDescription() const
{
    return dynamic_cast<const IqOrmOneObjectDescribingPropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QVariant IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor::propertyValue() const
{
    Q_CHECK_PTR(propertyDescription());
    return propertyDescription()->valueAsObjectId(object());
}

bool IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor::selectAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor::preSelect(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor::postSelect(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

QVariant IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor::convertSqlValue(const QVariant &sqlValue, bool *ok, QString *error) const
{
    Q_CHECK_PTR(propertyDescription());

    QVariant result;

    if (sqlValue.canConvert<qint32>()) {
        result = sqlValue;
    } else {
        if (error)
            *error = QObject::tr("Error convert value %0 to qint64 for property %1.")
                .arg(sqlValue.toString())
                .arg(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription()->propertyName());
        if (ok)
            *ok = false;
        return result;
    }

    if (error)
        error->clear();
    if (ok)
        *ok = true;

    return result;
}

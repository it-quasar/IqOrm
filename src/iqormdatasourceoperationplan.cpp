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

#include "iqormdatasourceoperationplan.h"

IqOrmDataSourceOperationPlan::IqOrmDataSourceOperationPlan():
    d(new IqOrmDataSourceOperationPlanData)
{
}

IqOrmAbstractDataSource::Operation IqOrmDataSourceOperationPlan::operation() const
{
    return d->operation;
}

void IqOrmDataSourceOperationPlan::setOperation(const IqOrmAbstractDataSource::Operation &operation)
{
    if (d->operation != operation)
        d->operation = operation;
}

qint64 IqOrmDataSourceOperationPlan::objectId() const
{
    return d->objectId;
}

void IqOrmDataSourceOperationPlan::setObjectId(qint64 objectId)
{
    if (d->objectId != objectId)
        d->objectId = objectId;
}

IqOrmAbstractDataSource *IqOrmDataSourceOperationPlan::dataSource() const
{
    return d->dataSource;
}

void IqOrmDataSourceOperationPlan::setDataSource(IqOrmAbstractDataSource *dataSource)
{
    if (d->dataSource != dataSource)
        d->dataSource = dataSource;
}

QSet<const IqOrmPropertyDescription *> IqOrmDataSourceOperationPlan::changedProperties() const
{
    return d->changedProperties;
}

void IqOrmDataSourceOperationPlan::setChangedProperites(const QSet<const IqOrmPropertyDescription *> &properites)
{
    d->changedProperties = properites;
}


IqOrmDataSourceOperationPlan::IqOrmDataSourceOperationPlanData::IqOrmDataSourceOperationPlanData():
    objectId(-1),
    operation(IqOrmAbstractDataSource::NotSetOperation),
    dataSource(Q_NULLPTR)
{
}

IqOrmDataSourceOperationPlan::IqOrmDataSourceOperationPlanData::~IqOrmDataSourceOperationPlanData()
{
}

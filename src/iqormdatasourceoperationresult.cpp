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

#include "iqormdatasourceoperationresult.h"
#include "iqormabstractdatasource.h"

IqOrmDataSourceOperationResult::IqOrmDataSourceOperationResultData::IqOrmDataSourceOperationResultData():
    dataSource(Q_NULLPTR)
{
}

IqOrmDataSourceOperationResult::IqOrmDataSourceOperationResultData::~IqOrmDataSourceOperationResultData()
{
    foreach (IqOrmDataSourceChanges *change, changes.values()) {
        delete change;
    }
}

IqOrmDataSourceOperationResult::IqOrmDataSourceOperationResult() :
    d(new IqOrmDataSourceOperationResultData)
{
}

IqOrmDataSourceOperationResult::operator bool() const
{
    return result();
}

bool IqOrmDataSourceOperationResult::result() const
{
    return d->error.isEmpty();
}

void IqOrmDataSourceOperationResult::setDataSource(IqOrmAbstractDataSource *dataSource)
{
    if (d->dataSource != dataSource) {
        d->dataSource = dataSource;

        foreach (IqOrmDataSourceChanges *change, d->changes.values()) {
            change->setDataSource(dataSource);
        }
    }
}

QString IqOrmDataSourceOperationResult::error() const
{
    return d->error;
}

void IqOrmDataSourceOperationResult::setError(const QString &error)
{
    if (d->error != error)
        d->error = error;
}

QList<IqOrmDataSourceChanges> IqOrmDataSourceOperationResult::allChanges() const
{
    QList <IqOrmDataSourceChanges> result;
    foreach (IqOrmDataSourceChanges *change, d->changes.values()) {
        result << *change;
    }

    return result;
}

IqOrmDataSourceChanges *IqOrmDataSourceOperationResult::changes(const IqOrmMetaModel *model, qint64 objectId)
{
    foreach (IqOrmDataSourceChanges *change, d->changes.values(model)) {
        if (change->objectId() == objectId)
            return change;
    }

    //Не нашли, создадим
    IqOrmDataSourceChanges *newChange = new IqOrmDataSourceChanges();
    newChange->setObjectOrmModel(model);
    newChange->setObjectId(objectId);
    newChange->setDataSource(d->dataSource);
    d->changes.insert(model, newChange);

    return newChange;
}




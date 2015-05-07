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

#include "iqormtransactioncontrol.h"

#include "iqormabstractdatasource.h"

IqOrmTransactionControl::IqOrmTransactionControlData::IqOrmTransactionControlData() :
    dataSource(Q_NULLPTR),
    transactionIsOpen(false)
{
}

IqOrmTransactionControl::IqOrmTransactionControlData::~IqOrmTransactionControlData()
{
    if (transactionIsOpen) {
        Q_CHECK_PTR(dataSource);
        dataSource->rollbackTransaction();
    }
}

IqOrmTransactionControl::IqOrmTransactionControl() :
    d(new IqOrmTransactionControlData)
{
}

IqOrmTransactionControl::IqOrmTransactionControl(IqOrmAbstractDataSource *dataSource) :
    d(new IqOrmTransactionControlData)
{
    Q_CHECK_PTR(dataSource);
    d->dataSource = dataSource;
    d->transactionIsOpen = d->dataSource->openTransaction();
}

IqOrmTransactionControl::~IqOrmTransactionControl()
{
}

bool IqOrmTransactionControl::isValid() const
{
    QMutexLocker locker(&d->mutex);
    return d->dataSource;
}

bool IqOrmTransactionControl::isTransactionOpen() const
{
    QMutexLocker locker(&d->mutex);
    return  d->transactionIsOpen;
}

bool IqOrmTransactionControl::commit()
{
    QMutexLocker locker(&d->mutex);
    Q_CHECK_PTR(d->dataSource);
    d->transactionIsOpen = false;
    return d->dataSource->commitTransaction();
}

bool IqOrmTransactionControl::rollback()
{
    QMutexLocker locker(&d->mutex);
    Q_CHECK_PTR(d->dataSource);
    d->transactionIsOpen = false;
    return d->dataSource->rollbackTransaction();
}

IqOrmAbstractDataSource *IqOrmTransactionControl::dataSource() const
{
    QMutexLocker locker(&d->mutex);
    return d->dataSource;
}

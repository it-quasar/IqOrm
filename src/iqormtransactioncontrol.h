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

#ifndef IQORMTRANSACTIONCONTROL_H
#define IQORMTRANSACTIONCONTROL_H

#include <QSharedData>
#include "iqorm_global.h"
#include <QMutex>

class IqOrmAbstractDataSource;

class IQORMSHARED_EXPORT IqOrmTransactionControl
{
public:
    IqOrmTransactionControl();

    IqOrmTransactionControl(IqOrmAbstractDataSource *dataSource);

    ~IqOrmTransactionControl();

    bool isValid() const;

    bool isTransactionOpen() const;

    bool commit();

    bool rollback();

    IqOrmAbstractDataSource *dataSource() const;

private:
    class IqOrmTransactionControlData: public QSharedData
    {
    public:
        IqOrmTransactionControlData();
        ~IqOrmTransactionControlData();
        IqOrmAbstractDataSource *dataSource;
        bool transactionIsOpen;
        mutable QMutex mutex;
    };

    QExplicitlySharedDataPointer<IqOrmTransactionControlData> d;
};

#endif // IQORMTRANSACTIONCONTROL_H

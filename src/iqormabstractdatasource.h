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

#ifndef IQORMABSTRACTDATASOURCE_H
#define IQORMABSTRACTDATASOURCE_H

#include <QObject>
#include "iqorm_global.h"
#include "iqormtransactioncontrol.h"

class IqOrmAbstractObjectDataSource;
class IqOrmAbstractModelDataSource;

class IQORMSHARED_EXPORT IqOrmAbstractDataSource : public QObject
{
    Q_OBJECT
public:
    explicit IqOrmAbstractDataSource(QObject *parent = Q_NULLPTR);

    virtual ~IqOrmAbstractDataSource();

    IqOrmTransactionControl transaction();

    virtual IqOrmAbstractObjectDataSource *objectDataSource() const = 0;

    virtual IqOrmAbstractModelDataSource *objectsModelDataSource() const = 0;

protected:
    friend class IqOrmTransactionControl;

    virtual bool openTransaction();

    virtual bool commitTransaction();

    virtual bool rollbackTransaction();
};

#endif // IQORMABSTRACTDATASOURCE_H

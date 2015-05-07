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

#ifndef IQORMABSTRACTTRIGGERS_H
#define IQORMABSTRACTTRIGGERS_H

#include <QObject>
#include "iqormobject.h"
#include "iqormdatasourceoperationplan.h"
#include "iqormdatasourceoperationresult.h"
#include "iqorm_global.h"
#include "iqormtransactioncontrol.h"

class IQORMSHARED_EXPORT IqOrmAbstractTriggers : public QObject
{
    Q_OBJECT
public:
    explicit IqOrmAbstractTriggers(QObject *parent = Q_NULLPTR);
    virtual ~IqOrmAbstractTriggers();

    virtual bool preLoad(IqOrmObject *object, IqOrmTransactionControl transaction, const IqOrmDataSourceOperationPlan &operationPlan, QString *errorText);

    virtual bool postLoad(IqOrmObject *object, IqOrmTransactionControl transaction, const IqOrmDataSourceOperationResult &operationResult, QString *errorText);

    virtual bool preSave(IqOrmObject *object, IqOrmTransactionControl transaction, const IqOrmDataSourceOperationPlan &operationPlan, QString *errorText);

    virtual bool postSave(IqOrmObject *object, IqOrmTransactionControl transaction, const IqOrmDataSourceOperationResult &operationResult, QString *errorText);

    virtual bool preRemove(IqOrmObject *object, IqOrmTransactionControl transaction, const IqOrmDataSourceOperationPlan &operationPlan, QString *errorText);

    virtual bool postRemove(IqOrmObject *object, IqOrmTransactionControl transaction, const IqOrmDataSourceOperationResult &operationResult, QString *errorText);
};

#endif // IQORMABSTRACTTRIGGERS_H

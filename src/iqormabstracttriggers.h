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
#include "iqormdatasourceoperationresult.h"
#include "iqorm_global.h"

class IQORMSHARED_EXPORT IqOrmAbstractTriggers : public QObject
{
    Q_OBJECT
public:
    enum SaveType {
        Persist,
        Update
    };

    explicit IqOrmAbstractTriggers(QObject *parent = 0);
    virtual ~IqOrmAbstractTriggers();

    virtual bool preLoad(IqOrmObject *object, qint64 objectId, QString *errorText = Q_NULLPTR);

    virtual bool postLoad(IqOrmObject *object, qint64 objectId, const IqOrmDataSourceOperationResult &operationResult, QString *errorText = Q_NULLPTR);

    virtual bool preSave(IqOrmObject *object, SaveType saveType, const QList<const IqOrmPropertyDescription*> propertiesToSave, QString *errorText = Q_NULLPTR);

    virtual bool postSave(IqOrmObject *object, SaveType saveType, const IqOrmDataSourceOperationResult &operationResult, QString *errorText = Q_NULLPTR);

    virtual bool preRemove(IqOrmObject *object, QString *errorText = Q_NULLPTR);

    virtual bool postRemove(IqOrmObject *object, qint64 objectId, const IqOrmDataSourceOperationResult &operationResult, QString *errorText = Q_NULLPTR);
};

#endif // IQORMABSTRACTTRIGGERS_H

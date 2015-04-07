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

#include "iqormabstracttriggers.h"

IqOrmAbstractTriggers::IqOrmAbstractTriggers(QObject *parent) :
    QObject(parent)
{
}

IqOrmAbstractTriggers::~IqOrmAbstractTriggers()
{
}

bool IqOrmAbstractTriggers::preLoad(IqOrmObject *object, qint64 objectId, QString *errorText)
{
    Q_UNUSED(object);
    Q_UNUSED(objectId);
    Q_UNUSED(errorText);
    return true;
}

bool IqOrmAbstractTriggers::postLoad(IqOrmObject *object, qint64 objectId, const IqOrmDataSourceOperationResult &operationResult, QString *errorText)
{
    Q_UNUSED(object);
    Q_UNUSED(objectId);
    Q_UNUSED(operationResult);
    Q_UNUSED(errorText);
    return true;
}

bool IqOrmAbstractTriggers::preSave(IqOrmObject *object, IqOrmAbstractTriggers::SaveType saveType, const QList<const IqOrmPropertyDescription *> propertiesToSave, QString *errorText)
{
    Q_UNUSED(object);
    Q_UNUSED(saveType);
    Q_UNUSED(propertiesToSave);
    Q_UNUSED(errorText);
    return true;
}

bool IqOrmAbstractTriggers::postSave(IqOrmObject *object, IqOrmAbstractTriggers::SaveType saveType, const IqOrmDataSourceOperationResult &operationResult, QString *errorText)
{
    Q_UNUSED(object);
    Q_UNUSED(saveType);
    Q_UNUSED(operationResult);
    Q_UNUSED(errorText);
    return true;
}

bool IqOrmAbstractTriggers::preRemove(IqOrmObject *object, QString *errorText)
{
    Q_UNUSED(object);
    Q_UNUSED(errorText);
    return true;
}

bool IqOrmAbstractTriggers::postRemove(IqOrmObject *object, qint64 objectId, const IqOrmDataSourceOperationResult &operationResult, QString *errorText)
{
    Q_UNUSED(object);
    Q_UNUSED(objectId);
    Q_UNUSED(operationResult);
    Q_UNUSED(errorText);
    return true;
}


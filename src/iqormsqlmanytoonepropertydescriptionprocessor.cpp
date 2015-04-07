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

#include "iqormsqlmanytoonepropertydescriptionprocessor.h"
#include "iqormmanytoonepropertychanges.h"
#include "iqormonetomanypropertychanges.h"

IqOrmSqlManyToOnePropertyDescriptionProcessor::IqOrmSqlManyToOnePropertyDescriptionProcessor() :
    IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor()
{
}

const IqOrmBaseManyToOnePropertyDescription *IqOrmSqlManyToOnePropertyDescriptionProcessor::propertyDescription() const
{
    return qobject_cast<const IqOrmBaseManyToOnePropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QString IqOrmSqlManyToOnePropertyDescriptionProcessor::selectFieldName() const
{
    Q_CHECK_PTR(propertyDescription());

    QString fieldName = escapedObjectTableName();
    fieldName.append(".");
    fieldName.append(escapedJoinColumnName());
    fieldName.append(" AS ");
    fieldName.append(escapedPropertyName(propertyDescription()->propertyName()));
    return fieldName;
}

QString IqOrmSqlManyToOnePropertyDescriptionProcessor::insertFieldName() const
{
    Q_CHECK_PTR(propertyDescription());


    return escapedJoinColumnName();
}

QString IqOrmSqlManyToOnePropertyDescriptionProcessor::updateFieldName() const
{
    return insertFieldName();
}

QVariant IqOrmSqlManyToOnePropertyDescriptionProcessor::insertValue() const
{
    return excapedValue(propertyValue());
}

QVariant IqOrmSqlManyToOnePropertyDescriptionProcessor::updateValue() const
{
    return insertValue();
}

QList<IqOrmSqlJoinOperation> IqOrmSqlManyToOnePropertyDescriptionProcessor::selectJoinOperations() const
{
    return QList<IqOrmSqlJoinOperation>();
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::insertAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if(!checkPropetyValueNotNull(result))
        return false;
    if(!checkPropertyValueObjectIsWasSavedToDb(result))
        return false;

    result->setError("");
    return true;
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::updateAllowed(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return insertAllowed(result);
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::removeAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::preInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::preUpdate(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::preRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::postInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if (propertyValue().isNull()) {
        result->setError("");
        return true;
    }

    qint64 associatedObjectId = propertyValue().toLongLong();

    if (associatedPropertyDescription()) {
        IqOrmOneToManyPropertyChanges *changes = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
        changes->addNewObjectId(object()->objectId());
    }

    IqOrmManyToOnePropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges(propertyDescription());
    changes->setOperation(IqOrmManyToOnePropertyChanges::Set);
    changes->setNewObjectId(associatedObjectId);

    result->setError("");
    return true;
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::postUpdate(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return postInsert(result);
}

bool IqOrmSqlManyToOnePropertyDescriptionProcessor::postRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if (propertyValue().isNull()) {
        result->setError("");
        return true;
    }

    qint64 associatedObjectId = propertyValue().toLongLong();

    if (associatedPropertyDescription()) {
        IqOrmOneToManyPropertyChanges *changes = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
        changes->addRemovedObjectId(object()->objectId());
    }

    return postInsert(result);
}

QString IqOrmSqlManyToOnePropertyDescriptionProcessor::escapedJoinColumnName() const
{
    Q_CHECK_PTR(propertyDescription());

    return escapedFieldName(propertyDescription()->joinColumnName());
}

const IqOrmBaseOneToManyPropertyDescription *IqOrmSqlManyToOnePropertyDescriptionProcessor::associatedPropertyDescription() const
{
    Q_CHECK_PTR(propertyDescription());
    return qobject_cast<const IqOrmBaseOneToManyPropertyDescription *>(propertyDescription()->inversedPropertyDescription());
}

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

#include "iqormsqlmanyobjectsdescribingpropertydescriptionprocessor.h"

IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor() :
    IqOrmSqlMappedPropertyDescriptionProcessor()
{
}

const IqOrmManyObjectDescribingPropertyDescription *IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::propertyDescription() const
{
    return dynamic_cast<const IqOrmManyObjectDescribingPropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QVariant IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::propertyValue() const
{
    Q_CHECK_PTR(propertyDescription());
    return propertyDescription()->valueAsObjectIds(object());
}

QString IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::insertFieldName() const
{
    return "";
}

QString IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::updateFieldName() const
{
    return "";
}

QVariant IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::insertValue() const
{
    return QVariant();
}

QVariant IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::updateValue() const
{
    return QVariant();
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::selectAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::insertAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if(!checkPropertyValueObjectsListIsWasSavedToDb(result))
        return false;

    result->setError("");
    return true;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::preSelect(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::preInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::preUpdate(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    bool ok;
    QSet<qint64> ownerObjectIdsToRemove = ownerObjectIds(result, &ok);
    if (!ok)
        return false;

    if (ownerObjectIdsToRemove.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    foreach (const QVariant &objectId, propertyValue().toList()) {
        ownerObjectIdsToRemove.remove(objectId.toLongLong());
    }

    if (ownerObjectIdsToRemove.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    QHash<qint64, qint64> joinTableChanges;
    bool removeResult = removeOwner(ownerObjectIdsToRemove, result, &joinTableChanges);
    if (removeResult) {
        QHashIterator<qint64, qint64> joinTableChangesI (joinTableChanges);
        while (joinTableChangesI.hasNext()) {
            joinTableChangesI.next();
            addRemovedObjectIdChanges(result, joinTableChangesI.key(), joinTableChangesI.value());
        }
    }

    return removeResult;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::preRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::postSelect(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::postInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);


    if (propertyValue().toList().isEmpty()) {
        result->setError("");
        return true;
    }

    QSet<qint64> ownerObjectIdsToInsert;
    foreach (const QVariant &objectId, propertyValue().toList()) {
        ownerObjectIdsToInsert << objectId.toLongLong();
    }

    if (ownerObjectIdsToInsert.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    QHash<qint64, qint64> joinTableChanges;
    bool setResult = setOwner(ownerObjectIdsToInsert, result, &joinTableChanges);

    if (setResult) {
        QHashIterator<qint64, qint64> joinTableChangesI (joinTableChanges);
        while (joinTableChangesI.hasNext()) {
            joinTableChangesI.next();
            addNewObjectIdChanges(result, joinTableChangesI.key(), joinTableChangesI.value());
        }
    }

    return setResult;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::postUpdate(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    QSet<qint64> ownerObjectIdsToInsert;
    foreach (const QVariant &objectId, propertyValue().toList()) {
        ownerObjectIdsToInsert << objectId.toLongLong();
    }

    if (ownerObjectIdsToInsert.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    bool ok;
    QSet<qint64> oldOwnerObjectIds = ownerObjectIds(result, &ok);
    if (!ok)
        return false;

    foreach (qint64 objectId, oldOwnerObjectIds) {
        ownerObjectIdsToInsert.remove(objectId);
    }

    if (ownerObjectIdsToInsert.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    QHash<qint64, qint64> joinTableChanges;
    bool setResult = setOwner(ownerObjectIdsToInsert, result, &joinTableChanges);
    if (setResult) {
        QHashIterator<qint64, qint64> joinTableChangesI (joinTableChanges);
        while (joinTableChangesI.hasNext()) {
            joinTableChangesI.next();
            addNewObjectIdChanges(result, joinTableChangesI.key(), joinTableChangesI.value());
        }
    }

    return setResult;
}

bool IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor::postRemove(IqOrmDataSourceOperationResult *result) const
{
    bool ok;
    QSet<qint64> ownerObjectIdsToRemove = ownerObjectIds(result, &ok);
    if (!ok)
        return false;

    if (ownerObjectIdsToRemove.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return removeOwner(ownerObjectIdsToRemove, result);
}

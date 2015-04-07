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

#include "iqormsqlmanytomanypropertydescriptionprocessor.h"
#include "iqormsqlmanytomanypropertychanges.h"
#include "iqormsqlinversedmanytomanypropertychanges.h"
#include <QPair>
#include <QDebug>

IqOrmSqlManyToManyPropertyDescriptionProcessor::IqOrmSqlManyToManyPropertyDescriptionProcessor() :
    IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor()
{
}

const IqOrmBaseManyToManyPropertyDescription *IqOrmSqlManyToManyPropertyDescriptionProcessor::propertyDescription() const
{
    return qobject_cast<const IqOrmBaseManyToManyPropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QString IqOrmSqlManyToManyPropertyDescriptionProcessor::selectFieldName() const
{
    Q_CHECK_PTR(associatedOrmModel());

    IqOrmJoinTable *joinTable = propertyDescription()->joinTable();
    Q_CHECK_PTR(joinTable);
    Q_ASSERT(joinTable->isValid());
    QString joinTableName = escapedTableName(joinTable->name());

    QString joinCondition1 = escapedObjectTableName();
    joinCondition1.append(".");
    joinCondition1.append(escapedIdFieldName());
    joinCondition1.append("=");
    joinCondition1.append(joinTableName);
    joinCondition1.append(".");
    joinCondition1.append(escapedTableName(joinTable->inverseJoinColumnName()));

    QString joinCondition2 = escapedAssociatedObjectTableName();
    joinCondition2.append(".");
    joinCondition2.append(escapedIdFieldName());
    joinCondition2.append("=");
    joinCondition2.append(joinTableName);
    joinCondition2.append(".");
    joinCondition2.append(escapedTableName(joinTable->joinColumnName()));

    bool needJoin = false;
    QString fieldName = generateFieldStringAggColumn(escapedAssociatedObjectTableName(),
                                                     escapedIdFieldName(),
                                                     joinCondition1 + " AND " + joinCondition2,
                                                     escapedPropertyName(propertyDescription()->propertyName()),
                                                     &needJoin);

    if (fieldName.isEmpty()) {
        qWarning() << QObject::tr("In database type %0 many to many reference not supported.")
                      .arg(sqlDataSource()->database().driverName());
    }

    return fieldName;
}

QList<IqOrmSqlJoinOperation> IqOrmSqlManyToManyPropertyDescriptionProcessor::selectJoinOperations() const
{
    Q_CHECK_PTR(associatedOrmModel());
    IqOrmJoinTable *joinTable = propertyDescription()->joinTable();
    Q_CHECK_PTR(joinTable);
    Q_ASSERT(joinTable->isValid());
    QString joinTableName = escapedTableName(joinTable->name());

    QString joinCondition1 = escapedObjectTableName();
    joinCondition1.append(".");
    joinCondition1.append(escapedIdFieldName());
    joinCondition1.append("=");
    joinCondition1.append(joinTableName);
    joinCondition1.append(".");
    joinCondition1.append(escapedTableName(joinTable->inverseJoinColumnName()));

    QString joinCondition2 = escapedAssociatedObjectTableName();
    joinCondition2.append(".");
    joinCondition2.append(escapedIdFieldName());
    joinCondition2.append("=");
    joinCondition2.append(joinTableName);
    joinCondition2.append(".");
    joinCondition2.append(escapedTableName(joinTable->joinColumnName()));

    bool needJoin = false;
    QString fieldName = generateFieldStringAggColumn(escapedAssociatedObjectTableName(),
                                                     escapedIdFieldName(),
                                                     joinCondition1 + " AND " + joinCondition2,
                                                     escapedPropertyName(propertyDescription()->propertyName()),
                                                     &needJoin);

    if (fieldName.isEmpty()) {
        qWarning() << QObject::tr("In database type %0 many to many reference not supported.")
                      .arg(sqlDataSource()->database().driverName());

        return QList<IqOrmSqlJoinOperation>();
    }


    QList<IqOrmSqlJoinOperation> joinOperations;
    if (needJoin) {
        IqOrmSqlJoinOperation joinOperation1;
        joinOperation1.setType(IqOrmSqlJoinOperation::LeftOuter);
        joinOperation1.setTable(joinTableName);
        joinOperation1.setCondition(joinCondition1);

        IqOrmSqlJoinOperation joinOperation2;
        joinOperation2.setType(IqOrmSqlJoinOperation::LeftOuter);
        joinOperation2.setTable(escapedAssociatedObjectTableName());
        joinOperation2.setCondition(joinCondition2);

        joinOperations << joinOperation1 << joinOperation2;
    }

    return joinOperations;
}

bool IqOrmSqlManyToManyPropertyDescriptionProcessor::updateAllowed(IqOrmDataSourceOperationResult *result) const
{
    if(!checkPropertyValueObjectsListIsWasSavedToDb(result))
        return false;

    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlManyToManyPropertyDescriptionProcessor::removeAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

const IqOrmBaseInversedManyToManyPropertyDescription *IqOrmSqlManyToManyPropertyDescriptionProcessor::associatedPropertyDescription() const
{
    Q_CHECK_PTR(propertyDescription());

    return qobject_cast<const IqOrmBaseInversedManyToManyPropertyDescription *>(propertyDescription()->inversedPropertyDescription());
}

QString IqOrmSqlManyToManyPropertyDescriptionProcessor::escapedAssociatedObjectTableName() const
{
    Q_CHECK_PTR(associatedOrmModel());
    return escapedTableName(associatedOrmModel()->tableName());
}

QSet<qint64> IqOrmSqlManyToManyPropertyDescriptionProcessor::ownerObjectIds(IqOrmDataSourceOperationResult *result, bool *ok) const
{
    Q_CHECK_PTR(result);

    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(propertyDescription()->joinTable());

    QString preparedQuery = "SELECT ";
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->joinColumnName()));
    preparedQuery.append("\n    FROM ");
    preparedQuery.append(escapedTableName(propertyDescription()->joinTable()->name()));
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->inverseJoinColumnName()));
    preparedQuery.append("=? ");

    QVariantList bindValues;
    bindValues << object()->objectId();

    QString error;
    QSqlQuery resultQuery = execQuery(preparedQuery, bindValues, true, ok, &error);
    if (!(*ok)) {
        result->setError(error);
        return QSet<qint64>();
    }

    if (resultQuery.size() == 0) {
        result->setError("");
        return QSet<qint64>();
    }

    QSet<qint64> resultOwnerIds;
    while (resultQuery.next()) {
        resultOwnerIds << resultQuery.value(0).toLongLong();
    }

    result->setError("");
    *ok = true;
    return resultOwnerIds;
}

bool IqOrmSqlManyToManyPropertyDescriptionProcessor::removeOwner(QSet<qint64> ownerIds,
                                                                 IqOrmDataSourceOperationResult *result,
                                                                 QHash<qint64, qint64> *joinTableChanges) const
{
    Q_CHECK_PTR(result);

    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(propertyDescription()->joinTable());
    Q_CHECK_PTR(object());

    QString preparedQuery = "SELECT ";
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append(", ");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->inverseJoinColumnName()));
    preparedQuery.append(", ");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->joinColumnName()));
    preparedQuery.append("\n    FROM ");
    preparedQuery.append(escapedTableName(propertyDescription()->joinTable()->name()));
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->inverseJoinColumnName()));
    preparedQuery.append("=? AND ");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->joinColumnName()));
    preparedQuery.append(" IN (");

    QVariantList bindValues;
    bindValues << object()->objectId();
    QStringList questList;
    foreach (qint64 objectId, ownerIds) {
        bindValues << objectId;
        questList << "?";
    }

    preparedQuery.append(questList.join(","));
    preparedQuery.append(")");

    bool ok;
    QString error;
    QSqlQuery query = execQuery(preparedQuery, bindValues, true, &ok, &error);
    if (!ok) {
        result->setError(error);
        return false;
    }

    QHash<qint64, QPair<qint64, qint64> > removedJoinTableChanges;
    while (query.next()) {
        removedJoinTableChanges[query.value(0).toLongLong()] = QPair<qint64, qint64>(query.value(1).toLongLong(),
                                                                                     query.value(2).toLongLong());
    }

    preparedQuery = "DELETE ";
    preparedQuery.append("\n    FROM ");
    preparedQuery.append(escapedTableName(propertyDescription()->joinTable()->name()));
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append(" IN (");

    bindValues.clear();
    questList.clear();
    QHashIterator<qint64, QPair<qint64, qint64> > removedJointTableChangesI (removedJoinTableChanges);
    while (removedJointTableChangesI.hasNext()) {
        removedJointTableChangesI.next();
        bindValues << removedJointTableChangesI.key();
        questList << "?";
    }

    preparedQuery.append(questList.join(","));
    preparedQuery.append(")");

    execQuery(preparedQuery, bindValues, true, &ok, &error);
    if (!ok) {
        result->setError(error);
        return false;
    }

    if (associatedPropertyDescription()) {
        removedJointTableChangesI.toFront();
        while (removedJointTableChangesI.hasNext()) {
            removedJointTableChangesI.next();
            IqOrmSqlInversedManyToManyPropertyChanges *changes = result->changes(associatedOrmModel(), removedJointTableChangesI.value().second)->propertyChanges<IqOrmSqlInversedManyToManyPropertyChanges>(associatedPropertyDescription());
            changes->addRemovedObjectId(removedJointTableChangesI.value().first, removedJointTableChangesI.key());
        }
    }

    if (joinTableChanges) {
        joinTableChanges->clear();
        removedJointTableChangesI.toFront();
        while (removedJointTableChangesI.hasNext()) {
            removedJointTableChangesI.next();
            joinTableChanges->insert(removedJointTableChangesI.value().second, removedJointTableChangesI.key());
        }
    }

    result->setError("");
    return true;
}

bool IqOrmSqlManyToManyPropertyDescriptionProcessor::setOwner(QSet<qint64> ownerIds,
                                                              IqOrmDataSourceOperationResult *result,
                                                              QHash<qint64, qint64> *joinTableChanges) const
{
    Q_CHECK_PTR(result);

    Q_CHECK_PTR (propertyDescription());
    Q_CHECK_PTR (propertyDescription()->joinTable());

    QString preparedQuery = "INSERT INTO ";
    preparedQuery.append(escapedTableName(propertyDescription()->joinTable()->name()));
    preparedQuery.append("(");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->inverseJoinColumnName()));
    preparedQuery.append(", ");
    preparedQuery.append(escapedFieldName(propertyDescription()->joinTable()->joinColumnName()));
    preparedQuery.append(")");
    preparedQuery.append("\n    VALUES (?, ?)");

    QSqlQuery query;
    QString error;
    if (!prepareQuery(query, preparedQuery, &error)) {
        result->setError(error);
        return false;
    }

    QHash<qint64, qint64> localJoinTableChanges;

    QVariantList bindValues;
    foreach (qint64 objectId, ownerIds) {
        bindValues.clear();
        bindValues << object()->objectId() << objectId;
        if (!execPreparedQuery(query, bindValues, &error)) {
            result->setError(error);
            return false;
        }

        localJoinTableChanges[objectId] = query.lastInsertId().toLongLong();
    }

    if (associatedPropertyDescription()) {
        QHashIterator<qint64, qint64> localJoinTableChangesI (localJoinTableChanges);
        while (localJoinTableChangesI.hasNext()) {
            localJoinTableChangesI.next();
            IqOrmSqlInversedManyToManyPropertyChanges *changes = result->changes(associatedOrmModel(), localJoinTableChangesI.key())
                    ->propertyChanges<IqOrmSqlInversedManyToManyPropertyChanges>(associatedPropertyDescription());
            changes->addNewObjectId(object()->objectId(), localJoinTableChangesI.value());
        }
    }

    if (joinTableChanges) {
        joinTableChanges->clear();
        QHashIterator<qint64, qint64> localJoinTableChangesI (localJoinTableChanges);
        while (localJoinTableChangesI.hasNext()) {
            localJoinTableChangesI.next();
            joinTableChanges->insert(localJoinTableChangesI.key(), localJoinTableChangesI.value());
        }
    }

    result->setError("");
    return true;
}

void IqOrmSqlManyToManyPropertyDescriptionProcessor::addNewObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                                                           qint64 objectId,
                                                                           qint64 joinTableRowId) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(object());

    IqOrmSqlManyToManyPropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges<IqOrmSqlManyToManyPropertyChanges>(propertyDescription());
    changes->addNewObjectId(objectId, joinTableRowId);
}

void IqOrmSqlManyToManyPropertyDescriptionProcessor::addRemovedObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                                                               qint64 objectId,
                                                                               qint64 joinTableRowId) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(object());

    IqOrmSqlManyToManyPropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges<IqOrmSqlManyToManyPropertyChanges>(propertyDescription());
    changes->addRemovedObjectId(objectId, joinTableRowId);
}

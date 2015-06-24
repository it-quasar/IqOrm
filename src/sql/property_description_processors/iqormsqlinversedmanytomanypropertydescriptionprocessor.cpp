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

#include "iqormsqlinversedmanytomanypropertydescriptionprocessor.h"
#include "iqormsqlmanytomanypropertychanges.h"
#include "iqormsqlinversedmanytomanypropertychanges.h"
#include "iqormmetamodelprivateaccessor.h"
#include <QPair>
#include <QDebug>

using namespace IqOrmPrivate;

IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::IqOrmSqlInversedManyToManyPropertyDescriptionProcessor() :
    IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor()
{
}

const IqOrmBaseInversedManyToManyPropertyDescription *IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::propertyDescription() const
{
    return qobject_cast<const IqOrmBaseInversedManyToManyPropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QString IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::selectFieldName() const
{

    Q_CHECK_PTR(associatedOrmModel());

    IqOrmJoinTable *joinTable = associatedPropertyDescription()->joinTable();
    Q_CHECK_PTR(joinTable);
    Q_ASSERT(joinTable->isValid());
    QString escapedJoinTableName = escapedTableName(joinTable->name());

    QString joinCondition1 = escapedObjectTableName();
    joinCondition1.append(".");
    joinCondition1.append(escapedIdFieldName());
    joinCondition1.append("=");
    joinCondition1.append(escapedJoinTableName);
    joinCondition1.append(".");
    joinCondition1.append(escapedTableName(joinTable->joinColumnName()));

    bool needJoin = false;
    QString fieldName = generateFieldStringAggColumn(escapedJoinTableName,
                                                     escapedTableName(joinTable->inverseJoinColumnName()),
                                                     joinCondition1,
                                                     escapedPropertyName(propertyDescription()->propertyName()),
                                                     &needJoin);


//    QString joinCondition2 = escapedAssociatedObjectTableName();
//    joinCondition2.append(".");
//    joinCondition2.append(escapedIdFieldName());
//    joinCondition2.append("=");
//    joinCondition2.append(joinTableName);
//    joinCondition2.append(".");
//    joinCondition2.append(escapedTableName(joinTable->inverseJoinColumnName()));

//    bool needJoin = false;
//    QString fieldName = generateFieldStringAggColumn(escapedAssociatedObjectTableName(),
//                                                     escapedIdFieldName(),
//                                                     joinCondition1 + " AND " + joinCondition2,
//                                                     escapedPropertyName(propertyDescription()->propertyName()),
//                                                     &needJoin);

    if (fieldName.isEmpty()) {
        qWarning() << QObject::tr("In database type %0 many to many reference not supported.")
                      .arg(sqlDataSource()->database().driverName());
    }

    return fieldName;
}

QList<IqOrmSqlJoinOperation> IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::selectJoinOperations() const
{

    Q_CHECK_PTR(associatedOrmModel());

    IqOrmJoinTable *joinTable = associatedPropertyDescription()->joinTable();
    Q_CHECK_PTR(joinTable);
    Q_ASSERT(joinTable->isValid());
    QString escapedJoinTableName = escapedTableName(joinTable->name());

    QString joinCondition1 = escapedObjectTableName();
    joinCondition1.append(".");
    joinCondition1.append(escapedIdFieldName());
    joinCondition1.append("=");
    joinCondition1.append(escapedJoinTableName);
    joinCondition1.append(".");
    joinCondition1.append(escapedTableName(joinTable->joinColumnName()));

    bool needJoin = false;
    QString fieldName = generateFieldStringAggColumn(escapedJoinTableName,
                                                     escapedTableName(joinTable->inverseJoinColumnName()),
                                                     joinCondition1,
                                                     escapedPropertyName(propertyDescription()->propertyName()),
                                                     &needJoin);

//    QString joinCondition2 = escapedAssociatedObjectTableName();
//    joinCondition2.append(".");
//    joinCondition2.append(escapedIdFieldName());
//    joinCondition2.append("=");
//    joinCondition2.append(joinTableName);
//    joinCondition2.append(".");
//    joinCondition2.append(escapedTableName(joinTable->inverseJoinColumnName()));

//    bool needJoin = false;
//    QString fieldName = generateFieldStringAggColumn(escapedAssociatedObjectTableName(),
//                                                     escapedIdFieldName(),
//                                                     joinCondition1 + " AND " + joinCondition2,
//                                                     escapedPropertyName(propertyDescription()->propertyName()),
//                                                     &needJoin);

    if (fieldName.isEmpty()) {
        qWarning() << QObject::tr("In database type %0 many to many reference not supported.")
                      .arg(sqlDataSource()->database().driverName());

        return QList<IqOrmSqlJoinOperation>();
    }

    QList<IqOrmSqlJoinOperation> joinOperations;
    if (needJoin) {
        IqOrmSqlJoinOperation joinOperation1;
        joinOperation1.setType(IqOrmSqlJoinOperation::LeftOuter);
        joinOperation1.setTable(escapedJoinTableName);
        joinOperation1.setCondition(joinCondition1);

        joinOperations << joinOperation1;
//        IqOrmSqlJoinOperation joinOperation2;
//        joinOperation2.setType(IqOrmSqlJoinOperation::LeftOuter);
//        joinOperation2.setTable(escapedAssociatedObjectTableName());
//        joinOperation2.setCondition(joinCondition2);

//        joinOperations << joinOperation1 << joinOperation2;
    }

    return joinOperations;
}

bool IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::updateAllowed(IqOrmDataSourceOperationResult *result) const
{
    if(!checkPropertyValueObjectsListIsWasSavedToDb(result))
        return false;

    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::removeAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

const IqOrmBaseManyToManyPropertyDescription *IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::associatedPropertyDescription() const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(associatedOrmModel());
    const IqOrmPropertyDescription *result = IqOrmMetaModelPrivateAccessor::propertyDescription(associatedOrmModel(), propertyDescription()->mappedBy());
    return qobject_cast<const IqOrmBaseManyToManyPropertyDescription *>(result);
}

QString IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::escapedAssociatedObjectTableName() const
{
    Q_CHECK_PTR(associatedOrmModel());
    return escapedTableName(associatedOrmModel()->tableName());
}

QSet<qint64> IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::ownerObjectIds(IqOrmDataSourceOperationResult *result, bool *ok) const
{
    Q_CHECK_PTR(result);

    Q_CHECK_PTR(associatedPropertyDescription());
    Q_CHECK_PTR(associatedPropertyDescription()->joinTable());

    QString preparedQuery = "SELECT ";
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->inverseJoinColumnName()));
    preparedQuery.append("\n    FROM ");
    preparedQuery.append(escapedTableName(associatedPropertyDescription()->joinTable()->name()));
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->joinColumnName()));
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

bool IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::removeOwner(QSet<qint64> ownerIds,
                                                                         IqOrmDataSourceOperationResult *result,
                                                                         QHash<qint64, qint64> *joinTableChanges) const
{
    Q_CHECK_PTR(result);

    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(associatedPropertyDescription());
    Q_CHECK_PTR(associatedPropertyDescription()->joinTable());
    Q_CHECK_PTR(object());

    QString preparedQuery = "SELECT ";
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append(", ");
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->joinColumnName()));
    preparedQuery.append(", ");
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->inverseJoinColumnName()));
    preparedQuery.append("\n    FROM ");
    preparedQuery.append(escapedTableName(associatedPropertyDescription()->joinTable()->name()));
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->joinColumnName()));
    preparedQuery.append("=? AND ");
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->inverseJoinColumnName()));
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
    preparedQuery.append(escapedTableName(associatedPropertyDescription()->joinTable()->name()));
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

    removedJointTableChangesI.toFront();
    while (removedJointTableChangesI.hasNext()) {
        removedJointTableChangesI.next();
        IqOrmSqlManyToManyPropertyChanges *changes = result->changes(associatedOrmModel(), removedJointTableChangesI.value().second)->propertyChanges<IqOrmSqlManyToManyPropertyChanges>(associatedPropertyDescription());
        changes->addRemovedObjectId(removedJointTableChangesI.value().first, removedJointTableChangesI.key());
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

bool IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::setOwner(QSet<qint64> ownerIds,
                                                                      IqOrmDataSourceOperationResult *result,
                                                                      QHash<qint64, qint64> *joinTableChanges) const
{
    Q_CHECK_PTR(result);

    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(associatedPropertyDescription());
    Q_CHECK_PTR(associatedPropertyDescription()->joinTable());

    QString preparedQuery = "INSERT INTO ";
    preparedQuery.append(escapedTableName(associatedPropertyDescription()->joinTable()->name()));
    preparedQuery.append("(");
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->joinColumnName()));
    preparedQuery.append(", ");
    preparedQuery.append(escapedFieldName(associatedPropertyDescription()->joinTable()->inverseJoinColumnName()));
    preparedQuery.append(")");
    preparedQuery.append("\n    VALUES (?, ?)");

    bool ok;
    QString error;
    QSqlQuery query = prepareQuery(preparedQuery, &ok, &error);
    if (!ok) {
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

    QHashIterator<qint64, qint64> localJoinTableChangesI (localJoinTableChanges);
    while (localJoinTableChangesI.hasNext()) {
        localJoinTableChangesI.next();
        IqOrmSqlManyToManyPropertyChanges *changes = result->changes(associatedOrmModel(), localJoinTableChangesI.key())
                ->propertyChanges<IqOrmSqlManyToManyPropertyChanges>(associatedPropertyDescription());
        changes->addNewObjectId(object()->objectId(), localJoinTableChangesI.value());
    }

    if (joinTableChanges) {
        joinTableChanges->clear();
        localJoinTableChangesI.toFront();
        while (localJoinTableChangesI.hasNext()) {
            localJoinTableChangesI.next();
            joinTableChanges->insert(localJoinTableChangesI.key(), localJoinTableChangesI.value());
        }
    }

    result->setError("");
    return true;
}

void IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::addNewObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                                                                   qint64 objectId,
                                                                                   qint64 joinTableRowId) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(object());

    IqOrmSqlInversedManyToManyPropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges<IqOrmSqlInversedManyToManyPropertyChanges>(propertyDescription());
    changes->addNewObjectId(objectId, joinTableRowId);
}

void IqOrmSqlInversedManyToManyPropertyDescriptionProcessor::addRemovedObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                                                                       qint64 objectId,
                                                                                       qint64 joinTableRowId) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(object());

    IqOrmSqlInversedManyToManyPropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges<IqOrmSqlInversedManyToManyPropertyChanges>(propertyDescription());
    changes->addRemovedObjectId(objectId, joinTableRowId);
}

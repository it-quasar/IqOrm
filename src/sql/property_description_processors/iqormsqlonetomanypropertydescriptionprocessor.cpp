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

#include "iqormsqlonetomanypropertydescriptionprocessor.h"
#include "iqormonetomanypropertychanges.h"
#include "iqormmanytoonepropertychanges.h"
#include "iqormmetamodelprivateaccessor.h"
#include <QDebug>

using namespace IqOrmPrivate;

IqOrmSqlOneToManyPropertyDescriptionProcessor::IqOrmSqlOneToManyPropertyDescriptionProcessor() :
    IqOrmSqlManyObjectsDescribingPropertyDescriptionProcessor()
{
}

const IqOrmBaseOneToManyPropertyDescription *IqOrmSqlOneToManyPropertyDescriptionProcessor::propertyDescription() const
{
    return qobject_cast<const IqOrmBaseOneToManyPropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QString IqOrmSqlOneToManyPropertyDescriptionProcessor::selectFieldName() const
{
    Q_CHECK_PTR(propertyDescription());


    QString joinCondition = escapedAssociatedObjectTableName();
    joinCondition.append(".");
    joinCondition.append(escapedAssociatedJoinColumnName());
    joinCondition.append(" = ");
    joinCondition.append(escapedObjectTableName());
    joinCondition.append(".");
    joinCondition.append(escapedIdFieldName());

    bool needJoin = false;
    QString fildName = generateFieldStringAggColumn(escapedAssociatedObjectTableName(),
                                                    escapedIdFieldName(),
                                                    joinCondition,
                                                    escapedPropertyName(propertyDescription()->propertyName()),
                                                    &needJoin);

    if (fildName.isEmpty()) {
        qWarning() << QObject::tr("In database type %0 many to one reference not supported.")
                      .arg(sqlDataSource()->database().driverName());
        return "";
    }

    return fildName;
}

QList<IqOrmSqlJoinOperation> IqOrmSqlOneToManyPropertyDescriptionProcessor::selectJoinOperations() const
{
    Q_CHECK_PTR(propertyDescription());


    QString joinCondition = escapedAssociatedObjectTableName();
    joinCondition.append(".");
    joinCondition.append(escapedAssociatedJoinColumnName());
    joinCondition.append(" = ");
    joinCondition.append(escapedObjectTableName());
    joinCondition.append(".");
    joinCondition.append(escapedIdFieldName());

    bool needJoin = false;
    generateFieldStringAggColumn(escapedAssociatedObjectTableName(),
                                 escapedIdFieldName(),
                                 joinCondition,
                                 escapedPropertyName(propertyDescription()->propertyName()),
                                 &needJoin);

    QList<IqOrmSqlJoinOperation> joinOperations;
    if (needJoin) {
        IqOrmSqlJoinOperation joinOperation;
        joinOperation.setType(IqOrmSqlJoinOperation::LeftOuter);
        joinOperation.setTable(escapedAssociatedObjectTableName());
        joinOperation.setCondition(joinCondition);

        joinOperations << joinOperation;
    }
    return joinOperations;
}

bool IqOrmSqlOneToManyPropertyDescriptionProcessor::updateAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(associatedPropertyDescription());

    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    if(!checkPropertyValueObjectsListIsWasSavedToDb(result))
        return false;

    return removeAllowed(result);
}

bool IqOrmSqlOneToManyPropertyDescriptionProcessor::removeAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(associatedPropertyDescription());
    if (associatedPropertyDescription()->notNull()) {
        //Если свойство не равно нулю, то проверим, чтоб в БД не было объектов с данным ид
        //иначе после обновления их свойство станет равно нулю, что не верно
        bool ok;

        QSet<qint64> brokenObjectIds = ownerObjectIds(result, &ok);
        if (!ok)
            return false;

        foreach (const QVariant &objectId, propertyValue().toList()) {
            brokenObjectIds.remove(objectId.toLongLong());
        }

        if (!brokenObjectIds.isEmpty()) {
            QStringList ids;
            foreach (qint64 objectId, brokenObjectIds) {
                ids << QString::number(objectId);
            }
            //В таблице есть запись с данны ид, ошибка
            result->setError(QObject::tr("%0 with ids %1 can not be NULL.")
                             .arg(IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(associatedOrmModel())->className())
                             .arg(ids.join(", ")));
            return false;
        }
    }

    result->setError("");
    return true;
}

const IqOrmBaseManyToOnePropertyDescription *IqOrmSqlOneToManyPropertyDescriptionProcessor::associatedPropertyDescription() const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(associatedOrmModel());
    const IqOrmPropertyDescription *result = IqOrmMetaModelPrivateAccessor::propertyDescription(associatedOrmModel(), propertyDescription()->mappedBy());
    return qobject_cast<const IqOrmBaseManyToOnePropertyDescription *>(result);
}

QString IqOrmSqlOneToManyPropertyDescriptionProcessor::escapedAssociatedObjectTableName() const
{
    Q_CHECK_PTR(associatedOrmModel());
    Q_ASSERT(!associatedOrmModel()->tableName().isEmpty());
    return escapedTableName(associatedOrmModel()->tableName());
}

QString IqOrmSqlOneToManyPropertyDescriptionProcessor::escapedAssociatedJoinColumnName() const
{

    Q_CHECK_PTR(associatedPropertyDescription());
    Q_ASSERT(!associatedPropertyDescription()->joinColumnName().isEmpty());
    return escapedFieldName(associatedPropertyDescription()->joinColumnName());
}

QSet<qint64> IqOrmSqlOneToManyPropertyDescriptionProcessor::ownerObjectIds(IqOrmDataSourceOperationResult *result,
                                                                                    bool *ok) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(ok);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());

    QString preparedQuery = "SELECT ";
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append("\n    FROM ");
    preparedQuery.append(escapedAssociatedObjectTableName());
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedAssociatedJoinColumnName());
    preparedQuery.append("=?");

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

    QSet<qint64> resultObjectIds;
    while (resultQuery.next()) {
        resultObjectIds << resultQuery.value(0).toLongLong();
    }

    result->setError("");
    *ok = true;
    return resultObjectIds;
}

bool IqOrmSqlOneToManyPropertyDescriptionProcessor::removeOwner(QSet<qint64> ownerIds,
                                                                IqOrmDataSourceOperationResult *result,
                                                                QHash<qint64, qint64> *joinTableChanges) const
{
    Q_UNUSED(joinTableChanges);

    if (ownerIds.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    QString preparedQuery = "UPDATE ";
    preparedQuery.append(escapedAssociatedObjectTableName());
    preparedQuery.append("\n    SET ");
    preparedQuery.append(escapedAssociatedJoinColumnName());
    preparedQuery.append("=?");
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append("\n    IN (");

    QVariantList bindValues;
    bindValues << QVariant(QVariant::LongLong);

    QStringList quetsList;
    foreach (qint64 objectId, ownerIds) {
        quetsList << "?";
        bindValues << objectId;
    }

    preparedQuery.append(quetsList.join(","));
    preparedQuery.append(")");

    bool ok;
    QString error;
    execQuery(preparedQuery, bindValues, true, &ok, &error);
    if (!ok) {
        result->setError(error);
        return false;
    }

    foreach (qint64 associatedObjectId, ownerIds) {
        IqOrmManyToOnePropertyChanges *changes = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
        changes->setOperation(IqOrmManyToOnePropertyChanges::Remove);
    }

    result->setError("");
    return true;
}

bool IqOrmSqlOneToManyPropertyDescriptionProcessor::setOwner(QSet<qint64> ownerIds,
                                                             IqOrmDataSourceOperationResult *result,
                                                             QHash<qint64, qint64> *joinTableChanges) const
{
    Q_UNUSED(joinTableChanges);
    if (ownerIds.isEmpty()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    QString preparedQuery = "UPDATE ";
    preparedQuery.append(escapedAssociatedObjectTableName());
    preparedQuery.append("\n    SET ");
    preparedQuery.append(escapedAssociatedJoinColumnName());
    preparedQuery.append("=?");
    preparedQuery.append("\n    WHERE ");
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append("\n    IN (");

    QVariantList bindValues;
    bindValues << object()->objectId();

    QStringList quetsList;
    foreach (qint64 objectId, ownerIds) {
        quetsList << "?";
        bindValues << objectId;
    }

    preparedQuery.append(quetsList.join(","));
    preparedQuery.append(")");

    bool ok;
    QString error;
    execQuery(preparedQuery, bindValues, true, &ok, &error);
    if (!ok) {
        result->setError(error);
        return false;
    }

    foreach (qint64 associatedObjectId, ownerIds) {
        IqOrmManyToOnePropertyChanges *changes = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
        changes->setOperation(IqOrmManyToOnePropertyChanges::Set);
        changes->setNewObjectId(object()->objectId());
    }

    result->setError("");
    return true;
}

void IqOrmSqlOneToManyPropertyDescriptionProcessor::addNewObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                                                          qint64 objectId,
                                                                          qint64 joinTableRowId) const
{
    Q_UNUSED(joinTableRowId);
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(object());

    IqOrmOneToManyPropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges(propertyDescription());
    changes->addNewObjectId(objectId);
}

void IqOrmSqlOneToManyPropertyDescriptionProcessor::addRemovedObjectIdChanges(IqOrmDataSourceOperationResult *result,
                                                                              qint64 objectId,
                                                                              qint64 joinTableRowId) const
{
    Q_UNUSED(joinTableRowId);
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(object());

    IqOrmOneToManyPropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges(propertyDescription());
    changes->addRemovedObjectId(objectId);
}

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

#include "iqormsqlinversedonetoonepropertydescriptionprocessor.h"
#include "iqorminversedonetoonepropertychanges.h"
#include "iqormonetoonepropertychanges.h"
#include "iqormmetamodelprivateaccessor.h"

using namespace IqOrmPrivate;

IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::IqOrmSqlInversedOneToOnePropertyDescriptionProcessor() :
    IqOrmSqlOneObjectDescrtibingPropertyDescriptionProcessor()
{
}

const IqOrmBaseInversedOneToOnePropertyDescription *IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::propertyDescription() const
{
    return qobject_cast<const IqOrmBaseInversedOneToOnePropertyDescription *>(IqOrmSqlAbstractPropertyDescriptionProcessor::propertyDescription());
}

QString IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::selectFieldName() const
{
    Q_CHECK_PTR(propertyDescription());


    QString fieldName = "MIN(";
    fieldName.append(escapedAssociatedObjectTableName());
    fieldName.append(".");
    fieldName.append(escapedIdFieldName());
    fieldName.append(") AS ");
    fieldName.append(escapedPropertyName(propertyDescription()->propertyName()));
    return fieldName;

}

QString IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::insertFieldName() const
{
    return "";
}

QString IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::updateFieldName() const
{
    return "";
}

QVariant IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::insertValue() const
{
    return QVariant();
}

QVariant IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::updateValue() const
{
    return QVariant();
}

QList<IqOrmSqlJoinOperation> IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::selectJoinOperations() const
{
    Q_CHECK_PTR(propertyDescription());


    QString joinCondition = escapedAssociatedObjectTableName();
    joinCondition.append(".");
    joinCondition.append(escapedAssociatedJoinColumnName());
    joinCondition.append(" = ");
    joinCondition.append(escapedObjectTableName());
    joinCondition.append(".");
    joinCondition.append(escapedIdFieldName());

    IqOrmSqlJoinOperation joinOperation;
    joinOperation.setType(IqOrmSqlJoinOperation::LeftOuter);
    joinOperation.setTable(escapedAssociatedObjectTableName());
    joinOperation.setCondition(joinCondition);

    QList<IqOrmSqlJoinOperation> result;
    result << joinOperation;
    return result;
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::insertAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    if(!checkPropertyValueObjectIsWasSavedToDb(result))
        return false;

    result->setError("");
    return true;
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::updateAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(associatedPropertyDescription());

    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    if(!checkPropertyValueObjectIsWasSavedToDb(result))
        return false;

    return removeAllowed(result);
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::removeAllowed(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(associatedPropertyDescription());
    if (associatedPropertyDescription()->notNull()) {
        //Если свойство не равно нулю, то проверим, чтоб в БД не было объектов с данным ид
        //иначе после обновления их свойство станет равно нулю, что не верно
        bool ok;

        qint64 brokenObjectId = oldObjectOwnerObjectId(result, &ok);
        if (!ok)
            return false;

        if (brokenObjectId != -1) {
            //В таблице есть запись с данны ид, ошибка
            result->setError(QObject::tr("%0 with id %1 can not be NULL.")
                             .arg(IqOrmMetaModelPrivateAccessor::targetStaticMetaObject(associatedOrmModel())->className())
                             .arg(brokenObjectId)
                             .arg(associatedPropertyDescription()->propertyName()));
            return false;
        }
    }

    result->setError("");
    return true;
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::preInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    result->setError("");
    return true;
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::preUpdate(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return preRemove(result);
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::preRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(object());
    Q_CHECK_PTR(object()->ormMetaModel());

    bool ok;
    qint64 oldOwnerObjectId = oldObjectOwnerObjectId(result, &ok);
    if (!ok)
        return false;

    if (oldOwnerObjectId != -1) {
        QString preparedQuery = "UPDATE ";
        preparedQuery.append(escapedAssociatedObjectTableName());
        preparedQuery.append("\n    SET ");
        preparedQuery.append(escapedAssociatedJoinColumnName());
        preparedQuery.append("=?");
        preparedQuery.append("\n    WHERE ");
        preparedQuery.append(escapedIdFieldName());
        preparedQuery.append("=?");

        QVariantList bindValues;
        bindValues << QVariant(QVariant::LongLong);
        bindValues << oldOwnerObjectId;

        QString error;
        execQuery(preparedQuery, bindValues, true, &ok, &error);
        if (!ok) {
            result->setError(error);
            return false;
        }

        IqOrmOneToOnePropertyChanges *changes = result->changes(associatedOrmModel(), oldOwnerObjectId)->propertyChanges(associatedPropertyDescription());
        changes->setOperation(IqOrmOneToOnePropertyChanges::Remove);
    }

    result->setError("");
    return true;
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::postInsert(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);

    if (propertyValue().isNull()) {
        result->setError("");
        return true;
    }

    qint64 associatedObjectId = propertyValue().toLongLong();

    QString preparedQuery = "UPDATE ";
    preparedQuery.append(escapedAssociatedObjectTableName());
    preparedQuery.append(" SET ");
    preparedQuery.append(escapedAssociatedJoinColumnName());
    preparedQuery.append("=?");
    preparedQuery.append(" WHERE ");
    preparedQuery.append(escapedIdFieldName());
    preparedQuery.append("=?");

    QVariantList bindValues;
    bindValues << object()->objectId();
    bindValues << associatedObjectId;

    bool ok;
    QString error;
    execQuery(preparedQuery, bindValues, true, &ok, &error);
    if (!ok) {
        result->setError(error);
        return false;
    }

    IqOrmOneToOnePropertyChanges *associatedObjectChanges = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
    associatedObjectChanges->setOperation(IqOrmOneToOnePropertyChanges::Set);
    associatedObjectChanges->setNewObjectId(object()->objectId());

    IqOrmInversedOneToOnePropertyChanges *changes = result->changes(ormModel(), object()->objectId())->propertyChanges(propertyDescription());
    changes->setOperation(IqOrmOneToOnePropertyChanges::Set);
    changes->setNewObjectId(associatedObjectId);

    result->setError("");
    return true;
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::postUpdate(IqOrmDataSourceOperationResult *result) const
{
    if (!propertyWasChanged()) {
        Q_CHECK_PTR(result);
        result->setError("");
        return true;
    }

    return postInsert(result);
}

bool IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::postRemove(IqOrmDataSourceOperationResult *result) const
{
    Q_CHECK_PTR(result);

    if (propertyValue().isNull()) {
        result->setError("");
        return true;
    }

    qint64 associatedObjectId = propertyValue().toLongLong();

    IqOrmOneToOnePropertyChanges *associatedObjectChanges = result->changes(associatedOrmModel(), associatedObjectId)->propertyChanges(associatedPropertyDescription());
    associatedObjectChanges->setPropertyDescription(associatedPropertyDescription());
    associatedObjectChanges->setOperation(IqOrmOneToOnePropertyChanges::Remove);

    result->setError("");
    return true;
}

const IqOrmBaseOneToOnePropertyDescription *IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::associatedPropertyDescription() const
{
    Q_CHECK_PTR(propertyDescription());
    Q_CHECK_PTR(associatedOrmModel());
    const IqOrmPropertyDescription *result = IqOrmMetaModelPrivateAccessor::propertyDescription(associatedOrmModel(), propertyDescription()->mappedBy());
    return qobject_cast<const IqOrmBaseOneToOnePropertyDescription *>(result);
}

QString IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::escapedAssociatedObjectTableName() const
{
    Q_CHECK_PTR(associatedOrmModel());
    Q_ASSERT(!associatedOrmModel()->tableName().isEmpty());
    return escapedTableName(associatedOrmModel()->tableName());
}

QString IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::escapedAssociatedJoinColumnName() const
{

    Q_CHECK_PTR(associatedPropertyDescription());
    Q_ASSERT(!associatedPropertyDescription()->joinColumnName().isEmpty());
    return escapedFieldName(associatedPropertyDescription()->joinColumnName());
}

qint64 IqOrmSqlInversedOneToOnePropertyDescriptionProcessor::oldObjectOwnerObjectId(IqOrmDataSourceOperationResult *result,
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
        return -1;
    }

    if (resultQuery.size() == 0) {
        result->setError("");
        return -1;
    }

    resultQuery.next();

    result->setError("");
    return resultQuery.value(0).toLongLong();
}
